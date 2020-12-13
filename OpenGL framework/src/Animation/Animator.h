#pragma once
#include "Animation/Animation.h"
#include "Animation/Joint.h"

class Animator
{
public:
	float animTime;
	Animation current;
	std::vector<Joint> m_Bones;
	float m_Duration;
	float m_Ticks;		//ticks per second
	glm::mat4 m_inverse_root;
	std::vector<uint32_t> m_ChildCount;
	std::vector<uint32_t> m_ChildIndex;

	Animator();

	Animator(const Animator& a);


	//TODO: improve
	void UpdateAnimTime();

	void UpdateReverse(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot);

	void UpdateHierarchy(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot);

	void UpdateHierarchy(glm::mat4& inverse_root);

	void SetPose(int poseNr);

	void UpdateAnimation(float dt, float speedControl = 1);

	std::vector<glm::mat4> GetPose() const;

	void CalcPose(float timeStamp);

	void SetAnimation(Animation& animation);
	void CalcPose(int poseIdx);
	void SetTPose();
	void CalcOffsetChain(std::vector<glm::mat4>& mats, glm::mat4& parent, int idx = 0)
	{
		Joint& joint = m_Bones[idx];
		glm::mat4 currentOffset = parent * joint.m_Offset;

		mats.emplace_back(currentOffset);

		for (uint32_t i : joint.m_ChildrenIndices)
		{
			CalcOffsetChain(mats, currentOffset, i);
		}



	}




};

