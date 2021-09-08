#pragma once
#include <string>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
namespace Shaders
{
 void LoadShader(const std::string& path);
}