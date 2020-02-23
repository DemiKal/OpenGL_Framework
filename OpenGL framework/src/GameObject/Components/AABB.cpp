#include "precomp.h"
#include "Model.h"
//#include "ShaderManager.h"


class Model;

void AABB::Draw(const Camera& camera)
{
	//glm::mat4 model = GetTransform();
	//glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 proj = camera.GetProjectionMatrix();

	Model* wirecube = EntityManager::GetEntities()[2]; //watch out!
	wirecube->model = GetTransform();
	GPUShader& shader = wirecube->GetShader();
	shader.Bind();
	shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	wirecube->Draw(camera);


}
