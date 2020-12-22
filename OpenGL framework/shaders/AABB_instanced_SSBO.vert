#version 430 core
const uint LEAFCOUNT = 2;
//shades a wired aabb with just the edges + instanced  with SSBO
layout(location = 0) in vec3 a_Pos;

struct AABB { vec4 bbmin; vec4 bbmax; };
//struct TransformRef
//{	
//	mat4 Transform;
//	uint Offset;
//};

layout (std430, binding = 0) readonly buffer BVH_buffer { AABB AABB_buffer[]; };
//layout (std430, binding = 0) readonly buffer TransformBuffer { AABB AABB_buffer[]; };s

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform int u_Offset;
uniform int u_TopLevel;

void main() 
{
	AABB bounds = AABB_buffer[u_Offset + gl_InstanceID];
	uint childCount = floatBitsToUint(bounds.bbmax.w);
	vec3 scale = vec3(0, 0,0);
	
	//if(childCount <= LEAFCOUNT) //only draw leaf nodes
		scale = bounds.bbmax.xyz - bounds.bbmin.xyz;
	vec3 center = 0.5f * (bounds.bbmax.xyz + bounds.bbmin.xyz);

	gl_Position = u_Projection * u_View * u_Model * vec4( a_Pos * scale + center, 1.0f);
};