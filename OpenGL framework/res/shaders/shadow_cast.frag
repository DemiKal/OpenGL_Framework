
#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

const int StackSize = 32;
const int LEAFSIZE = 2;

uniform bool u_shadowCast;
uniform bool u_useZbuffer;
uniform bool u_linearizeZ;
uniform bool u_showNormal;

uniform float u_near_plane;
uniform float u_far_plane;
uniform float u_aspectRatio;
uniform float u_screenWidth;
uniform float u_screenHeight;
uniform float u_ambientLight;
uniform float u_epsilon;

uniform vec3 u_cameraPos;
uniform vec3 u_viewDir;
uniform vec3 u_cameraUp;
uniform vec3 u_lightDir;
 
uniform mat4 u_inv_projMatrix;
uniform mat4 u_inv_viewMatrix; 

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D zBuffer;


struct Triangle { vec4 A; vec4 B; vec4 C; };

struct BVHNode { vec4 m_min; vec4 m_max; };

layout (std430,  binding = 0) readonly buffer BVH_buffer 
{
	BVHNode BVH[] ;	
};
 
layout (std430, binding = 1) readonly buffer Triangle_buffer
{
	Triangle triangles[] ;	
};
 
layout (std430, binding = 2) readonly buffer Index_buffer
{
	uint tri_indices[] ;	
};
  


struct AABB
{
	vec3 m_min;
	vec3 m_max;
};

struct HitData
{
	bool hasHit;
	float u,v, dist;
};

  

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

	uint stackPtr[StackSize];
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
		uint bboxIdx = stackPtr[currentIdx - 1];
		currentIdx--;
 		BVHNode currentBVH = BVH[bboxIdx];

		uint childCount =  floatBitsToUint(currentBVH.m_max.w); //currentBVH.m_count;
		bool isInterior = childCount > LEAFSIZE; //TODO: leaf check
		//vec3 BBmin = vec3(currentBVH.minX,currentBVH.minY,currentBVH.minZ);
		//vec3 BBmax = vec3(currentBVH.maxX,currentBVH.maxY,currentBVH.maxZ);
		
		AABB currentAABB = AABB(currentBVH.m_min.xyz , currentBVH.m_max.xyz);  //GetAABB(bboxIdx);

		if (IntersectAABB(rayOrigin, rayDir, currentAABB))
		{
			if (isInterior)
			{
				uint leftFirst = floatBitsToUint(currentBVH.m_min.w);
				uint rightFirst = leftFirst + 1;
				//BVHNode lBVH = BVH[leftChild];
				 
				//atomicCompSwap(leftFir)
				stackPtr[currentIdx++] = leftFirst;  //leftFirst ? leftChild  : rightChild;
				stackPtr[currentIdx++] = rightFirst;  //leftFirst ? rightChild : leftChild ;
			}
			else //leaf node
			{
				uint nodeStart = floatBitsToUint(currentBVH.m_min.w);
				//int nodeStart = bboxIdx;
				 
				for (int i = 0; i < childCount; i++)
				{
					uint j = nodeStart + i; 
					
					uint tIdx = tri_indices[j];
					
					vec3 v0 = triangles[tIdx].A.xyz;
					vec3 v1 = triangles[tIdx].B.xyz;
					vec3 v2 = triangles[tIdx].C.xyz;
					 
					HitData Thit = triIntersect(rayOrigin, rayDir, v0, v1, v2);

					float newT = Thit.dist;
					if (newT > 0 && newT >= u_epsilon)
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
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_near_plane * u_far_plane) / 
	(u_far_plane + u_near_plane - z * (u_far_plane - u_near_plane));	
}

float LinearizeDepth2(float z)
{
  float n = u_near_plane ; // camera z near
  float f = u_far_plane ; // camera z far
  //float z = texture2D(depthSampler, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

vec3 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = u_inv_projMatrix * clipSpacePosition;

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

	vec3 albedo3 = texture(gAlbedoSpec, TexCoords).rgb;

	vec3 rayOrigin = u_cameraPos;
	vec3 scrnPlane = cross(cameraUp, viewDir);

	vec3 screenHoriz = scrnCoords.x * scrnPlane;
	vec3 screenVert = scrnCoords.y * -cameraUp;

	vec2 pixelCoord = vec2(TexCoords.x, 1.0f - TexCoords.y) * vec2(u_screenWidth, u_screenHeight);
	vec3 rayDir = RayFromCam(pixelCoord.x, pixelCoord.y);

	vec4 albedo4 = vec4(albedo3, 1);
 
	vec4 finalColor = albedo4;

 
	vec3 normal = texture(gNormal, TexCoords.xy).xyz;
	vec3 worldPos_fromBuffer = texture(gPosition, TexCoords.xy).rgb;
	
	float NdotL = dot(normal, u_lightDir);
	float diffuse = clamp(max(NdotL, 0) + u_ambientLight, 0, 1);


	float zValue = texture(zBuffer, TexCoords.xy).x;
	float zLinear =   LinearizeDepth2(zValue) ;
	
	vec3 wposFromG = worldPos_fromBuffer;
	vec3 wposFromZ = WorldPosFromDepth(u_linearizeZ ? zLinear : zValue );

	vec3 wpos = u_useZbuffer ? wposFromZ : wposFromG;

	finalColor = vec4(diffuse * albedo4.rgb, 1.0f);
	//finalColor = vec4(wpos, 1.0);
	
	if(u_shadowCast)
	{
		if(NdotL > 0 && zValue < 1) 
		{
				float bias = 0.005*tan(acos(clamp(NdotL,0,1))); 
				bias = clamp(bias, 0., 0.01);
				float slope = 2 - NdotL ; //1 + 1 - ndotl
				 
				BVHhit bvhR = TraverseBVH(wpos + (u_epsilon*slope ) * normal, u_lightDir);
				if(bvhR.hasHit)
				{
						//vec3 wpos = rayOrigin + bvhR.t  * rayDir;
					finalColor = vec4(finalColor.xyz * 0.2f    , 1.0f);
					//finalColor = vec4(bvhR.u, bvhR.v, 1, 1.0f);
				}
		}
	}
	 
	// finalColor = vec4(vec3(zLinear), 1.0);
	if(u_showNormal) finalColor =vec4(normal,1.0);
	FragColor = finalColor;
}