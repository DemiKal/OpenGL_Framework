#version 330 core
layout (location = 0) in vec3 a_Pos;

out vec3 TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;
//uniform mat4 u_Model; //inverse model

void main()
{
    TexCoords = a_Pos;
    vec4 pos = u_Projection * u_View * vec4(a_Pos, 1.0);
    gl_Position = pos.xyww;
}  