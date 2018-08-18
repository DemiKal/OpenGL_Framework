#include "GameObject.h"



GameObject::GameObject(const std::string& name)
	: m_va(nullptr),
	m_vb(nullptr),
	m_layout(nullptr),
	m_ib(nullptr),
	m_name(name)
{
	
}


GameObject::~GameObject()
{
}

GameObject GameObject::CreateCube()
{
	GameObject cube("Cube");



}
