#include "precomp.h"
#include "Rendering/ShaderManager.h"
#include "Animation/AnimationChannel.h"
#include "Animation/Joint.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"

void UpdateHierarchy(Joint& current, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverse_root)
{
	glm::mat4 currentMat = parentMat * current.m_pose_transform;
	current.m_pose_transform = inverse_root * currentMat * current.m_offset;

	for (auto& cp : current.m_childrenPair)
		UpdateHierarchy(bones[cp.second], bones, currentMat, inverse_root);
}

void Animator::UpdateAnimation(float deltaTime)
{
	animTime = fmod(animTime + 1000 * deltaTime, m_duration);

	for (Joint& joint : m_bones)
	{
		AnimationChannel& channel = current.m_animationChannels[joint.m_index];
		float tick = animTime * m_ticks;

		size_t prev_indexPos = channel.FindPositionIndex(animTime);

		auto prevPos = channel.GetPositionByIndex(prev_indexPos);
		auto nextPos = channel.GetPositionByIndex(prev_indexPos + 1);

		float delta = nextPos.time - prevPos.time;
		float interp = (tick - prevPos.time) / delta;
		interp = glm::clamp(interp, 0.0f, 1.0f);
		glm::vec3 pos1 = prevPos.position;
		glm::vec3 pos2 = nextPos.position;
		glm::vec3 interpPos = glm::mix(pos1, pos2, interp);
		glm::mat4 posMat = glm::translate(glm::mat4(1.0f), interpPos);

		size_t prev_indexRot = channel.FindRotationIndex(tick);
		auto prevRot = channel.GetRotationByIndex(prev_indexRot);
		auto nextRot = channel.GetRotationByIndex(prev_indexRot + 1);

		float deltaRot = nextRot.time - prevRot.time;
		float interpolantRot = (tick - prevRot.time) / delta;
		interpolantRot = glm::clamp(interpolantRot, 0.0f, 1.0f);
		glm::quat interpolatedRot = glm::mix(prevRot.rotation, nextRot.rotation, interpolantRot);
		glm::mat4 rotMat = glm::mat4_cast(interpolatedRot);
		joint.m_pose_transform = posMat * rotMat;
	}

	UpdateHierarchy(m_bones[0], m_bones, glm::mat4(1.0f), m_inverse_root);
}

std::unordered_map<std::string, glm::mat4> Animator::CalcPose()
{
	return std::unordered_map<std::string, glm::mat4>();
}
