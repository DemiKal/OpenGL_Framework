# OpenGL_Framework
Welcome to [Engine name]. Here we have a game engine that includes
- Custom raytracing shaders
- Shadow casting as Post Process
- Model loading
- BVH (SAH) builder
- Physics [todo]
- Collision detection [todo]
- Path planning [todo]
- AI component [todo]

This engine is designed to utilize modern OpenGL code [(AZDO)](https://www.gdcvault.com/play/1020791/Approaching-Zero-Driver-Overhead-in).

# Build Instructions
We use [vcpkg](https://github.com/microsoft/vcpkg) for C++ libraries. Tested with VS 2017 and VS 2019.
Simply clone this repository, then install libraries with vcpkg, then unzip the resources files as explained below.

## vcpkg 
If you already have vcpkg installed, just run install the libraries listed below.

1. You can clone it in any folder, but C:\vcpkg is recommended. Run `git clone https://github.com/Microsoft/vcpkg` in C:/
2. Run `bootstrap-vcpkg.bat` 
3. Add system variable `VCPKG_ROOT` with the installation folder as value, in our case `C:/vcpkg`
4. Add system variable `VCPKG_DEFAULT_TRIPLET` with value `x64-windows`
5. Add `C:\vcpkg` as a system path variable. You should now be able to run it directly from the command line (try closing the window and/or restarting Windows to see if it works).
6. Install the packages. From CMD, run `vckpg install glm glfw magic-enum fmt imgui spdlog stb assimp`
6. Run `vcpkg integrate install` for good measure, to make sure it's integrated into Visual Studio. 

## Assets
[Todo] Fix this approach and add another (LFS) repo for assets and resources

Download the (art) resources file (res.zip) which contains models, textures, shaders, and more.

unzip into ...\OpenGL_Framework\OpenGL_Framework\res.  The `res` folder will be adjacent to the `src` folder. Make sure you're in the 'deepest' `OpenGL_Framework` folder.

# Libraries used
[TODO] put here libraries used with their license clauses.
