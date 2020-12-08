#version 330 core
layout (location = 0) in vec3 a_Pos;
//layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{

    mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
    vs_out.normal = vec3(vec4( 1, 0, 0, 0.0));
    gl_Position = u_View * u_Model * vec4(a_Pos * vec3(1.01, 1.01, 1.01), 1.0); 
}