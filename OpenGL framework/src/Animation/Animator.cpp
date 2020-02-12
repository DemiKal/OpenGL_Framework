#include "precomp.h"

void UpdateHierarchy(Joint& current, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverse_root)
{
	glm::mat4 currentMat = parentMat * current.pose_transform;
	current.pose_transform = inverse_root * currentMat * current.offset;

	for (auto& cp : current.childrenPair)
		UpdateHierarchy(bones[cp.second], bones, currentMat, inverse_root);
}

void Animator::UpdateAnimation(float deltaTime)
{
	animTime = fmod(animTime + 1000* deltaTime, m_duration);

	for (Joint& joint : m_bones)
	{
		for (AnimationChannel& channel : current.m_animationChannels)
		{
			if (channel.m_name == joint.Name) //lmao
			{
				float tick = animTime * m_ticks;
				//tick = std::fmod(tick, m_duration);

				unsigned int prev_indexPos = channel.FindPositionIndex(animTime);

				auto prevPos = channel.m_positionKeys[prev_indexPos];
				auto nextPos = channel.m_positionKeys[prev_indexPos + 1];

				float delta = nextPos.first - prevPos.first;
				float interp = (tick - prevPos.first) / delta;
				interp = glm::clamp(interp, 0.0f, 1.0f);
				glm::vec3 pos1 = prevPos.second; //channel.m_positionKeys[prev_index].second;
				glm::vec3 pos2 = nextPos.second; //channel.m_positionKeys[0].second;
				glm::vec3 interpPos = glm::mix(pos1, pos2, interp);
				glm::mat4 posMat = glm::translate(glm::mat4(1.0f), interpPos);

				size_t prev_indexRot = channel.FindRotationIndex(tick);
				auto prevRot = channel.m_rotationKeys[prev_indexPos];
				auto nextRot = channel.m_rotationKeys[prev_indexPos + 1];

				float deltaRot = nextRot.first - prevRot.first;
				float interpolantRot = (tick - prevRot.first) / delta;

				glm::quat interpolatedRot = glm::mix(prevRot.second, nextRot.second, interpolantRot);
				glm::mat4 rotMat = glm::mat4_cast(interpolatedRot);
				joint.pose_transform = posMat * rotMat;
			}
		}
	}

	UpdateHierarchy(m_bones[0], m_bones, glm::mat4(1.0f), m_inverse_root);

}

std::unordered_map<std::string, glm::mat4> Animator::CalcPose()
{
	return std::unordered_map<std::string, glm::mat4>();
}
