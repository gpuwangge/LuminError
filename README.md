<p float="left">
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/vulkanLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/windowsLogo.png" alt="alt text" width="150" height="120">  
  <img src="https://github.com/gpuwangge/VulkanPlatform/blob/main/images/androidLogo.png" alt="alt text" width="220" height="120">  
</p>

# LuminError
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







# [Build Instruction](https://github.com/gpuwangge/LuminError/blob/main/BuildInstruction.md)  



