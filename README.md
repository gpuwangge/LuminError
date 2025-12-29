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


# Smart App Control（智能应用控制）导致的启动延迟说明  
在 Windows 11（22H2 及以上版本）中，微软引入了一项名为 Smart App Control（智能应用控制，SAC） 的安全功能。  
该功能会对未签名或不在微软信任列表中的可执行文件进行云端分析，以判断其安全性。  
为什么会导致启动延迟？  
如果你运行的是自行编译的程序（例如本项目），SAC 会在每次启动时执行以下操作：  
• 	上传文件哈希到微软云端进行安全性判断  
• 	如果无法判断，则进行本地行为分析  
• 	未签名程序每次编译都会改变哈希，因此每次运行都被视为“新程序”  
这会导致大约 1 秒左右的启动延迟，属于 SAC 的正常行为。  
如何关闭 Smart App Control？  
如果你不希望每次启动都被延迟，可以选择关闭 SAC：  
1. 	打开 设置  
2. 	进入 隐私和安全性  
3. 	打开 Windows 安全中心  
4. 	选择 应用和浏览器控制  
5. 	点击 Smart App Control 设置  
6. 	将其设置为 关闭（Off）  

重要提示：关闭后无法直接重新开启  
Smart App Control 的设计要求它只能在“干净系统”中启用。  
因此：  
• 	一旦关闭，就无法在现有系统中重新开启  
• 	唯一的重新开启方式是重置 / 重装 Windows 11  
这是微软当前的官方设计限制。  
未来改进（预计 2026 年）  
微软已在 Insider 频道测试新的 SAC 机制，  
预计在 2026 年推出允许用户关闭后重新开启的版本，无需重装系统。  

# Startup Delay Caused by Smart App Control (SAC)  
Windows 11 (version 22H2 and later) introduces a security feature called Smart App Control (SAC).  
This feature analyzes executable files that are unsigned or not recognized by Microsoft’s trust system.  
Why does it cause startup delays?  
If you run a self‑compiled program (such as this project), SAC performs the following checks every time you launch it:  
• 	Uploads the file hash to Microsoft’s cloud for safety evaluation  
• 	Performs local behavioral analysis if the cloud cannot determine safety  
• 	Since unsigned programs change their hash on every build, SAC treats each run as a “new” application  
This results in a ~1 second startup delay, which is expected behavior under SAC.  
How to disable Smart App Control  
If you prefer not to experience this delay, you can disable SAC:  
1. 	Open Settings  
2. 	Go to Privacy & security  
3. 	Open Windows Security  
4. 	Select App & browser control  
5. 	Click Smart App Control settings  
6. 	Set it to Off  

Important Note: You cannot re‑enable SAC without reinstalling Windows  
Smart App Control can only be enabled on a “clean” Windows installation.  
Therefore:  
• 	Once turned off, SAC cannot be turned back on in the current system  
• 	The only way to re‑enable it is to reset or reinstall Windows 11  
This is a current design limitation from Microsoft.  
Future improvement (expected in 2026)  
Microsoft is testing a new SAC mechanism in the Insider builds.  
A future update expected around 2026 will allow users to re‑enable SAC without reinstalling Windows.  