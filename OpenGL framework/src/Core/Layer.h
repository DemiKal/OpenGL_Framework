#pragma once
#include "precomp.h"

namespace meme {
	class Editor;
}

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
	meme::Editor* m_Editor = nullptr;
	friend meme::Editor;
};

inline Layer::Layer(std::string name): m_Name(std::move(name))
{
}

inline const std::string& Layer::GetName() const
{
	return m_Name;
}
