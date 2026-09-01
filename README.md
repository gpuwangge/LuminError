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



对于我之前做的dragon和sponza的project，分别写一段介绍的文字

下面是偏作品集 / 项目汇报风格的版本，结合你此前的 Vulkan 光线追踪渲染器开发背景来写。

Dragon 项目
Dragon 项目以高面数 Dragon 模型为核心测试场景，重点验证自研 Vulkan Ray Tracing 渲染管线在复杂几何条件下的正确性与稳定性。项目完成了模型与材质数据的加载、底层加速结构（BLAS）和顶层加速结构（TLAS）的构建、光线追踪管线与 Shader Binding Table 配置，并通过 ray generation、closest-hit 和 miss shader 实现基础的光照、阴影与反射效果。由于 Dragon 模型几何细节密集，该场景也用于检查 BVH/AS 构建、实例变换、法线插值、命中信息读取以及 GPU 资源同步等关键环节，帮助定位和解决复杂网格在光线追踪渲染中的可见性、材质绑定与数值精度问题。

Sponza 项目
Sponza 项目以经典的室内建筑场景为基础，面向更接近真实应用的多物体、多材质、层次化空间渲染需求。相比单一主体的 Dragon 场景，Sponza 包含大量独立网格、纹理和材质区域，能够更全面地验证 glTF/GLB 资产解析、PBR 材质参数处理、纹理采样、descriptor 管理及实例级加速结构组织流程。项目在 Vulkan 光线追踪框架中实现了场景加载与 GPU 资源上传，并利用光线追踪计算直接光照、阴影、反射及环境贡献；同时，它也是评估渲染器在复杂遮挡关系、不同材质响应和大规模场景数据下性能与正确性的主要测试案例。

语言简洁一点，介绍一下各自用了那些算法，比如dragon用了whitted style，sponza用了path tracing和NEE。再介绍下dragon和sponza的模型来源出处，和其在图形学研究上的地位

Dragon 项目
Dragon 项目使用 Whitted-style Ray Tracing 实现基础的递归光线追踪。渲染器从相机发射主光线，计算局部光照与阴影，并在命中镜面或透明材质时递归追踪反射、折射光线，因此适合验证光线—三角形求交、法线与材质计算、递归反射，以及 Vulkan Ray Tracing 中 BLAS/TLAS 和 Shader Binding Table 等核心流程。

使用的 Stanford Dragon 是由 Brian Curless 和 Marc Levoy 于 1996 年在 Stanford Graphics Lab 通过三维扫描龙形雕塑生成的经典测试模型，包含约 87 万个三角形。它因几何复杂、细节丰富而长期用于测试三维扫描、网格处理、加速结构构建和高质量渲染算法，是计算机图形学中非常有代表性的 benchmark 模型。

Sponza 项目
Sponza 项目采用 Monte Carlo Path Tracing 渲染复杂室内场景，并加入 Next Event Estimation（NEE）。Path Tracing 通过随机采样 BSDF 方向来估计多次光线反弹带来的间接光照；NEE 则在每次表面命中时显式采样光源并发射阴影光线，提高直接光照的收敛速度、降低噪声。该项目主要用于验证多材质场景加载、PBR 材质、纹理采样、间接光照、阴影遮挡与多次反弹路径的累积渲染。

Sponza Atrium 是由 Marko Dabrović 创建、以克罗地亚杜布罗夫尼克 Sponza Palace 的中庭为原型的场景模型。它最初为渲染竞赛制作，随后因其复杂的室内遮挡结构、大量材质与纹理、明显的明暗区域和适合观察全局光照的空间布局，被全球光照研究与实时渲染社区广泛采用，现已成为测试 GI、Path Tracing、光照采样、去噪和渲染性能的经典场景。

算法差异
项目	核心算法	主要效果与用途
Dragon	Whitted-style Ray Tracing	直接光照、硬阴影、递归镜面反射与折射；适合验证传统递归光线追踪流程
Sponza	Path Tracing + NEE	多次间接反弹、全局光照、颜色串扰，以及通过显式光源采样降低噪声
简而言之，Dragon 更侧重于实现和验证经典递归光线追踪；Sponza 则更侧重于 Monte Carlo 全局光照，以及复杂真实场景中的收敛质量与渲染器可扩展性。Whitted-style 方法通常显式计算反射、折射和局部光照；Path Tracing 则通过随机路径估计光传输，而 NEE 让路径不必仅靠随机反弹“碰巧”命中光源。

翻译英文：模型是Stanford Graphics Lab通过三维扫描龙形雕塑生成的经典测试模型，包含约 87 万个三角形。  
Dragon 模型几何细节密集，用于检查 BVH/AS 构建、实例变换、法线插值、命中信息读取以及 GPU 资源同步等环节。  
光追算法：Whitted-style Ray Tracing。
The model is a classic test asset created by the Stanford Graphics Lab through 3D scanning of a dragon sculpture. It contains approximately 870,000 triangles.

With its dense geometric detail, the Dragon model was used to validate key stages of the rendering pipeline, including BVH/acceleration-structure construction, instance transformations, normal interpolation, hit-record retrieval, and GPU resource synchronization.

Ray-tracing algorithm: Whitted-style Ray Tracing.

翻译英文：Sponza是以克罗地亚杜布罗夫尼克Sponza Palace的中庭为原型的场景模型。它最初为渲染竞赛制作，随后因其复杂的室内遮挡结构、大量材质与纹理、明显的明暗区域和适合观察全局光照的空间布局，被全球光照研究与实时渲染社区广泛采用，现已成为测试 GI、Path Tracing、光照采样、去噪和渲染性能的经典场景。  
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



