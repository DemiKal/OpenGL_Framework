#include "precomp.h"
#include "Rendering/ShaderManager.h"
#include "Animation/AnimationChannel.h"
#include "Animation/Joint.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"

void Animator::UpdateHierarchy(Joint& current, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverse_root)
{
	glm::mat4 currentMat = parentMat * current.m_PoseTransform;
	current.m_PoseTransform = inverse_root * currentMat * current.m_Offset;

	// for (auto& cp : current.m_ChildrenPair)
	for (auto& ci : current.m_ChildrenIndices)
		UpdateHierarchy(bones[ci], bones, currentMat, inverse_root);
}

void Animator::UpdateHierarchy(glm::mat4& inverse_root)
{
	const glm::mat4 parentMat = inverse_root;

	std::vector<int> stack = { 0 }; //idx 0 = start
	while (!stack.empty())
	{
		int idx = stack.back();
		stack.pop_back();
		Joint& current = m_bones[idx];
		fmt::print("Name {}, idx {}\n", current.m_Name, idx);
		current.m_PoseTransform = inverse_root * current.m_PoseTransform;
		glm::mat4 parentMat = current.m_PoseTransform;

		for (auto& c : current.m_ChildrenIndices)
			stack.emplace_back(c);


	}

	//glm::mat4 currentMat = parentMat * current.m_PoseTransform;
	//current.m_PoseTransform = inverse_root * currentMat * current.m_Offset;
	//
	////for (auto& cp : current.m_ChildrenPair)
	//for (auto& ci : current.m_ChildrenIndices)
	//	UpdateHierarchy(bones[ci], bones, currentMat, inverse_root);
}

void Animator::UpdateAnimation(const float dt, const float speedControl )
{
	animTime = fmod(animTime + 1000 * dt * speedControl, m_duration);
	///animTime = 0;
	for (Joint& joint : m_bones)
	{
		AnimationChannel& channel = current.m_AnimationChannels[joint.m_Index];
		float tick = animTime * m_ticks;

		unsigned int prev_indexPos = channel.FindPositionIndex(animTime);

		auto prevPos = channel.GetPositionByIndex(prev_indexPos);
		auto nextPos = channel.GetPositionByIndex(prev_indexPos + 1);

		float delta = nextPos.time - prevPos.time;
		float interp = (tick - prevPos.time) / delta;
		interp = glm::clamp(interp, 0.0f, 1.0f);
		glm::vec3 pos1 = prevPos.position;
		glm::vec3 pos2 = nextPos.position;
		glm::vec3 interpPos = glm::mix(pos1, pos2, interp);
		glm::mat4 posMat = glm::translate(glm::mat4(1.0f), interpPos);

		unsigned int prev_indexRot = channel.FindRotationIndex(tick);
		const RotationKey& prevRot = channel.GetRotationByIndex(prev_indexRot);
		const RotationKey& nextRot = channel.GetRotationByIndex(prev_indexRot + 1);

		float deltaRot = nextRot.time - prevRot.time;
		float interpolantRot = (tick - prevRot.time) / delta;
		interpolantRot = glm::clamp(interpolantRot, 0.0f, 1.0f);
		glm::quat interpolatedRot = glm::mix(prevRot.rotation, nextRot.rotation, interpolantRot);
		glm::mat4 rotMat = glm::mat4_cast(interpolatedRot);
		joint.m_PoseTransform = posMat * rotMat;
	}

	UpdateHierarchy(m_bones[0], m_bones, glm::mat4(1.0f), m_inverse_root);
	//UpdateHierarchy(m_inverse_root);
}

std::unordered_map<std::string, glm::mat4> Animator::CalcPose()
{
	return std::unordered_map<std::string, glm::mat4>();
}
