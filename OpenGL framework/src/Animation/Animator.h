#pragma once
#include "Animation\Animation.h"
#include "Animation\Joint.h"

class Animator
{
public:
	float animTime;
	Animation current;
	std::vector<Joint> m_bones;
	float m_duration;
	float m_ticks;		//ticks per second
	glm::mat4 m_inverse_root;

	Animator() : animTime(0), current() {}

	Animator(const Animator& a) :
		animTime(a.animTime), current(a.current), m_bones(a.m_bones),
		m_duration(a.m_duration),
		m_ticks(a.m_ticks),
		m_inverse_root(a.m_inverse_root) {}


	//TODO: improve
	void UpdateAnimTime() {
		float AnimationTime = animTime + 1.0f / 60.0f; //TODO: fixed at 60fps! 
		if (AnimationTime > current.m_length)
			AnimationTime = fmod(AnimationTime, current.m_length);

		animTime = AnimationTime;
	}

	void UpdateAnimation(float deltaTime);

	std::unordered_map<std::string, glm::mat4> CalcPose();

	void SetAnimation(Animation& animation) {
		current = animation;
	}



};

