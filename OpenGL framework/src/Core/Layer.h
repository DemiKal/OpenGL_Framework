#pragma once
#include <string.h>

#include <utility>
#include "precomp.h"

class Layer
{
public:
	Layer(std::string name);
	virtual ~Layer() = default;

	virtual void OnAttach() {}

	virtual void OnDetach() {}
	virtual void OnUpdate(float dt) {}
	virtual void OnImGuiRender(float dt) {}
	//virtual void OnEvent(Event& event) {}

	[[nodiscard]] const std::string& GetName() const;
protected:
	std::string m_Name;
};

inline Layer::Layer(std::string name): m_Name(std::move(name))
{
}

inline const std::string& Layer::GetName() const
{
	return m_Name;
}
