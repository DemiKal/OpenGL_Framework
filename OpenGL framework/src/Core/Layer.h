#pragma once
#include <string.h>
#include "precomp.h"

class Layer
{
public:
	Layer(const std::string& name) : m_Name(name)
	{

	}
	virtual ~Layer() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float dt) {}
	virtual void OnImGuiRender() {}
	//virtual void OnEvent(Event& event) {}

	const std::string& GetName() const { return m_Name; }
protected:
	std::string m_Name;
};