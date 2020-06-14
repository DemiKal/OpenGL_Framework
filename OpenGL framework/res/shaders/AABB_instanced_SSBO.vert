#version 430 core

//shades a wired aabb with just the edges + instanced  with SSBO
layout(location = 0) in vec3 v_pos;
layout(location = 1) in mat4 instance_mat;

struct AABB { vec4 bbmin; vec4 bbmax; };
layout (std430, binding = 0) readonly buffer BVH_buffer { AABB AABB_buffer[]; };

uniform mat4 u_view;
uniform mat4 u_projection;

void main() 
{
	AABB bounds = AABB_buffer[gl_InstanceID];
	vec3 scale = bounds.bbmax.xyz  - bounds.bbmin.xyz;
	vec3 center = 0.5f * (bounds.bbmax.xyz + bounds.bbmin.xyz);
	gl_Position = u_projection * u_view * vec4( v_pos * scale + center, 1.0f);
};