#pragma once
#include "Gizmo.h"

//class Camera;

class FrustumGizmo : public Gizmo 
{
public:
	FrustumGizmo(EditorLayer* edl, const std::string& name = "Frustum Gizmo");

	void Draw() override;

	//Camera* m_EditorCam;
	//Camera* m_DummyCam;

};

