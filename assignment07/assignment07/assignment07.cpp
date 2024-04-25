/**
 * This is the base project for assignment #7
 */

#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_inverse.hpp>

#include <iostream>


#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "learnopengl/shader_4722.h"

#include "cs4722/artifact.h"
#include "cs4722/view.h"
#include "cs4722/light.h"
#include "cs4722/window.h"
#include "cs4722/buffer_utilities.h"
#include "cs4722/texture_utilities.h"
#include "callbacks.h"

static cs4722::view *the_view;
static Shader *shader;

static std::vector<cs4722::artifact*> artifact_list;

static cs4722::light a_light;


static cs4722::height_field_fnl *terrain_height_field;
static glm::vec3 terrain_scale = glm::vec3(100, 10, 100);
static glm::vec3 terrain_translate = glm::vec3(0,0,0);


void init()
{
    the_view = new cs4722::view();
    the_view->enable_logging = false;
    the_view->camera_position = terrain_scale / 2.0f + glm::vec3(0, 10, 0);
    a_light.ambient_light = cs4722::x11::gray25;
    a_light.light_direction_position = glm::vec4(25,40,25,1);

    shader = new Shader("../vertex_shader.glsl",
                  "../fragment_shader.glsl");
	shader->use();

    glEnable(GL_DEPTH_TEST);



    terrain_height_field = new cs4722::height_field_fnl(200, 200);
    terrain_height_field->frequency = 1.5;
    terrain_height_field->seed = 12345;
//    terrain_height_field->seed = clock();
    terrain_height_field->fractal_type = FastNoiseLite::FractalType_FBm;
    terrain_height_field->noise_type = FastNoiseLite::NoiseType_OpenSimplex2;
    terrain_height_field->number_of_octaves = 4;
//    terrain_height_field->ping_pong_strength = 7;
    terrain_height_field->generate();
    cs4722::shape* b = new cs4722::terrain(terrain_height_field);

    auto* artf = new cs4722::artifact();
    artf->the_shape = b;

    artf->world_transform.scale = terrain_scale;
    artf->world_transform.translate = terrain_translate;
    artifact_list.push_back(artf);

    terrain_height_field->scale = terrain_scale;
    terrain_height_field->translate = terrain_translate;


    //Load texture
    cs4722::init_texture_from_file("../media/wall.png", 1);
    auto wrap_type = GL_MIRRORED_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);

    //2nd texture
    cs4722::init_texture_from_file("../media/Stone.png", 2);
    wrap_type = GL_MIRRORED_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);

    /////////////////////////////////////////////////////////////
    //Cubemap
    cs4722::init_cube_texture_from_path("../media/cubemap", 10, "jpg");

    auto skybox_scale = 40000.0f;

    auto* a_shape = new cs4722::block();
    auto artfb = new cs4722::artifact();
    artfb->the_shape = a_shape;
    artfb->world_transform.scale = glm::vec3(skybox_scale);
    artfb->texture_unit = 10;
    artfb->surface_effect = 3;

    artifact_list.push_back(artfb);
    ///////////////////////////////////////////////////////////
    auto* torus = new cs4722::torus();
    for(auto x = 8; x <= 92; x += 7 ) {
        for(auto z = 8; z <= 92; z += 7 ) {
            auto y = terrain_height_field->altitude(x, z) + 2;

            auto artf = new cs4722::artifact_rotating();
            artf->the_shape = torus;
            artf->world_transform.scale = glm::vec3(2, 2, 2);
            artf->world_transform.translate =  glm::vec3(x, y, z);

            artf->animation_transform.rotation_axis = glm::vec3 (0, 1, 0);
            artf->animation_transform.rotation_center = glm::vec3(x, y, z);
            artf->rotation_rate = M_PI/5;

            //Surface effect
            if(x%2 == 0 ) {
                artf->surface_effect = 4;
                artf->texture_unit = 10;
            }
            else {
                artf->surface_effect = 2;
                artf->texture_unit = 2;
            }
            //Texture unit
            ////if(z%2 == 0)
               // artf->texture_unit = 10;
            //else
                //artf->texture_unit = 2;

            artf->surface_material.specular_strength = z*3.0f/100.0f;
            artf->surface_material.shininess = x*30.0f/100.0f;

            artf->surface_material.diffuse_color =  cs4722::color::color_rgb(0, (255 * x) / 100, (255 * z) / 100);

            //Push artifact
            artifact_list.push_back(artf);
        }
    }


    cs4722::init_buffers(shader->ID, artifact_list, "b_position","b_color",
                         "b_texture_coord", "b_normal");

}



void render()
{
    static auto last_time = 0.0;
    auto time = glfwGetTime();
    auto delta_time = time - last_time;
    last_time = time;

    auto view_transform = glm::lookAt(the_view->camera_position,
                                      the_view->camera_position + the_view->camera_forward,
                                      the_view->camera_up);
    auto projection_transform = glm::infinitePerspective(the_view->perspective_fovy,
                                                         the_view->perspective_aspect,
                                                         the_view->perspective_near);
    auto vp_transform = projection_transform * view_transform;
    shader->setVec4("u_light_position", a_light.light_direction_position);
    shader->setVec3("u_camera_position", the_view->camera_position);

	for (auto artf: artifact_list) {

		artf->animate(time, delta_time);
        auto model_transform = artf->animation_transform.matrix() * artf->world_transform.matrix();

        //
        /**
         *      The model-view-projection transform we have been sending to the vertex shader
         *          is here replaced by two separate parts: the model transform and
         *          the view-projection.
         *          These will be combined in the vertex shader.
         *          But, the model transform is needed there to convert to world coordinates
         *          so it's easier to set the transforms up this way
         */
        shader->setMat4("u_m_transform", model_transform);
        shader->setMat4("u_vp_transform", vp_transform);
        shader->setMat4("u_normal_transform", glm::inverseTranspose(model_transform));


        shader->setVec4("u_ambient_light", a_light.ambient_light.as_float_array());
        shader->setVec4("u_diffuse_light", a_light.diffuse_light.as_float_array());
        shader->setVec4("u_specular_light", a_light.specular_light.as_float_array());
        shader->setVec4("u_ambient_color", artf->surface_material.ambient_color.as_float_array());
        shader->setVec4("u_diffuse_color", artf->surface_material.diffuse_color.as_float_array());
        shader->setVec4("u_specular_color", artf->surface_material.specular_color.as_float_array());
        shader->setFloat("u_specular_shininess", artf->surface_material.shininess);
        shader->setFloat("u_specular_strength", artf->surface_material.specular_strength);

        shader->setInt("u_surface_effect", artf->surface_effect);
        shader->setInt("u_sampler", artf->texture_unit);
        shader->setInt("u_cube_sampler", artf->texture_unit);

        glDrawArrays(GL_TRIANGLES, artf->the_shape->buffer_start,
			artf->the_shape->buffer_size);
		
	}
}




int
main(int argc, char** argv)
{
	glfwInit();
    cs4722::set_opengl_43();
    GLFWwindow *window = cs4722::setup_window_9_16_9("Shading Demonstration");

    gladLoadGL(glfwGetProcAddress);
//	cs4722::setup_debug_callbacks();

	init();
    the_view->perspective_aspect = cs4722::get_aspect_ratio(window);

	glfwSetWindowUserPointer(window, the_view);
    glfwSetKeyCallback(window, general_key_callback);
    glfwSetCursorPosCallback(window, move_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

	while (!glfwWindowShouldClose(window))
	{
        glClearBufferfv(GL_COLOR, 0, cs4722::x11::gray25.as_float_array());
        glClear(GL_DEPTH_BUFFER_BIT);

        render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}
