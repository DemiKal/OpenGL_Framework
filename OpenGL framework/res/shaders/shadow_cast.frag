#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

const int StackSize = 64;
const int LEAFSIZE = 2;

//Gbuffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D u_depthBuffer;

uniform float u_screenWidth;
uniform float u_screenHeight;
uniform float u_nearPlane;
uniform float u_farPlane ;

uniform mat4 u_inv_projMatrix;
uniform mat4 u_inv_viewMatrix;
uniform bool u_shadowCast;

//uniforms
uniform vec3 u_lightDir;



struct Triangle { vec4 A; vec4 B; vec4 C; };
struct AABB { vec3 m_min; vec3 m_max; };
struct HitData { bool hasHit; float u,v, dist; };
struct BVHNode { vec4 m_min; vec4 m_max; };

layout (std430, binding = 0) readonly buffer BVH_buffer { BVHNode BVH[]; };
layout (std430, binding = 1) readonly buffer Triangle_buffer { Triangle triangles[]; };
layout (std430, binding = 2) readonly buffer Index_buffer { uint tri_indices[] ; };

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

vec3 RayFromCam(float mouseX, float mouseY);
BVHhit TraverseBVH(vec3 rayOrigin, vec3 rayDir);
bool IntersectAABB(vec3 rayOrigin, vec3 rayDirection, AABB bbox);
HitData triIntersect(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2);
float LinearizeDepth(float depth);
float LinearizeDepth2(float z);



void main()
{             
	vec2 pixelCoord = vec2(TexCoords.x, 1.0f - TexCoords.y) * vec2(u_screenWidth, u_screenHeight);
	vec3 rayDir = RayFromCam(pixelCoord.x, pixelCoord.y);

	vec4 albedoSpec = texture(gAlbedoSpec, TexCoords.xy).xyzw;
	vec4 finalColor = vec4(albedoSpec.rgb, 1.0);

	//float depthval = texture(u_depthBuffer, TexCoords.xy).r ;
	//float depthLinear = LinearizeDepth2(depthval);
	//vec4 worldPosZbuffer = vec4(WorldPosFromDepth(depthval), 1.0f);
	vec3 wposGbuffer = texture(gPosition, TexCoords.xy).rgb;
	vec3 normalGbuffer = texture(gNormal, TexCoords.xy).xyz;
	 
	vec3 wpos = wposGbuffer;
	if(u_shadowCast)
	{
		BVHhit bvhR = TraverseBVH(wpos  + 0.01f * u_lightDir, u_lightDir);
		if(bvhR.hasHit)
		{
			//vec3 wpos = rayOrigin + bvhR.t  * rayDir;
			finalColor = vec4(finalColor.xyz * 0.2f, 1.0f);
		}
	}

	FragColor = finalColor;
}



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
				//return true;
				
				for (int i = 0; i < childCount; i++)
				{
					uint j = nodeStart + i; 
					
					uint tIdx = tri_indices[j];
					
					vec3 v0 = triangles[tIdx].A.xyz;
					vec3 v1 = triangles[tIdx].B.xyz;
					vec3 v2 = triangles[tIdx].C.xyz;
					 
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
}


HitData triIntersect(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2)
{
	vec3 v1v0 = v1 - v0;
	vec3 v2v0 = v2 - v0;
	vec3 rov0 = ro - v0;

#if 1
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

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_nearPlane * u_farPlane) / 
	(u_farPlane + u_nearPlane - z * (u_farPlane - u_nearPlane));	
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

    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = u_inv_viewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}