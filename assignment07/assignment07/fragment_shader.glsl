#version 430 core

/**
In this implementation, the shading calculations are done on a fragment by fragment basis.
This tends to give better results than computing vertex colors and interpolating (Gouraud shading).

There are three components computed: ambient, diffuse, and specular.

The text book and the notes describe how the calculations approximate actual physical lighting effects.
*/


out vec4 fColor;


in vec4 w_normal;
in vec4 w_position;
in vec2 s_texture_coord;
in vec4 s_color;

in vec4 o_position;

/**
Light attributes and material attributes passed as uniform values.
*/
// light
uniform vec4 u_ambient_light;
uniform vec4 u_specular_light;
uniform vec4 u_diffuse_light;
uniform vec4 u_light_position; // position of the light
uniform vec3 u_camera_position;

// material
uniform vec4 u_ambient_color;
uniform vec4 u_specular_color;
uniform vec4 u_diffuse_color;
uniform float u_specular_shininess;     // exponent for sharpening highlights
uniform float u_specular_strength;      // extra factor to adjust shininess

uniform int u_surface_effect ;
uniform sampler2D  u_sampler;
uniform samplerCube u_cube_sampler;

void main()
{
    vec4 base_color = vec4(1, 1, 0, 1);;
    switch(u_surface_effect)
    {
        case 0:
        base_color = s_color;
        break;

        case 1:
        base_color = u_diffuse_color;
        break;

        case 2:
        base_color = texture(u_sampler, s_texture_coord);
        break;

        case 3:
        base_color = texture(u_cube_sampler, o_position.xyz);
        break;

        case 4:
        vec3 rfl = reflect(w_position.xyz-u_camera_position, normalize(w_normal.xyz));
          fColor = texture(u_cube_sampler, rfl) * vec4(.9);
        break;
    }


    vec3 light_direction = w_position.xyz - u_light_position.xyz;
    vec3 vnn = normalize(w_normal.xyz);
    float diffuse_factor = max(0.0, dot(vnn, -normalize(light_direction)));
    vec3 half_vector = normalize(normalize(-light_direction) - normalize(w_position.xyz - u_camera_position));
    float specular_factor = max(0.0, dot(vnn, half_vector));
    if (diffuse_factor == 0.0)
        specular_factor = 0.0;
    else
        specular_factor = pow(specular_factor, u_specular_shininess) * u_specular_strength;  // sharpen the highlight
    vec4 ambient_component = u_ambient_color * u_ambient_light;
    vec4 diffuse_component = diffuse_factor * base_color * u_diffuse_light;
    vec4 specular_component = specular_factor * u_specular_color * u_specular_light;
    vec4 total = ambient_component + diffuse_component + specular_component;

    if(u_surface_effect == 3)
        fColor = base_color;
    else
        fColor = vec4(total.rgb, 1.0);


}
