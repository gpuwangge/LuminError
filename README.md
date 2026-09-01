<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
</p>

# LuminError
LuminError 是一个面向现代实时渲染的开源游戏引擎，核心基于 Vulkan 构建，并集成硬件加速光线追踪能力。引擎旨在提供从窗口与输入交互、资源加载、场景管理，到高质量实时图形渲染的一体化基础框架。  

在渲染层面，LuminError 利用 Vulkan 的低开销、显式资源管理与跨平台特性，实现高性能的图形与计算任务调度，并可进一步支持光线追踪阴影、反射、全局光照等更具真实感的视觉效果。引擎同时集成 GLM、stb_image、tinyobjloader、tinygltf 等组件，用于数学计算、纹理读取以及 OBJ、glTF/GLB 等常见资产格式的加载，为 PBR 材质、模型渲染和场景构建提供基础支持。  

在平台与交互层面，LuminError 通过 SDL3 统一处理窗口创建、键盘鼠标输入、事件分发及底层系统交互，减少平台相关代码，使项目能够更方便地扩展到 Windows、Linux 等支持 Vulkan 与 SDL 的平台。SDL3_ttf 则为引擎提供了文本和 TrueType 字体渲染能力，可用于调试信息、UI 和游戏内文字显示。  

项目采用 CMake 管理构建流程，并将 Vulkan SDK、SDL3、yaml-cpp 等主要第三方依赖集成在 external/ 目录中，降低了首次配置和构建的成本。无论是希望深入理解 Vulkan 光追管线、研究实时渲染技术，还是作为自定义游戏或图形应用的开发基础，LuminError 都提供了一个可阅读、可修改、可持续扩展的开源引擎框架。  

LuminError is an open-source game engine designed for modern real-time rendering. Built around Vulkan, it integrates hardware-accelerated ray tracing capabilities. The engine aims to provide a unified foundation covering window and input interaction, asset loading, scene management, and high-quality real-time graphics rendering.  

On the rendering side, LuminError leverages Vulkan’s low overhead, explicit resource management model, and cross-platform capabilities to schedule graphics and compute workloads efficiently. It can be further extended to support more realistic visual effects, including ray-traced shadows, reflections, and global illumination. The engine also integrates components such as GLM, stb_image, tinyobjloader, and tinygltf for mathematical computation, texture loading, and importing common asset formats including OBJ and glTF/GLB, providing the foundation for PBR materials, model rendering, and scene construction.  

For platform abstraction and interaction, LuminError uses SDL3 to handle window creation, keyboard and mouse input, event dispatch, and low-level system integration through a unified interface. This reduces platform-specific code and makes it easier to extend the project to platforms such as Windows and Linux that support Vulkan and SDL. SDL3_ttf additionally provides text and TrueType font rendering for debugging overlays, user interfaces, and in-game text.  

The project uses CMake to manage its build workflow, with key third-party dependencies—including the Vulkan SDK, SDL3, and yaml-cpp—integrated under the external/ directory. This lowers the initial setup and build cost for new contributors. Whether used to study Vulkan ray-tracing pipelines, explore real-time rendering techniques, or serve as a development foundation for custom games and graphics applications, LuminError provides an open-source engine framework that is readable, modifiable, and continuously extensible.  

本项目提供大量测试案例(example/)，每个测试案例都是含有独立的游戏框架：  

This project provides a large collection of test cases under example/. Each test case contains an independent game framework:   

## SimpleRayPipelineDragon
模型是Stanford Graphics Lab通过三维扫描龙形雕塑生成的经典测试模型，包含约 87 万个三角形。  
Dragon 模型几何细节密集，用于检查 BVH/AS 构建、实例变换、法线插值、命中信息读取以及 GPU 资源同步等环节。  
光追算法：Whitted-style Ray Tracing。  

The model is a classic test asset created by the Stanford Graphics Lab through 3D scanning of a dragon sculpture. It contains approximately 870,000 triangles.  
With its dense geometric detail, the Dragon model was used to validate key stages of the rendering pipeline, including BVH/acceleration-structure construction, instance transformations, normal interpolation, hit-record retrieval, and GPU resource synchronization.  
Ray-tracing algorithm: Whitted-style Ray Tracing.  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineDragon1.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineDragon2.png" alt="alt text">  

## SimpleRayPipelineSponza
Sponza是以克罗地亚杜布罗夫尼克Sponza Palace的中庭为原型的场景模型。它最初为渲染竞赛制作，随后因其复杂的室内遮挡结构、大量材质与纹理、明显的明暗区域和适合观察全局光照的空间布局，被全球光照研究与实时渲染社区广泛采用，现已成为测试 GI、Path Tracing、光照采样、去噪和渲染性能的经典场景。  
Sponza 包含大量独立网格、纹理和材质区域，能够更全面地验证 glTF/GLB 资产解析、PBR 材质参数处理、纹理采样、descriptor 管理及实例级加速结构组织流程。  
光追算法：Monte Carlo Path Tracing。NEE开启。  

Sponza is a scene model based on the atrium of the Sponza Palace in Dubrovnik, Croatia. It was originally created for a rendering contest and was later widely adopted by the global-illumination and real-time-rendering communities because of its complex indoor occlusion, numerous materials and textures, pronounced light-and-shadow regions, and spatial layout well suited to evaluating global illumination. It has since become a classic benchmark scene for testing GI, path tracing, light sampling, denoising, and rendering performance.  
Sponza contains a large number of individual meshes, textures, and material regions, making it suitable for more comprehensive validation of glTF/GLB asset parsing, PBR material-parameter processing, texture sampling, descriptor management, and instance-level acceleration-structure organization.  
Ray-tracing algorithm: Monte Carlo Path Tracing with Next Event Estimation (NEE) enabled.  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza1.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza2.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza3.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza4.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza5.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza6.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineSponza7.png" alt="alt text">  




# [More Examples](https://github.com/gpuwangge/LuminError/blob/main/ExampleLists.md)  


# [Build Instruction](https://github.com/gpuwangge/LuminError/blob/main/BuildInstruction.md)  



