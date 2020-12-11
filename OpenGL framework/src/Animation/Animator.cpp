#include "precomp.h"
#include "Rendering/ShaderManager.h"
#include "Animation/AnimationChannel.h"
#include "Animation/Joint.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"

Animator::Animator(): animTime(0), current(), m_duration(0), m_ticks(0), m_inverse_root()
{
}

Animator::Animator(const Animator& a):
	animTime(a.animTime), current(a.current), m_Bones(a.m_Bones),
	m_duration(a.m_duration),
	m_ticks(a.m_ticks),
	m_inverse_root(a.m_inverse_root)
{
}

void Animator::UpdateAnimTime()
{
	float AnimationTime = animTime + 1.0f / 60.0f; //TODO: fixed at 60fps! 
	if (AnimationTime > current.m_Length)
		AnimationTime = fmod(AnimationTime, current.m_Length);

	animTime = AnimationTime;
}


void Animator::UpdateHierarchy(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot)
{
	const glm::mat4 currentMat = parentMat * currentJoint.m_PoseTransform;
	currentJoint.m_PoseTransform = inverseRoot * currentMat * currentJoint.m_Offset;

	for (auto& ci : currentJoint.m_ChildrenIndices)
		UpdateHierarchy(bones[ci], bones, currentMat, inverseRoot);
}

void Animator::UpdateHierarchy(glm::mat4& inverse_root)
{
	const glm::mat4 parentMat = inverse_root;

	std::vector<int> stack = { 0 }; //idx 0 = start
	while (!stack.empty())
	{
		int idx = stack.back();
		stack.pop_back();
		Joint& current = m_Bones[idx];
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

void Animator::SetPose(int poseNr)
{
	const int channelSize = static_cast<int>(current.m_AnimationChannels.size());
	if( poseNr < 0  || poseNr >= channelSize)
	{
		fmt::print("pose index out of bounds!");
		return;
	}

	const float tick = static_cast<float>(poseNr) / static_cast<float>(channelSize);
	
	CalcPose(tick);

	UpdateHierarchy(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
}

void Animator::UpdateAnimation(const float dt, const float speedControl )
{
	animTime = fmod(animTime + 1000 * dt * speedControl, m_duration);
	///animTime = 0;
	float tick = animTime * m_ticks;
	CalcPose(tick);

	UpdateHierarchy(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
}

std::vector<glm::mat4>  Animator::GetPose()  const
{
	std::vector<glm::mat4> mats;
	for(const Joint& j : m_Bones)
		mats.push_back(j.m_PoseTransform);
	return mats;
}

void Animator::CalcPose(const float tick)
{
	for (Joint& joint : m_Bones)
	{
		AnimationChannel& channel = current.m_AnimationChannels[joint.m_Index];

		const unsigned int prev_indexPos = channel.FindPositionIndex(animTime);

		auto &prevPos = channel.GetPositionByIndex(prev_indexPos);
		auto &nextPos = channel.GetPositionByIndex(prev_indexPos + 1);

		const float delta = nextPos.time - prevPos.time;
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
	//return std::unordered_map<std::string, glm::mat4>();
}

void Animator::SetAnimation(Animation& animation)
{
	current = animation;
}
