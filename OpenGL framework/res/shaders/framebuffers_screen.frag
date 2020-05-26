
#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

const int StackSize = 64;

uniform float u_near_plane;
uniform float u_far_plane;

uniform float u_smoothStepStart;
uniform float u_smoothStepEnd;

uniform vec3 u_cameraPos;
uniform vec3 u_viewDir;
uniform vec3 u_cameraUp;


uniform sampler2D u_positionBuffer;
uniform sampler2D screenTexture;
uniform sampler1D u_triangleTexture;
uniform sampler2D u_depthBuffer;

uniform vec3 u_lightDir;

uniform bool u_useZbuffer;

struct BVHNode
{
	int m_start;	
	int m_end;		//struct BVHNode{
	int m_leftFirst; //	int m_start;		//4	+
	int m_count;	//	int m_end;			//4	+
	vec4 m_min;//	int m_leftFirst;	//4	+
	vec4 m_max;//	int m_count;		//4 = 16
};

layout (std430, binding = 0) buffer BVH_buffer
{
	BVHNode BVH[] ;	
};
 
 struct Trianglee
 {
	vec4 A; //float d1;
	vec4 B;	//float d2;
	vec4 C;	//float d3;
 };

layout (std430, binding = 1) buffer Triangle_buffer
{
	Trianglee triangles[] ;	
};
 
layout (std430, binding = 2) buffer Index_buffer
{
	uint tri_indices[] ;	
};
 
 

//uniform isampler1D u_trianglesTexture;	// 3 floats xyz 

//bvh data int texture
uniform isampler1D u_indexTexture;	// 4 ints 
//int m_start		x ; r		
//int m_end			y ; g	
//int m_leftFirst	z ; b
//int m_count		w ; a	

//two 3-float textures for min/max vectors of aabb
uniform sampler1D u_minTexture;		// 3 floats for min bbox
uniform sampler1D u_maxTexture;		// 3 floats

uniform float u_aspectRatio;
uniform float u_screenWidth;
uniform float u_screenHeight;

uniform mat4 u_inv_projMatrix;
uniform mat4 u_inv_viewMatrix;

uniform usampler1D u_triangleIdxTexture;


struct AABB {
	vec3 m_min;
	vec3 m_max;
};

struct HitData
{
	bool hasHit;
	float u,v, dist;
};

AABB GetAABB(int index)
{
	vec3 minv = texelFetch(u_minTexture, index, 0).xyz;
	vec3 maxv = texelFetch(u_maxTexture, index, 0).xyz;
	return AABB(minv, maxv);
}

uniform AABB u_boundingBox;

HitData triIntersect(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2)
{
	vec3 v1v0 = v1 - v0;
	vec3 v2v0 = v2 - v0;
	vec3 rov0 = ro - v0;

#if 0
	// Cramer's rule for solcing p(t) = ro+t·rd = p(u,v) = vo + u·(v1-v0) + v·(v2-v1)
	float d = 1.0 / determinant(mat3(v1v0, v2v0, -rd));
	float u = d * determinant(mat3(rov0, v2v0, -rd));
	float v = d * determinant(mat3(v1v0, rov0, -rd));
	float t = d * determinant(mat3(v1v0, v2v0, rov0));
#else
	// The four determinants above have lots of terms in common. Knowing the changing
	// the order of the columns/rows doesn't change the volume/determinant, and that
	// the volume is dot(cross(a,b,c)), we can precompute some common terms and reduce
	// it all to:
	vec3  n = cross(v1v0, v2v0);
	vec3  q = cross(rov0, rd);
	float d = 1.0 / dot(rd, n);
	float u = d * dot(-q, v2v0);
	float v = d * dot(q, v1v0);
	float t = d * dot(-n, rov0);
#endif    

	if (u < 0.0 || v < 0.0 || (u + v) > 1.0)
		t = -1.0;

	return HitData(t > -1.0f, u , v, t); 
	 //vec3(t, u, v); 
}

bool IntersectAABB(vec3 rayOrigin, vec3 rayDirection, AABB bbox)
{
	//vec3  direction rayDirection;
	//vec3  origin = rayOrigin;

	vec3 d = vec3(
		1.0f / rayDirection.x,
		1.0f / rayDirection.y,
		1.0f / rayDirection.z);

	float t = 99999999.f;
	float tx1 = (bbox.m_min.x - rayOrigin.x) * d.x;
	float tx2 = (bbox.m_max.x - rayOrigin.x) * d.x;

	float tmin = min(tx1, tx2);
	float tmax = max(tx1, tx2);

	float ty1 = (bbox.m_min.y - rayOrigin.y) * d.y;
	float ty2 = (bbox.m_max.y - rayOrigin.y) * d.y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));

	float tz1 = (bbox.m_min.z - rayOrigin.z) * d.z;
	float tz2 = (bbox.m_max.z - rayOrigin.z) * d.z;

	tmin = max(tmin, min(tz1, tz2));
	tmax = min(tmax, max(tz1, tz2));
	//tCurrent = tmin;
	//return vec3(tmax, tmin)
	return tmax >= max(0.0f, tmin) && tmin < t;
}


vec3 RayFromCam(float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / u_screenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / u_screenHeight;
	float z = 1.0f;

	//normalized device coordinates [-1:1, -1:1, -1:1]
	vec3 ray_nds = vec3(x, y, z);

	// clip space (4d homogenized) [-1:1, -1:1, -1:1, -1:1]
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	// eye space [-x:x, -y:y, -z:z, -w:w]
	vec4 ray_eye = u_inv_projMatrix * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space [-x:x, -y:y, -z:z, -w:w]
	vec3 ray_world = vec3(u_inv_viewMatrix * ray_eye);
	ray_world = normalize(ray_world);

	//Ray ray(this->PositionRead(), ray_world);

	return ray_world;
}


vec4 BoxColor(int index, vec3 rayOrigin, vec3 rayDir)
{
	AABB aabb = GetAABB(index);
	if (IntersectAABB(rayOrigin, rayDir, aabb))
	{
		return vec4(0.1, 0.3, 0.7, 1.0f);
	}

	return vec4(0, 0, 0, 0);
}

struct BVHhit
{
	bool hasHit;
	int depth;
	float u;
	float v;
	float t;
	int triIdx;
	int bbIdx;
};



BVHhit TraverseBVH(vec3 rayOrigin, vec3 rayDir)
{
	BVHhit result;

	 //int size = textureSize(u_indexTexture, 0);
	//int size = BVH.length();
	int currentIdx = 0;

	int stackPtr[StackSize];
	stackPtr[0] = 0; //sentinel null guard value
	currentIdx++;

	//if( IntersectAABB(rayOrigin, rayDir, GetAABB(0) )) 
	//	return true;
	//else return false;
	bool hasHit = false;

	float t = 999999.f;
	float _u = 0;
	float _v = 0; 
	
	//loop---------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------
	while (currentIdx > 0 && currentIdx < StackSize && !hasHit )
	{
		int bboxIdx = stackPtr[currentIdx - 1];
		currentIdx--;
 		BVHNode currentBVH = BVH[bboxIdx];

		int childCount =  currentBVH.m_count;
		bool  isInterior = childCount > 2; //TODO: leaf check
		AABB currentAABB = AABB(currentBVH.m_min.xyz, currentBVH.m_max.xyz);  //GetAABB(bboxIdx);

		if (IntersectAABB(rayOrigin, rayDir, currentAABB))
		{
			if (isInterior)
			{
				int leftChild = currentBVH.m_leftFirst;
				int rightChild = currentBVH.m_leftFirst + 1;
				//BVHNode lBVH = BVH[leftChild];
				//BVHNode rBVH = BVH[rightChild];


				//AABB leftAABB = AABB(lBVH.m_min.xyz, lBVH.m_max.xyz); //GetAABB(leftChild);
				//vec3 centerL = 0.5f * leftAABB.m_min + 0.5f * leftAABB.m_max;

				//AABB rightAABB = AABB(rBVH.m_min.xyz, rBVH.m_max.xyz); //GetAABB(rightChild);
				//vec3 centerR = 0.5f * rightAABB.m_min + 0.5f * rightAABB.m_max;

				//vec3 vecL = (rayOrigin - centerL);
				//vec3 vecR = (rayOrigin - centerR);
				//
				//float sqrDistL = dot(vecL, vecL);
				//float sqrDistR = dot(vecR, vecR);
				//
				//bool leftFirst = sqrDistL <= sqrDistR;
				//if (leftFirst)
				//{
				//	leftChild = leftChild ^ rightChild;
				//	rightChild = leftChild ^ rightChild;
				//	leftChild = leftChild ^ rightChild;
				//}
				
				//atomicCompSwap(leftFir)
				stackPtr[currentIdx++] = leftChild;  //leftFirst ? leftChild  : rightChild;
				stackPtr[currentIdx++] = rightChild;  //leftFirst ? rightChild : leftChild ;
			}
			else //leaf node
			{
				int nodeStart = currentBVH.m_start;
				//return true;
				for (int i = 0; i < childCount; i++)
				{
					int j = nodeStart + i;
					//uint tIdx = texelFetch(u_triangleIdxTexture, j, 0).x;
					
					uint tIdx = tri_indices[j];
					
					//vec3 v0 = texelFetch(u_triangleTexture, 3 * int(tIdx), 0).xyz;
					//vec3 v1 = texelFetch(u_triangleTexture, 3 * int(tIdx) + 1, 0).xyz;
					//vec3 v2 = texelFetch(u_triangleTexture, 3 * int(tIdx) + 2, 0).xyz;

					vec3 v0 = triangles[  tIdx  ].A.xyz;
					vec3 v1 = triangles[  tIdx  ].B.xyz;
					vec3 v2 = triangles[  tIdx  ].C.xyz;


					HitData Thit = triIntersect(rayOrigin, rayDir, v0, v1, v2);

					float newT = Thit.dist;
					if (newT > 0)
					{
						hasHit = true;
						if (newT < t )
						{
							t = newT;
							_u = Thit.u;
							_v = Thit.v;
						}

					} 
				}
			}
		}
	}
	 
	
	result.u = _u;
	result.v = _v;
	result.t = t;
	result.hasHit = hasHit; 

	return result;
	//vec4 boxhit = BoxColor(u_bboxIdx, rayOrigin, rayDir);
	//
	//if (boxhit.a > 0)	finalColor = vec4(boxhit.xyz, 1.0f);
	//vec4 boxhits = vec4(0,0,0,0);
	//for (int i = 0; i < 70; i++)
	//{
	//	int childCount = texelFetch(u_indexTexture, i, 0).a; //w = alpha component
	//	if (childCount < 3)
	//	{
	//		vec4 boxhit = BoxColor(i, rayOrigin, rayDir);
	//		//finalColor = mix(finalColor, vec4(boxhit.xyz, 1.0f), 0.6f);
	//		boxhits +=  boxhit ;
	//	}
	//}


}
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_near_plane * u_far_plane) / 
	(u_far_plane + u_near_plane - z * (u_far_plane - u_near_plane));	
}
float LinearizeDepth2(float z)
{
  float n = 1.0; // camera z near
  float f = 100.0; // camera z far
  //float z = texture2D(depthSampler, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

 
vec3 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = u_inv_projMatrix * clipSpacePosition;

	//u_inv_projMatrix
	//u_inv_viewMatrix

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = u_inv_viewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
	vec2 aspectRatio = vec2(u_aspectRatio, 1.0f);
	vec2 scrnCoords = vec2((0.5f - TexCoords.xy)) * aspectRatio;

	vec3 cameraUp = normalize(u_cameraUp);
	vec3 viewDir = u_viewDir;

	vec3 albedo3 = texture(screenTexture, TexCoords).rgb;

	vec3 rayOrigin = u_cameraPos;
	vec3 scrnPlane = cross(cameraUp, viewDir);

	vec3 screenHoriz = scrnCoords.x * scrnPlane;
	vec3 screenVert = scrnCoords.y * -cameraUp;

	vec2 pixelCoord = vec2(TexCoords.x, 1.0f - TexCoords.y) * vec2(u_screenWidth, u_screenHeight);
	vec3 rayDir = RayFromCam(pixelCoord.x, pixelCoord.y);

	//rayDir = normalize(viewDir + screenHoriz + screenVert); //old method

	vec4 albedo4 = vec4(albedo3, 1);
	vec4 black = vec4(0, 0, 0, 1);
	vec4 red = vec4(1, 0, 0, 1);
	vec4 white = vec4(1, 1, 1, 1);
	vec4 finalColor = albedo4;

	//vec4 triangleColor = vec4(texture(u_triangleTexture, TexCoords.x).rgb, 1.0f);
	//vec4 mixedColor = mix(albedo4, triangleColor, 1.f);
	//bool foundHit = false;
	//bool foundHit = false;
 //for (int i = 0; i < 1200 ; i += 3)
 //{
 //	 vec3 v0 = texelFetch(u_triangleTexture, i, 0).xyz;
 //	 vec3 v1 = texelFetch(u_triangleTexture, i + 1, 0).xyz;
 //	 vec3 v2 = texelFetch(u_triangleTexture, i + 2, 0).xyz;
 //	
 ////	  vec3 v0 = vec3(0, 0, 0);
 ////	  vec3 v1 = vec3(0, 1, 0);
 ////	  vec3 v2 = vec3(1, 0, 0);
 //	// 
 //	vec3 hitData = triIntersect(rayOrigin, rayDir, v0, v1, v2);
 //	if( hitData.x > 0 ) foundHit = true;
 //}
  //
	//
	float u;
	float v;
	//bool foundHit = TraverseBVH(rayOrigin, rayDir, u, v);
	// bool foundHit = false;//	TraverseBVH(rayOrigin, rayDir, u, v);
	//finalColor = vec4(1, 1, 1, 1);
	//vec3 spyrotex = texture(u_spyroTexture, vec2(u,v), 0).rgb;
	
	//finalColor = foundHit ? vec4(u, v, 0.5f, 1.0f) : albedo4 ;
	float depthval = texture(u_depthBuffer, TexCoords.xy).r ;
	float depthLinear = LinearizeDepth2(depthval);
	vec4 distanceColor = vec4( vec3(depthLinear), 1.0f);
	finalColor=distanceColor;
	float smoothStp =smoothstep(u_smoothStepStart, u_smoothStepEnd, depthLinear);
	finalColor = mix(albedo4, vec4(0.1f,0.1f,0.1f,1.0f),  smoothStp);	// mix(albedo4 , distanceColor, 0.5f); ;
	vec4 worldPosZbuffer = vec4(WorldPosFromDepth(depthval), 1.0f);
	vec4 distPerpixel = worldPosZbuffer -	vec4( u_cameraPos, 1.0f) ;
	float distL = length(distPerpixel.xyz);
	vec3 worldPos_fromBuffer = texture(u_positionBuffer, TexCoords.xy).rgb;
	 
	
	vec3 wpos = u_useZbuffer ? worldPosZbuffer.xyz : worldPos_fromBuffer;

	//float diffuse = max(0, dot(normal, u_lightDir));
	finalColor = vec4(albedo4.rgb, 1.0f);

	BVHhit bvhR = TraverseBVH(wpos  + 0.001f * u_lightDir, u_lightDir);
	if(bvhR.hasHit)
	{
		//vec3 wpos = rayOrigin + bvhR.t  * rayDir;
		finalColor = vec4(finalColor.xyz * 0.2f, 1.0f);
	}

	FragColor = finalColor;
	// if(distL >20) 
	//	finalColor = black;

//BVHNode top = BVH[0];
//BVHNode lBVH = BVH[40];
	//BVHNode rBVH = BVH[top.m_leftFirst + 1];
	//BVHNode lBVH2 = BVH[lBVH.m_leftFirst];
	//BVHNode rBVH2 = BVH[lBVH.m_leftFirst + 1];


	//AABB lAABB = AABB(lBVH.m_min.xyz, lBVH.m_max.xyz);
	//AABB rAABB = AABB(lBVH.m_min.xyz, lBVH.m_max.xyz);




	//bool hitL =  IntersectAABB(rayOrigin, rayDir, lAABB )  || IntersectAABB(rayOrigin, rayDir, rAABB );
	//finalColor = hitL ? black : red;

	//IntersectAABB(rayOrigin, rayDir, lAABB ) ;

	// BVHhit bvhR = TraverseBVH(rayOrigin, rayDir);
	// finalColor = bvhR.hasHit ? vec4(bvhR.u, bvhR.v, 1  ,1): vec4(1,0,0.4,1);
	//finalColor =  vec4(1 - normal, 1);
	//
	//vec3 lightDir = -normalize(worldPos.xyz - u_lightDir);
	//BVHhit shadowData =  TraverseBVH(normal.xyz + 0.001f *   lightDir,   lightDir);
	//
	//if(shadowData.hasHit)
	//{
	//	finalColor = vec4(finalColor.xyz * vec3(0.,0.,0. ),1);
	//}
	//
	//FragColor =   finalColor;


	
	//finalColor = vec4(distPerpixel.xyz,1.0f);
	//finalColor = vec4(0.5,0,1,1);
	
	//for(int i = 0; i < 15; i++)
	//{
	//	 vec3 v0 = triangles[i].A.xyz;
	//	 vec3 v1 = triangles[i].B.xyz;
	//	 vec3 v2 = triangles[i].C.xyz;
	//	//vec3 v0 = texelFetch(u_triangleTexture, i, 0).xyz;
	//	//vec3 v1 = texelFetch(u_triangleTexture, i + 1, 0).xyz;
	//	//vec3 v2 = texelFetch(u_triangleTexture, i + 2, 0).xyz;
	//
	//	//AABB aaabbbb = AABB(BVH[0].m_min.xyz, BVH[0].m_max.xyz); 
	//	//bool hitBB = IntersectAABB(rayOrigin, rayDir , aaabbbb);
	//	
	//	HitData hd = triIntersect(rayOrigin, rayDir, v0, v1, v2);
	//	if(hd.hasHit)
	//	{
	//			finalColor = vec4(hd.u, hd.v, 1.0f,1.0f);
	//	}
	//}
	//if(hitBB)
	//finalColor = black;
	
	
	


	//float mixAlpha = boxhit.a > 0 ? 0.4f : 0.0f;
	//FragColor = mix(albedo4, boxhit, mixAlpha);
	//int size = textureSize(u_indexTexture, 0);
	//vec4 boxColor = vec4(0, 0, 0, 1);
	//vec4 boxhit = BoxColor(u_bboxIdx, rayOrigin, rayDir);
	//
	//if (boxhit.a > 0)	finalColor = vec4(boxhit.xyz, 1.0f);
	//vec4 boxhits = vec4(0,0,0,0);
	//for (int i = 0; i < 70; i++)
	//{
	//	int childCount = texelFetch(u_indexTexture, i, 0).a; //w = alpha component
	//	if (childCount < 3)
	//	{
	//		vec4 boxhit = BoxColor(i, rayOrigin, rayDir);
	//		//finalColor = mix(finalColor, vec4(boxhit.xyz, 1.0f), 0.6f);
	//		boxhits +=  boxhit ;
	//	}
	//}
	//
	//boxhits = vec4(normalize(boxhits.xyz), boxhits.w);
	//
	//if(boxhits.w > 0)
	//	FragColor = vec4( mix(albedo4.xyz, boxhits.xyz, 0.5f), 1.0f);
	//else FragColor = albedo4;
	//TraverseBVH()


}