#include "precomp.h"
#include "HardwareQuery.h"

void HardwareQuery::Query()
{
	std::cout << "--- BEGIN GPU QUERY ---\n";

	std::cout << "GLSL version: " << glGetString(GL_VERSION) << std::endl;
	GLint max_tex_size = 0;
	GLint max_uniform_locations = 0;
	GLint maxInvoc = 0;
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &max_uniform_locations);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxInvoc);
	int work_grp_size[3];
	// maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);


	std::cout << "GL_MAX_TEXTURE_SIZE: " << max_tex_size << "\n";
	std::cout << "GL_MAX_UNIFORM_LOCATIONS: " << max_uniform_locations / 1024 << "kb \n";
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: " << maxInvoc / 1024 << "kb \n";
	printf("GL_MAX_COMPUTE_WORK_GROUP_SIZE: %i, %i, %i ", work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	std::cout << "--- END GPU QUERY-------------\n";
}
