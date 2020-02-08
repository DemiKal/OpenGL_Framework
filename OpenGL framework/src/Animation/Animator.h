#pragma once
class Animator
{
public:
	float animTime;
	Animation current;

	Animator() : animTime(0), current() {}

	//TODO: improve
	void UpdateAnimTime() {
		float AnimationTime = animTime + 1.0f / 60.0f; //TODO: fixed at 60fps! 
		if (AnimationTime > current.length)
			AnimationTime = fmod(AnimationTime, current.length);

		animTime = AnimationTime;
	}

	void UpdateAnimation() {
		UpdateAnimTime();
		std::unordered_map<std::string, glm::mat4> currentPose = CalcPose();
		//ApplyPose()
	}

	std::unordered_map<std::string, glm::mat4> CalcPose()
	{

	}





};

