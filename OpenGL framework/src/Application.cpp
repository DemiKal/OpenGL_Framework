#include "precomp.h"
#include "Application.h"
#include <glm/gtc/matrix_access.hpp>

int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	InputManager::SetWindow(window);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	{
		Renderer renderer;
		renderer.SetAlphaBlending(true);
		ShaderManager::Init(); //init before any model

		//Model obj = Model();
		//obj.loadModel("res/meshes/nanosuit/nanosuit.obj", aiProcess_Triangulate);
		//obj.SetShader("normalmapshader");

		Model cube = Model("res/meshes/Crash Bandicoot/Spike/SpikeHero.obj", aiProcess_Triangulate);
		cube.name = "cube";

		EntityManager::AddEntity(cube);
		cube.SetShader("basic");
		//cube.getMesh(0).addTexture(Texture2D("res/textures/marble.jpg", "texture_diffuse"));

		EntityManager::GetEntities()[0]->m_position = { 0,5,0 };
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), { 4,5,6 });
		glm::vec4 col1 = glm::column(trans, 3);

		Model plane = Model::CreatePlane();
		plane.name = "plane";
		EntityManager::AddEntity(plane);

		//TriangleBuffer::AddTriangles(plane);

		plane.SetShader("plane");
		plane.getMesh(0).addTexture(Texture2D("res/textures/brickwall.jpg", "texture_diffuse"));

		Model wireCube = Model::CreateCubeWireframe();
		wireCube.name = "wirecube";
		EntityManager::AddEntity(wireCube);

		wireCube.SetShader("singlecolor");

		Model spyro("res/meshes/Spyro/Spyro.obj", aiPostProcessSteps::aiProcess_Triangulate);
		spyro.SetShader("basic");
		spyro.name = "spyro";
		EntityManager::AddEntity(spyro);

		//Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiPostProcessSteps::aiProcess_Triangulate);
		//artisans.SetShader("basic");
		//artisans.name = "name";
		//EntityManager::AddEntity(artisans);

		//Model nanosuit("res/meshes/nanosuit/nanosuit.obj", (aiPostProcessSteps)(aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace));
		//nanosuit.name = "nanosuit";
		//EntityManager::AddEntity(nanosuit);
		//nanosuit.SetShader("normalmapshader");

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		// screen quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		float lineverts[] = { 0, 0, 1, 1 };

		unsigned int lineVao, lineVBO;
		glGenVertexArrays(1, &lineVao);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVao);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineverts), &lineverts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		GPUShader& postProcessShader = ShaderManager::GetShader("framebuffers_screen");

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);

		GPUShader& lineshader = ShaderManager::GetShader("lineshader");
		GPUShader& boneshader = ShaderManager::GetShader("bones");

		// framebuffer configuration
		// -------------------------
		////////////////////////////

		FrameBuffer framebuffer;

		static bool output = true;
		// create a color attachment texture
		unsigned int textureColorbuffer;
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		RenderBufferObject rbo;
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Texture back("res/textures/uvtest.png");

		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 200.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateYlocal(glm::radians(-90.0f));

		Camera::SetMainCamera(&cam2);
		//Camera* cam = Camera::GetMain(); //??
		//* cam->Position()   += glm::vec3(2, 0, 3);
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		glEnable(GL_DEPTH_TEST);

		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color(0.3f, 0.5, 0.1, 0.5f);

		auto& singledotshader = ShaderManager::GetShader("singledotshader");
		//
		// Render loop
		//

		LightManager::AddLight({ 0,  5, 0 }, { 1,0,0 });
		LightManager::AddLight({ -2,  3, 0 }, { 0,1,0 });
		LightManager::AddLight({ 2,  3, 0 }, { 0,0,1 });

		const glm::vec3 upWorld(0, 1, 0);
		const glm::vec3 rightWorld(1, 0, 0);
		const glm::vec3 forwardWorld(0, 0, -1);

		//auto tt = glm::rotate(plane.model, glm::radians(45.0f+90.0f), { 0,0,1 });
		//plane.getMesh(0).m_aabb.RecalcBounds(tt);
		//wireCube.model = plane.getMesh(0).m_aabb.GetTransform();
		//plane.model *= tt;
		double prevFrameTime = glfwGetTime();

		glm::vec3 ll(-1, -1, -1);	glm::vec3 ll2(-1, -1, 1);
		glm::vec3 lr(1, -1, -1);	glm::vec3 lr2(1, -1, 1);
		glm::vec3 ur(1, 1, -1);		glm::vec3 ur2(1, 1, 1);
		glm::vec3 ul(-1, 1, -1);	glm::vec3 ul2(-1, 1, 1);

		std::vector<glm::vec3> aabbVerts = {
			ll,		//0
			lr,		//1
			ur,		//2
			ul,		//3
			ll2,	//4
			lr2,	//5
			ur2,	//6
			ul2		//7  
		};




		unsigned int aabbVAO, aabbVBO;
		glGenVertexArrays(1, &aabbVAO);
		glGenBuffers(1, &aabbVBO);
		glBindVertexArray(aabbVAO);
		glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
		glBufferData(GL_ARRAY_BUFFER, aabbVerts.size() * sizeof(glm::vec3), &aabbVerts[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		std::vector<unsigned int> indices =
		{
			 0, 1, 1, 2, 2, 3, 3, 0,
			 4, 5, 5, 6, 6, 7, 7, 4,
			 0, 4, 1, 5, 2, 6, 3, 7
		};

		GLuint elementbuffer;
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);






		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;

			ImGui_ImplGlfwGL3_NewFrame();

#pragma region input

			InputManager::Update(camera);

			const float average = (float)std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000.0f * float(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30) frametimes.clear();
#pragma endregion input

			framebuffer.Bind();
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			// make sure we clear the framebuffer's content
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube.Update(deltaTime);
			plane.Update(deltaTime);
			spyro.Update(deltaTime);
			//nanosuit.Update(deltaTime);
			//artisans.Update(deltaTime);

			cube.Draw(camera);
			plane.Draw(camera);
			spyro.Draw(camera);
			//artisans.Draw(camera);

			//nanosuit.GetShader().Bind();
			//nanosuit.GetShader().setVec3("lightPos", LightManager::GetLight(0).get_position());
			//nanosuit.GetShader().setVec3("viewPos", camera.GetPosition());
			//nanosuit.Draw(camera);

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			auto [hit, selected] = camera.MousePick(mouseX, mouseY);

			std::string selectedStr = "None";
			if (hit)
			{
				selectedStr = selected->name;
			}

			ImGui::LabelText("label", selectedStr.c_str());


			LightManager::debug_render(camera);



			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			ImGui::ColorEdit4("clear color", static_cast<float*>(&override_color[0]));
			ImGui::SliderFloat3("Position", &spyro.m_position[0], -100.0f, 100.0f);
			ImGui::SliderFloat3("Rotation", &spyro.m_rotation[0], 0, 360);
			ImGui::SliderFloat3("Scale", &spyro.m_scale[0], 0, 10);

			glm::vec3 minV(std::numeric_limits<float>::infinity());
			glm::vec3 maxV(-std::numeric_limits<float>::infinity());



			for (auto& v_l : spyro.getMesh(0).positionVertices)
			{
				glm::vec3 v_w = spyro.model * glm::vec4(v_l, 1.0f);
				minV = glm::vec3(std::min(v_w.x, minV.x), std::min(v_w.y, minV.y), std::min(v_w.z, minV.z));
				maxV = glm::vec3(std::max(v_w.x, maxV.x), std::max(v_w.y, maxV.y), std::max(v_w.z, maxV.z));
			}

			auto& aabb = spyro.getMesh(0).m_aabb;
			ImGui::Text("Recalc min: %f, %f, %f", minV.x, minV.y, minV.z);
			ImGui::Text("AABB min: %f, %f, %f", aabb.m_min.v.x, aabb.m_min.v.y, aabb.m_min.v.z);
			ImGui::Text("Recalc max: %f, %f, %f", maxV.x, maxV.y, maxV.z);
			ImGui::Text("AABB max: %f, %f, %f", aabb.m_max.v.x, aabb.m_max.v.y, aabb.m_max.v.z);


			//aabbVerts[0] = { minV.x, minV.y, minV.z };	//ll
			//aabbVerts[1] = { maxV.x, minV.y, minV.z };	//lr
			//aabbVerts[2] = aabbVerts[1];				 
			//aabbVerts[3] = { maxV.x, maxV.y, minV.z };	//ur
			//aabbVerts[4] = aabbVerts[3];				 
			//aabbVerts[5] = { minV.x, maxV.y, minV.z };	//ul
			//aabbVerts[6] = aabbVerts[0];				 
			//
			//aabbVerts[7] = aabbVerts[0];   aabbVerts[7].z = maxV.z;
			//aabbVerts[8] = aabbVerts[1];   aabbVerts[8].z = maxV.z;
			//aabbVerts[9] = aabbVerts[2];   aabbVerts[9].z = maxV.z;
			//aabbVerts[10] = aabbVerts[3];	aabbVerts[10].z = maxV.z;
			//aabbVerts[11] = aabbVerts[4];	aabbVerts[11].z = maxV.z;
			//aabbVerts[12] = aabbVerts[5];	aabbVerts[12].z = maxV.z;
			//aabbVerts[13] = aabbVerts[6];	aabbVerts[13].z = maxV.z;
			//
			//aabbVerts[14] = aabbVerts[0]; aabbVerts[15] = aabbVerts[7];
			//aabbVerts[16] = aabbVerts[1]; aabbVerts[17] = aabbVerts[8];
			//aabbVerts[18] = aabbVerts[3]; aabbVerts[19] = aabbVerts[10];
			//aabbVerts[20] = aabbVerts[5]; aabbVerts[21] = aabbVerts[7];

			auto& ogAABB = spyro.getMesh(0).m_aabb_OG;
			glm::vec3 minVOOB = ogAABB.m_min;
			glm::vec3 maxVOOB = ogAABB.m_max;

			aabbVerts[0] =   glm::vec4{ minV.x, minV.y, minV.z, 1.0f };
			aabbVerts[1] =   glm::vec4{ maxV.x, minV.y, minV.z, 1.0f };
			aabbVerts[2] =   glm::vec4{ maxV.x, maxV.y, minV.z, 1.0f };
			aabbVerts[3] =   glm::vec4{ minV.x, maxV.y, minV.z, 1.0f };
						    
			aabbVerts[4] =   glm::vec4{ minV.x, minV.y, maxV.z, 1.0f };
			aabbVerts[5] =   glm::vec4{ maxV.x, minV.y, maxV.z, 1.0f };
			aabbVerts[6] =   glm::vec4{ maxV.x, maxV.y, maxV.z, 1.0f };
			aabbVerts[7] =   glm::vec4{ minV.x, maxV.y, maxV.z, 1.0f };


			glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, aabbVerts.size() * sizeof(glm::vec3), (void*)&aabbVerts[0]);



			auto& aabbshader = ShaderManager::GetShader("aabb");
			aabbshader.Bind();
			aabbshader.SetUniformMat4f("view", camera.GetViewMatrix());
			aabbshader.SetUniformMat4f("projection", camera.GetProjectionMatrix());

			glBindVertexArray(aabbVAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

			// Draw the triangles !
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT,	(void*)0);

			aabbVerts[0] = spyro.model * glm::vec4{ minVOOB.x, minVOOB.y, minVOOB.z, 1.0f };
			aabbVerts[1] = spyro.model * glm::vec4{ maxVOOB.x, minVOOB.y, minVOOB.z, 1.0f };
			aabbVerts[2] = spyro.model * glm::vec4{ maxVOOB.x, maxVOOB.y, minVOOB.z, 1.0f };
			aabbVerts[3] = spyro.model * glm::vec4{ minVOOB.x, maxVOOB.y, minVOOB.z, 1.0f };

			aabbVerts[4] = spyro.model * glm::vec4{ minVOOB.x, minVOOB.y, maxVOOB.z, 1.0f };
			aabbVerts[5] = spyro.model * glm::vec4{ maxVOOB.x, minVOOB.y, maxVOOB.z, 1.0f };
			aabbVerts[6] = spyro.model * glm::vec4{ maxVOOB.x, maxVOOB.y, maxVOOB.z, 1.0f };
			aabbVerts[7] = spyro.model * glm::vec4{ minVOOB.x, maxVOOB.y, maxVOOB.z, 1.0f };
			glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, aabbVerts.size() * sizeof(glm::vec3), (void*)&aabbVerts[0]);

			glBindVertexArray(aabbVAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, (void*)0);





			prevFrameTime = currentFrameTime;
			glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			postProcessShader.Bind();

			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());


			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back(static_cast<float>(diffms));
		}
	}

	ShaderManager::Destroy();
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}


