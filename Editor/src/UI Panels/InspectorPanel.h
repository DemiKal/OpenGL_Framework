#pragma once
#include "Panel.h"

struct InspectorPanel :   public Panel
{
 
	InspectorPanel(EditorLayer* edl);
	void OnImGuiRender(float dt) override;
};

