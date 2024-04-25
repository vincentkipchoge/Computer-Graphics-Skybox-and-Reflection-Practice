#version 430 core

in vec4 b_position;
in vec4 b_color;
in vec4 b_normal;
in vec2 b_texture_coord;

uniform mat4 u_m_transform;
uniform mat4 u_vp_transform;
uniform mat4 u_normal_transform;

out vec4 w_normal;
out vec4 w_position;
out vec2 s_texture_coord;
out vec4 s_color;

out vec4 o_position ;

void
main()
{
    s_texture_coord = b_texture_coord;
    // The normal is transformed to world coordinates
    w_normal = u_normal_transform * b_normal;
    // The fragment position in the worldcoordinate frame is needed to compute shading.
    // So, the vertex position in world coordinates is sent to the fragment shader
    //      The corner positions will be interpolated to get the fragment position in world coordinates.
    w_position = u_m_transform * b_position;
    // Projecting the view coordinates finishes the transformation to the default coordinate frame.
    gl_Position =  u_vp_transform * w_position;
    s_color = b_color;
    o_position  = b_position;
}
