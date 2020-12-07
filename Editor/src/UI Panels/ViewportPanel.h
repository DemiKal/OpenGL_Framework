#pragma once
#include "Panel.h"

struct ViewportPanel : public Panel
{
	ViewportPanel(EditorLayer* edl);
	void OnImGuiRender(float dt) override;
};

