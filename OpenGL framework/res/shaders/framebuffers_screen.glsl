#shader vertex
#version 330 core
///#########################\\\
///							\\\
///		VERTEX SHADER		\\\
///							\\\
///#########################\\\

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}




///#########################\\\
///							\\\
///		FRAGMENT SHADER		\\\
///							\\\
///#########################\\\


#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 u_cameraPos;
uniform vec3 u_viewDir;
uniform vec3 u_cameraUp;

uniform sampler2D screenTexture;
uniform sampler1D u_triangleTexture;

uniform float u_aspectRatio;
uniform float u_screenWidth;
uniform float u_screenHeight;

uniform mat4 u_inv_projMatrix;
uniform mat4 u_inv_viewMatrix;


vec3 triIntersect(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2)
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

	return vec3(t, u, v);
}

vec3 RayFromCam(float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / u_screenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / u_screenHeight;
	float z = 1.0f;
	mat4 inv_proj_mat = u_inv_projMatrix;
	mat4 inv_view_matrix = u_inv_viewMatrix;

	//normalized device coordinates [-1:1, -1:1, -1:1]
	vec3 ray_nds = vec3(x, y, z);

	// clip space (4d homogenized) [-1:1, -1:1, -1:1, -1:1]
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	// eye space [-x:x, -y:y, -z:z, -w:w]
	vec4 ray_eye = inv_proj_mat * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space [-x:x, -y:y, -z:z, -w:w]
	vec3 ray_world = vec3(inv_view_matrix * ray_eye);
	ray_world = normalize(ray_world);

	//Ray ray(this->PositionRead(), ray_world);

	return ray_world;
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
	vec4 black = vec4(0, 0, 0, 1.0f);
	vec4 finalColor = albedo4;
	vec4 triangleColor = vec4(texture(u_triangleTexture, TexCoords.x).rgb, 1.0f);
	//vec4 mixedColor = mix(albedo4, triangleColor, 1.f);

	for (int i = 0; i < 38; i += 3)
	{
		vec3 A = texelFetch(u_triangleTexture, i + 0, 0).rgb;
		vec3 B = texelFetch(u_triangleTexture, i + 1, 0).rgb;
		vec3 C = texelFetch(u_triangleTexture, i + 2, 0).rgb;

		vec3 intrs = triIntersect(rayOrigin, rayDir, A, B, C);
		if (intrs.x > 0)
		{
			finalColor = vec4(intrs.y, intrs.z, 1.0f, intrs.x);
			break;
		}
	}

	FragColor = finalColor;

	//bool intrsects = false;
   //for (int i = 0; i < 10; i++)
   //{
   //	vec3 A = texture(u_triangleTexture, 3 * i + 0).rgb;
   //	vec3 B = texture(u_triangleTexture, 3 * i + 1).rgb;
   //	vec3 C = texture(u_triangleTexture, 3 * i + 2).rgb;
   //	vec3 intrs = triIntersect(rayOrigin, rayDir, A, B, C);
   //	if (intrs.x > 0 ) intrsects |= true;
   //}
   //
   //
   //if (intrsects)
   //	color = vec4(1.0f,0,0,1);
   //else color = vec4(intrs.y, intrs.z, 1, 1);


   //color = vec4(rayDir, 1.0f);


   //if (dist < 0.5)	color = mix(blacktrans, vec4(col, 1.0f), 0.5f);
   // FragColor = color;//vec4(dist,dist,0, 1.0f);
   //vec4 col4 = vec4(TexCoords.xy, 0.0f,  1.0f);
   //FragColor = mix(vec4(col,1.0f),col4,0.5f)  ;//vec4(dist,dist,0, 1.0f); weird interpolation vagueness


}