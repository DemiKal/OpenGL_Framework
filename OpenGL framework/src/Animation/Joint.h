#pragma once
class Joint
{
public:
	int  m_index;
	std::string m_name;
	std::vector<Joint> m_children;
	glm::mat4 m_offset; //=offset!
	glm::mat4 m_inv_mat;
	glm::mat4 m_pose_transform;	//transform by interpolation of position, rotation, and scale, and combining into matrix
	glm::mat4 m_nodeTransform;	//transform of the node by assimp
	std::vector<std::pair< std::string, unsigned int>> m_childrenPair;

	Joint(int idx, const std::string& _name, const glm::mat4& local_mat)
		:
		m_index(idx), m_name(_name),
		m_offset(local_mat),
		m_inv_mat(glm::inverse(local_mat)),
		m_pose_transform(),
		m_nodeTransform(),
		m_childrenPair()
	{

	}

	Joint()
		:
		m_index(-1),
		m_name(""),
		m_children(),
		m_offset(glm::mat4(1.0f)),
		m_inv_mat(glm::mat4(1.0f)),
		m_pose_transform(),
		m_nodeTransform(),
		m_childrenPair()
	{
		
	}

	std::string GetName() const;

	void AddChild(Joint& j);

	glm::mat4 GetLocalTransform() const;

	void CalcInverseTransform(const glm::mat4& T_parent);
};





