#pragma once
class SSBO
{
	uint32_t m_RenderID = 0;
	bool IsInit = false;
	void* m_DataPtr = nullptr;
	uint32_t m_Size = 0;
	
	SSBO()
	{

	}
	void Bind() 
	{

	};
	uint32_t Size() { return 0; }

};

