# OpenGL_Framework
Welcome to [Engine name]. Here we have a game engine that includes
- Custom raytracing shaders (Compute + CUDA) [todo]
- Ray traced shadows as Post Process
- Model loading
- Two-level BVH builder (Top level Binned and Bottom level SAH),
- Deferred + Forward pipeline
- Physics [todo]
- Collision detection [todo]
- Path planning [todo]
- AI component [todo]
- Seperate scripting modules [todo]
- Tiled shading / light culling [todo]
- Culling optimizations [todo]

This engine is designed to utilize modern OpenGL code [(AZDO)](https://www.gdcvault.com/play/1020791/Approaching-Zero-Driver-Overhead-in).
Possible Vulkan extensions in the future 

# Build Instructions
Run `git clone --recursive https://github.com/DemiKal/OpenGL_Framework`
If you forgot `--recursive`, run `git submodule update --init`

## vcpkg 
If you already have vcpkg installed, just run install the libraries listed below (step 6).

1. You can clone it in any folder, but C:\vcpkg is recommended. Run `git clone https://github.com/Microsoft/vcpkg` in C:/
2. Run `bootstrap-vcpkg.bat` 
3. Add system variable `VCPKG_ROOT` with the installation folder as value, in our case `C:/vcpkg`
4. Add system variable `VCPKG_DEFAULT_TRIPLET` with value `x64-windows`
5. Add `C:\vcpkg` as a system path variable. You should now be able to run it directly from the command line (try closing the window and/or restarting Windows to see if it works).
6. Install the packages. From CMD, run: `vcpkg install glm glfw3 fmt spdlog stb assimp`
6. Run `vcpkg integrate install` for good measure, to make sure it's integrated into Visual Studio. 

You can enable use of precompiled headers in the `OpenGL Framework` project for faster compilation. Make sure to set `Editor` as the start up project in Visual Studio.

## Assets
Simply unzip Assets.zip file directly where it is.

# Libraries used
[TODO] put here libraries used with their license clauses.
