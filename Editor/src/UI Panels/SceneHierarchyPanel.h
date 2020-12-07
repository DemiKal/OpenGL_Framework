#pragma once
#include "Panel.h"

struct SceneHierarchyPanel : public Panel
{
public:
	SceneHierarchyPanel(EditorLayer* edl);
	void OnImGuiRender(float dt) override;
};

