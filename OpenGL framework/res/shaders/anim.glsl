#shader vertex
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
layout(location = 0) in vec3 position;		//0 - 3
layout(location = 1) in vec3 normal;		//3 - 6
layout(location = 2) in vec2 texCoord;		//6 - 8
layout(location = 3) in vec3 aTangent;		//8 - 11
layout(location = 4) in vec3 aBitangent;	//11 - 14
layout(location = 5) in vec3 boneIDs;		//14 - 17			//TODO: convert to ivec3!
layout(location = 6) in vec3 weights;		//17 - 20

uniform mat4 mBones[MAXBONES];

//out vec2 v_TexCoord;
//out vec3 v_normal;
//out vec3 v_pos;
//out vec3 v_aTangent;
//out vec3 v_aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 idxColor;



void main() {
	//mat4 BoneTransform = mat4(1.0f) * mBones[];
	//int idx = int(boneIDs.x);

	//mat4 BoneTransforml = mBones[0];
	mat4 BoneTransform = mBones[int(boneIDs[0])] * weights[0];
	BoneTransform += mBones[int(boneIDs[1])] * weights[1];
	BoneTransform += mBones[int(boneIDs[2])] * weights[2];


	//vec4 pos = 

	vec3 col = vec3(0, 0, 0);
	col += colors[int(boneIDs[0])] * weights[0];
	col += colors[int(boneIDs[1])] * weights[1];
	col += colors[int(boneIDs[2])] * weights[2];

	gl_Position = projection * view * model * BoneTransform * vec4(position, 1.0f);

	idxColor = col;//boneIDs.y / 16.0f, boneIDs.z / 16.0f);
	//id2 = idx;
	//float idx = weights.x;
	//if (idx > 15)		idxColor = vec3(1.0f, 0.0f, 0.0f);
	//else if (idx < 0)	idxColor = vec3(0.0f, 0.0f, 1.0f);
	//else			  	idxColor = vec3(1.0f, 1.0f, 1.0f);
};


///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\



#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_pos;
out vec4 fragColor;

in /*flat*/ vec3 idxColor;

uniform vec4 u_Color;


void main()
{
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
	//fragColor = vec4(1, 0, 1, 1);
	fragColor = vec4(idxColor, 1.0f);
};
