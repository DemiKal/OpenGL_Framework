#pragma once
class Joint
{
public:
	int  Index;
	std::string Name;
	std::vector<Joint> Children;
	glm::mat4 mat_local;
	glm::mat4 inv_mat;
	std::vector<std::pair< std::string, unsigned int>> childrenPair;
	Joint(int idx, const std::string& _name, const glm::mat4& local_mat)
		:
		Index(idx), Name(_name), mat_local(local_mat), inv_mat(glm::inverse(local_mat)), childrenPair()
	{};

	Joint()
		: Index(-1), Name(""), Children(), mat_local(glm::mat4(1.0f)),	
		inv_mat(glm::mat4(1.0f)), childrenPair() {};

	std::string GetName() {
		return Name;
	}
	void AddChild(Joint& j) {
		Children.emplace_back(j);
	}

	glm::mat4  GetLocalTransform() { return mat_local; }

	void CalcInverseTransform(const glm::mat4& T_parent) {
		glm::mat4 bindT = T_parent * mat_local;

		inv_mat = glm::inverse(bindT);

		for (auto& j : Children) {
			CalcInverseTransform(bindT);
		}
	}

};



