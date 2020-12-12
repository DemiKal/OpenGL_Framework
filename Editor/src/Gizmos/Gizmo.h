#pragma once
#include <glm/glm.hpp>
#include <EditorLayer.h>

class EditorLayer;

class Gizmo
{
public:
	virtual void Draw() {};
	Gizmo(EditorLayer* edl, const std::string& name) :
		m_Edl(edl),
		m_Name(name),
		m_Color(0, 1, 0, 1),
		m_Enabled(false) {}

	EditorLayer* m_Edl;
	std::string m_Name;
	glm::vec4 m_Color ;
	bool m_Enabled;

	friend  EditorLayer;
};

