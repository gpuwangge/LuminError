<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
</p>

# LuminEvo 
**LuminEvo** is a **Vulkan-based rendering framework for real-time graphics, hardware ray tracing, and GPU experimentation**, written in C++.  

The engine directly manages Vulkan GPU resources, synchronization, command submission, descriptor sets, and rendering pipelines. It supports graphics and compute workloads and provides a low-level environment for experimenting with **rasterization, hardware ray tracing, path tracing, GPU resource management, and rendering performance**.  

### Key Features
* **Vulkan Rendering** — Graphics and compute workloads with explicit GPU resource and synchronization management
* **Hardware Ray Tracing** — BLAS/TLAS acceleration structures and Vulkan ray-tracing pipelines
* **Path Tracing** — Monte Carlo path tracing with Next Event Estimation (NEE)
* **PBR Rendering** — Physically based materials and glTF/GLB asset support
* **GPU Experiments** — Rendering workloads, compute workloads, ray-tracing experiments, and performance analysis
* **Modular Architecture** — Decoupled renderer, resource, and pipeline subsystems

LuminEvo is under active development and serves as a personal environment for exploring **GPU architecture concepts, rendering algorithms, Vulkan programming, and GPU performance**.  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/LuminEvolutionArch.png" alt="alt text"> 

## SimpleRayPipelineDragon
The model is a classic test asset created by the Stanford Graphics Lab through 3D scanning of a dragon sculpture. It contains approximately 870,000 triangles.  
With its dense geometric detail, the Dragon model was used to validate key stages of the rendering pipeline, including BVH/acceleration-structure construction, instance transformations, normal interpolation, hit-record retrieval, and GPU resource synchronization.  
Ray-tracing algorithm: Whitted-style Ray Tracing.  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineDragon1.png" alt="alt text">  
<img src="https://github.com/gpuwangge/LuminError/blob/main/images/SimpleRayPipelineDragon2.png" alt="alt text">  

## SimpleRayPipelineSponza
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
LuminEvo includes **35 example programs** across three pipeline types:  

| Pipeline | Examples | Highlights |
|----------|----------|------------|
| Graphics | 27 | Triangle, PBR, shadow mapping, MSAA, skybox, particles, environment mapping |
| Compute | 6 | Path tracing, GEMM, storage buffer/image, texture blur |
| Ray Tracing | 2 | Whitted-style (Stanford Dragon), Monte Carlo path tracing with NEE (Sponza) |

# [Build Instruction](https://github.com/gpuwangge/LuminError/blob/main/BuildInstruction.md)  



