#pragma once
#include "Animation/Animation.h"
#include "Animation/Joint.h"

class Animator
{
public:
	float animTime;
	Animation current;
	std::vector<Joint> m_Bones;
	float m_duration;
	float m_ticks;		//ticks per second
	glm::mat4 m_inverse_root;
	std::vector<uint32_t> m_ChildCount;
	std::vector<uint32_t> m_ChildIndex;

	Animator();

	Animator(const Animator& a);


	//TODO: improve
	void UpdateAnimTime();

	void UpdateHierarchy(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot);

	void UpdateHierarchy(glm::mat4& inverse_root);

	void SetPose(int poseNr);

	void UpdateAnimation(float dt, float speedControl = 1);

	std::vector<glm::mat4> GetPose() const;

	void CalcPose(float timeStamp);

	void SetAnimation(Animation& animation);
};

