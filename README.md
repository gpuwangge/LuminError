<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
</p>

# LuminError 
Vulkan-based real-time rendering and GPU architecture
experimentation framework written in C++.

## Highlights
- Vulkan 1.3/1.4 rendering architecture
- Hardware ray tracing with VK_KHR_acceleration_structure
  and VK_KHR_ray_tracing_pipeline
- BLAS/TLAS construction and GPU resource management
- Whitted-style ray tracing
- Monte Carlo path tracing with Next Event Estimation
- Compute and graphics workloads
- PBR materials and glTF/GLB asset pipeline
- GPU synchronization and descriptor management
- Rendering validation and performance experiments  

<img src="https://github.com/gpuwangge/LuminError/blob/main/images/LuminErrorArch.png" alt="alt text"> 

LuminError is a **Vulkan-based rendering framework for real-time graphics, ray tracing, and GPU experimentation**, written in C++.  

The engine directly manages Vulkan GPU resources, synchronization, command submission, and rendering pipelines, providing explicit control over resource lifetimes and GPU execution behavior. It supports both graphics and compute workloads, as well as **hardware ray tracing** through Vulkan ray-tracing extensions.  

The framework includes core systems for resource and scene management, graphics and compute rendering, and OBJ/glTF/GLB asset pipelines with support for textures and PBR materials. It is designed as an environment for studying and experimenting with **GPU rendering pipelines, ray tracing, resource management, synchronization, and performance**.  

LuminError is under active development, with ongoing work focused on expanding rendering features, GPU workloads, ray-tracing techniques, and performance experiments.  

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


# [Build Instruction](https://github.com/gpuwangge/LuminError/blob/main/BuildInstruction.md)  



