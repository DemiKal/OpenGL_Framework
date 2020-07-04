#version 330 core
layout (location = 0) in vec3 v_Pos;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_Tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_lightProj;

out vec2 texCoords;

void main()
{
    texCoords = v_Tex;
   // gl_Position = u_lightProj *  u_model * vec4(v_Pos, 1.0);
    gl_Position = u_projection *  u_view *  u_model * vec4(v_Pos, 1.0);
   
}