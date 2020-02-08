#pragma once

class JointTransform
{
public:
	glm::vec3 Position;
	glm::quat Rotation;	

	JointTransform(const glm::vec3& pos, const glm::quat& rot)
		:
		Position(pos), Rotation(rot) {};

	JointTransform InterpolateJoints(const JointTransform& A, const JointTransform& B, float interp) {
		//JointTransform jt;
		glm::vec3 p = interp * (A.Position + B.Position);
		glm::quat r = glm::slerp(A.Rotation, B.Rotation, interp); // or lerp?

		return JointTransform(p, r);

	}

};

