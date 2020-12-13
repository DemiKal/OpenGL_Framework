#include "precomp.h"
#include "Rendering/ShaderManager.h"
#include "Animation/AnimationChannel.h"
#include "Animation/Joint.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"

Animator::Animator() : animTime(0), current(), m_Duration(0), m_Ticks(0), m_inverse_root()
{
}

Animator::Animator(const Animator& a) :
	animTime(a.animTime), current(a.current), m_Bones(a.m_Bones),
	m_Duration(a.m_Duration),
	m_Ticks(a.m_Ticks),
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


void Animator::UpdateReverse(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot)
{
	  glm::mat4 currentMat =    inverse(currentJoint.m_Offset);
	currentMat *= parentMat;// (currentJoint.m_Offset);
	currentMat *= currentJoint.m_Offset;
	//currentJoint.m_Reverse = currentJoint.m_Reverse * inverseRoot * currentMat * currentJoint.m_Offset;
	currentJoint.m_Reverse = currentMat;
	for (auto& ci : currentJoint.m_ChildrenIndices)
		UpdateReverse(bones[ci], bones, currentMat, inverseRoot);
}

void Animator::UpdateHierarchy(Joint& currentJoint, std::vector<Joint>& bones, const glm::mat4& parentMat, glm::mat4& inverseRoot)
{
	const glm::mat4 currentMat = parentMat * currentJoint.m_PoseTransform;
	currentJoint.m_PoseTransform = inverseRoot * parentMat * currentJoint.m_PoseTransform  * currentJoint.m_Offset;

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
}

void Animator::SetPose(int poseNr)
{
	const int channelSize = static_cast<int>(current.m_AnimationChannels[0].GetLength());
	if (poseNr < 0 || poseNr >= channelSize)
	{
		fmt::print("pose index out of bounds!");
		poseNr = (poseNr + 1) % (channelSize - 1);
	}

	float singlePoseTime = m_Duration / channelSize;
	animTime = 0;
	animTime = fmod(animTime + singlePoseTime * poseNr, m_Duration);

	//fmod(animTime     , m_Duration);
	///animTime = 0;
	float tick = animTime * m_Ticks;

	//	const float tick = static_cast<float>(poseNr) / static_cast<float>(channelSize);

	CalcPose(poseNr);

	UpdateHierarchy(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
	UpdateReverse(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
}

void Animator::UpdateAnimation(const float dt, const float speedControl)
{
	animTime = fmod(animTime + 1000 * dt * speedControl, m_Duration);
	///animTime = 0;
	float tick = animTime * m_Ticks;
	CalcPose(tick);

	UpdateHierarchy(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
	UpdateReverse(m_Bones[0], m_Bones, glm::mat4(1.0f), m_inverse_root);
}

std::vector<glm::mat4> Animator::GetPose()  const
{
	std::vector<glm::mat4> mats;
	for (const Joint& j : m_Bones)
		mats.push_back(j.m_PoseTransform);
	return mats;
}

void Animator::CalcPose(const float tick)
{
	for (Joint& joint : m_Bones)
	{
		AnimationChannel& channel = current.m_AnimationChannels[joint.m_Index];

		const unsigned int prev_indexPos = channel.FindPositionIndex(tick);

		const auto& prevPos = channel.GetPositionByIndex(prev_indexPos);
		const auto& nextPos = channel.GetPositionByIndex(prev_indexPos + 1);

		const float delta = nextPos.time - prevPos.time;
		float interp = (tick - prevPos.time) / delta;
		interp = glm::clamp(interp, 0.0f, 1.0f);
		glm::vec3 pos1 = prevPos.position;
		glm::vec3 pos2 = nextPos.position;
		glm::vec3 interpPos = glm::mix(pos1, pos2, interp);
		glm::mat4 posMat = glm::translate(glm::mat4(1.0f), interpPos);

		const unsigned int prev_indexRot = channel.FindRotationIndex(animTime);
		const RotationKey& prevRot = channel.GetRotationByIndex(prev_indexRot);
		const RotationKey& nextRot = channel.GetRotationByIndex(prev_indexRot + 1);

		const float deltaRot = nextRot.time - prevRot.time;
		float interpolantRot = (tick - prevRot.time) / deltaRot;
		interpolantRot = glm::clamp(interpolantRot, 0.0f, 1.0f);
		glm::quat   interpolatedRot = glm::slerp(prevRot.rotation, nextRot.rotation, interpolantRot);

		glm::mat4 rotMat = glm::mat4_cast(interpolatedRot);
		joint.m_PoseTransform = posMat * rotMat;
		joint.m_PurePose = joint.m_PoseTransform;

	}
	//return std::unordered_map<std::string, glm::mat4>();
}

void Animator::SetAnimation(Animation& animation)
{
	current = animation;
}


//Calculate pose on the indexed value
void Animator::CalcPose(const int poseIdx)
{
	for (Joint& joint : m_Bones)
	{
		AnimationChannel& channel = current.m_AnimationChannels[joint.m_Index];

		//const unsigned int prev_indexPos = channel.FindPositionIndex(tick);

		const auto& posKey = channel.GetPositionByIndex(poseIdx);

		glm::mat4 posMat = glm::translate(glm::mat4(1.0f), posKey.position);

		const RotationKey& rotationKey = channel.GetRotationByIndex(poseIdx);
		const RotationKey& nextRotationKey = channel.GetRotationByIndex(poseIdx);
		const glm::quat rot = glm::slerp(rotationKey.rotation, nextRotationKey.rotation, 0.5f);

		glm::mat4 rotMat = glm::mat4_cast(rot);
		joint.m_PoseTransform = posMat * rotMat;
	}
	//return std::unordered_map<std::string, glm::mat4>();
}

void Animator::SetTPose()
{
	for (Joint& j : m_Bones)
		j.m_PoseTransform = glm::mat4(1.0f);
}
