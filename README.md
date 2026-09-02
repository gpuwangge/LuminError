<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
</p>

# LuminError
LuminError(R) 是一台基于 Vulkan 的游戏引擎，专门把代码、显存和光线拧成一座会呼吸的数字城市。  

它用 Vulkan 接管 GPU：低开销、显式资源控制、跨平台调度，一个都不少。硬件光线追踪则负责制造“真实感幻觉”——阴影更利落，反射更骚。  

从窗口创建、键鼠输入、资源加载、场景管理，到高质量图形与计算渲染，LuminError 都给你铺好了底层线路。GLM 管数学，stb_image 读纹理，tinyobjloader 和 tinygltf 把 OBJ、glTF/GLB 资产塞进引擎，为 PBR 材质、模型和场景搭建备好零件。  

SDL3 是跨平台接入终端，负责窗口、事件和输入；SDL3_ttf 则让 HUD、调试信息和游戏内文字在屏幕上亮起来，不至于只剩 GPU 在自言自语。  

构建交给 CMake，Vulkan SDK、SDL3、yaml-cpp 等依赖已收编进 external/。想研究 Vulkan 光追、拆渲染管线、魔改材质系统，或者从零搭一座自己的霓虹世界？  

LuminError(R) is a Vulkan-based game engine built to weave code, VRAM, and rays into a living digital city.  

It uses Vulkan to take direct control of the GPU: low overhead, explicit resource management, and cross-platform scheduling are all part of the package. Hardware ray tracing creates the illusion of realism—sharper shadows and more striking reflections.  

From window creation, keyboard and mouse input, resource loading, and scene management to high-quality graphics and compute rendering, LuminError provides the underlying infrastructure. GLM handles the math; stb_image loads textures; tinyobjloader and tinygltf bring OBJ and glTF/GLB assets into the engine, providing the building blocks for PBR materials, models, and scenes.  

SDL3 serves as the cross-platform entry point, handling windows, events, and input. SDL3_ttf makes HUD elements, debugging information, and in-game text visible on screen, so the GPU is not left talking to itself.  

CMake handles the build system, while dependencies such as the Vulkan SDK, SDL3, and yaml-cpp are already integrated under external/. Whether you want to study Vulkan ray tracing, dissect rendering pipelines, heavily customize the material system, or build your own neon world from scratch, LuminError gives you a foundation to start from.  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/LuminErrorArch.png" alt="alt text"> 

本项目提供测试案例(example/)，每个测试案例都含有独立的游戏框架：  

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



