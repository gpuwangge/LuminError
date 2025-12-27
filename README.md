# Build
```
mkdir build  
cd build
cmake -G "MinGW Makefiles" ..
cmake -E time make -j
```

# Run
```
cd build/bin  
```
Run SimpleTriangle:  
```
.\SimpleTriangle.exe
```
Run all tests:
```
.\autotest.bat
```

# Prerequisites (Windows)
Before building this project on Windows 10/11, please ensure the following tools are installed:  

1. CMake  
Used for generating the build system.  
Recommended version: CMake ≥ 3.x  
Download: https://cmake.org/download/
Instruction(Chinese): https://github.com/gpuwangge/Wiki/blob/main/documents/CMake.md  

2. MinGW-w64 (Windows Toolchain)
Required for compiling the project on Windows using the MinGW toolchain.  
Make sure the following tools are available in your system PATH:  
gcc, g++, gdb, make  
Download: https://www.mingw-w64.org/  
Instruction(Chinese): https://github.com/gpuwangge/Wiki/blob/main/documents/VSCode.md  

3. Visual Studio Code (VS Code)  
Recommended code editor.  
Download: https://code.visualstudio.com/  
Suggested extensions:  
CMake Tools  
C/C++  

4. Git  
Required for cloning the repository and version control on Windows.  
Download: https://git-scm.com/download/win  

Once all prerequisites are installed, you can proceed to configuring and building the project using CMake.  

# Integrated Components
This project comes with several third-party libraries already integrated.  
All headers and prebuilt libraries are located under the external/ directory, so no additional downloads are required.  
The following components are included:  

• Vulkan SDK (headers & libs & layers)  
Used for low-level graphics and compute operations.  

• GLM  
A header-only math library optimized for graphics applications.  

• SDL3  
Handles window creation, input, and cross-platform system utilities.  

• SDL3_ttf  
Provides TrueType font rendering support for text display.  

• yaml-cpp  
Used for parsing and handling YAML configuration files.  

• stb_image  
Header-only library for loading image files (PNG, JPG, etc.).  

• tinyobjloader  
A lightweight library for loading OBJ 3D model files.  

All components are bundled inside the repository under external/, and the CMake build system is already configured to include them.  
