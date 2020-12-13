#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

vec3 colors[16] = vec3[](
	vec3(1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, 0, 1),
	vec3(1, 0, 1),
	vec3(1, 1, 0),
	vec3(0, 1, 1),
	vec3(0.5f, 0, 1),
	vec3(1, 0, 0.5f),
	vec3(1, 0.5f, 0),
	vec3(1, 0.5f, 0.5f),
	vec3(0.5f, 1, 0),
	vec3(0.5f, 1, 1),
	vec3(0.5f, 0.5f, 0),
	vec3(0.25f, 0.25f, 1),
	vec3(0.25f, 0.25f, 0.25f),
	vec3(1, 0.25f, 0.75f));


const int MAXBONES = 20;
layout(location = 0) in vec3 a_Position;		//0 - 3
layout(location = 1) in vec3 a_Normal;		//3 - 6
layout(location = 2) in vec2 a_TexCoord;		//6 - 8
//layout(location = 3) in vec3 aTangent;		//8 - 11
//layout(location = 4) in vec3 aBitangent;	//11 - 14
layout(location = 3) in vec4 a_BoneIDs;		//14 - 17			//TODO: convert to ivec3!
layout(location = 4) in vec4 a_Weights;		//17 - 20

uniform mat4 mBones[MAXBONES];

//out vec2 v_TexCoord;
//out vec3 v_normal;
//out vec3 v_pos;
//out vec3 v_aTangent;
//out vec3 v_aBitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;

uniform sampler2D texture_diffuse1;

void main() 
{

	v_TexCoord = a_TexCoord;

	mat4 BoneTransform = mBones[int(a_BoneIDs[0])] * a_Weights[0];
	BoneTransform += mBones[int(a_BoneIDs[1])] * a_Weights[1];
	BoneTransform += mBones[int(a_BoneIDs[2])] * a_Weights[2];
	BoneTransform += mBones[int(a_BoneIDs[3])] * a_Weights[3];

 
	//vec3 col = vec3(0, 0, 0);
	//col += colors[int(a_BoneIDs[0])] * a_Weights[0];
	//col += colors[int(a_BoneIDs[1])] * a_Weights[1];
	//col += colors[int(a_BoneIDs[2])] * a_Weights[2];
	//col += colors[int(a_BoneIDs[3])] * a_Weights[3];

	gl_Position = u_Projection * u_View * u_Model * BoneTransform * vec4(a_Position, 1.0f);
};
