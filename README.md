<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
</p>

# LuminError
LuminError(R) 是一个基于 Vulkan 开发的游戏引擎项目，主要用于实时渲染、光线追踪和图形功能实验。  

引擎底层使用 Vulkan，直接处理 GPU 资源、同步、命令提交和渲染流程。相比封装程度更高的图形接口，这种方式需要处理更多细节，但也能更清楚地控制资源生命周期和渲染管线。项目支持硬件光线追踪，可用于实现阴影、反射等效果，并为路径追踪、全局光照等后续功能预留扩展空间。  

目前项目包含窗口创建、键盘鼠标输入、资源加载、场景管理、图形渲染和计算渲染等基础模块。GLM 用于向量、矩阵和变换计算；stb_image 用于加载纹理；tinyobjloader 和 tinygltf 分别处理 OBJ 与 glTF/GLB 格式资源，为模型、贴图、PBR 材质和场景数据提供支持。  

SDL3 负责跨平台窗口、事件和输入处理，SDL3_ttf 用于 HUD、调试信息及游戏内文本渲染。  

项目使用 CMake 管理构建流程，Vulkan SDK、SDL3、yaml-cpp 等依赖统一整理在 external/ 目录中。  

LuminError(R) 不是一个已经封装完成、开箱即用的商业引擎，更像是一套正在逐步搭建的 Vulkan 渲染框架。它适合用来研究光线追踪、调试渲染管线、扩展材质系统，或者把一座还只有网格、灯光和日志输出的城市慢慢做出来。  

LuminError(R) is a game engine project built on Vulkan, primarily intended for real-time rendering, ray tracing, and graphics experimentation.  

At its core, the engine uses Vulkan to manage GPU resources, synchronization, command submission, and the rendering pipeline directly. Compared with higher-level graphics APIs, this approach requires handling more low-level details, but it also provides clearer control over resource lifetimes and rendering behavior. The project supports hardware ray tracing for effects such as shadows and reflections, while leaving room for future features including path tracing and global illumination.  

The project currently includes core systems for window creation, keyboard and mouse input, asset loading, scene management, graphics rendering, and compute rendering. GLM is used for vector, matrix, and transformation math; stb_image handles texture loading; tinyobjloader and tinygltf are used to import OBJ and glTF/GLB assets, providing support for models, textures, PBR materials, and scene data.  

SDL3 handles cross-platform windows, events, and input, while SDL3_ttf is used for rendering HUD elements, debug information, and in-game text.  

The project uses CMake for build management. Dependencies such as the Vulkan SDK, SDL3, and yaml-cpp are organized under the external/ directory.  

LuminError(R) is not a fully packaged, plug-and-play commercial engine. It is better described as a Vulkan rendering framework that is still being built out over time. It can be used to study ray tracing, debug rendering pipelines, extend material systems, or gradually build a world that currently consists of little more than geometry, lights, and log output.  


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



