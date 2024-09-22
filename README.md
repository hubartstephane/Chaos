# Chaos

Chaos is a project dedicated for game development.
It is composed by a C++ library named **chaos** and several games or test executables

Internally, **premake5** is used to generate solutions:
* **Visual Studio 2022** on windows
* **Codelite** on linux

Chaos uses the following external libraries:
* OpenGL
* Glfw
* Glew
* Glm
* Imgui
* Boost
* Irrklang
* Nlohmann
* Freetype2
* Freeimage
* Tinyxml2
* Lua
* Assimp
* ZLib
* Glslang

Some of thoses libraries are forked from the original depot, some are ZIPPED within Chaos files themselves
 
# Installation on windows

* First, you need to install git and its tools:

[https://git-scm.com/download/win](https://git-scm.com/download/win)

* Start git shell and get the depot from git:
```
git clone --recursive https://github.com/hubartstephane/Chaos.git
```
* Go within installation directory and install all dependencies (this will make all required libraries available):
```
install-extra-windows.bsh
```
* Generate the **Visual Studio 2022** solution with premake5:
```
generate-windows.bsh
```

# Installation on linux

* First, you need to install git and its tools:

```
sudo apt install git
```

* Then get the depot from git

```
git clone --recursive https://github.com/hubartstephane/Chaos.git
```
* Go within installation directory and install all dependencies (this will make all required libraries available):

```
install-extra-linux.bsh -lib
install-extra-linux.bsh -exe
install-extra-linux.bsh -tools                    (this is not mandatory)
```

* Generate the **Codelite** solution with premake5:
```
generate-linux.bsh
```
