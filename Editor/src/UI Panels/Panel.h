#pragma once
//#include "EditorLayer.h"

class EditorLayer;

struct  Panel
{
	Panel(EditorLayer* edl) : m_Edl(edl) {}
	EditorLayer* m_Edl;
	virtual void  OnImGuiRender(float dt) {};
};
