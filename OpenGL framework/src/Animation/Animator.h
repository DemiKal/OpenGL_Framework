#pragma once
#include "Animation/Animation.h"
#include "Animation/Joint.h"

class Animator
{
public:
	float animTime;
	Animation current;
	std::vector<Joint> m_bones;
	float m_duration;
	float m_ticks;		//ticks per second
	glm::mat4 m_inverse_root;
	std::vector<uint32_t> m_ChildCount;
	std::vector<uint32_t> m_ChildIndex;



	Animator() : animTime(0), current(), m_duration(0), m_ticks(0), m_inverse_root()
	{
	}

	Animator(const Animator& a) :
		animTime(a.animTime), current(a.current), m_bones(a.m_bones),
		m_duration(a.m_duration),
		m_ticks(a.m_ticks),
		m_inverse_root(a.m_inverse_root) {}


	//TODO: improve
	void UpdateAnimTime() 
	{
		float AnimationTime = animTime + 1.0f / 60.0f; //TODO: fixed at 60fps! 
		if (AnimationTime > current.m_Length)
			AnimationTime = fmod(AnimationTime, current.m_Length);

		animTime = AnimationTime;
	}

	void UpdateHierarchy(Joint& current, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverse_root);

	void UpdateHierarchy(glm::mat4& inverse_root);

	void UpdateAnimation(const float dt, const float speedControl = 1);

	std::unordered_map<std::string, glm::mat4> CalcPose();

	void SetAnimation(Animation& animation) {
		current = animation;
	}



};

