#include "renderProcess.h"
#include "TypeAppInfo.h"
#include "IGameEngine.h"
#include "context.h"

namespace LERenderer{

/**************************
* Attachments(Description) 
**************************/
void CRenderProcess::create_attachmentdescription_shadowmap_depthlight(VkFormat depthFormat){  
	//bUseAttachmentLightDepth = true;

	//added for model
	attachmentdescription_shadowmap_depthlight.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	//attachment_description_light_depth_shadowmap.samples = msaaSamples; //VK_SAMPLE_COUNT_1_BIT
	attachmentdescription_shadowmap_depthlight.samples = VK_SAMPLE_COUNT_1_BIT; //hardware depthbias for shadowmap
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachmentdescription_shadowmap_depthlight.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentdescription_shadowmap_depthlight.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_shadowmap_depthlight.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentdescription_shadowmap_depthlight.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_shadowmap_depthlight.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentdescription_shadowmap_depthlight.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //will automatically change at the end of renderpass
	//attachment_description_light_depth_shadowmap.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void CRenderProcess::create_attachmentdescription_mainscene_depthlight(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentLightDepth = true;

	//added for model
	attachmentdescription_mainscene_depthlight.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachmentdescription_mainscene_depthlight.samples = msaaSamples; //VK_SAMPLE_COUNT_1_BIT
	//attachment_description_light_depth.samples = VK_SAMPLE_COUNT_1_BIT; //hardware depthbias for shadowmap
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachmentdescription_mainscene_depthlight.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentdescription_mainscene_depthlight.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_depthlight.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentdescription_mainscene_depthlight.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_depthlight.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	attachmentdescription_mainscene_depthlight.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //will automatically change at the end of renderpass

}

void CRenderProcess::create_attachmentdescription_mainscene_depthcamera(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples){  
	//bUseAttachmentDepth = true;

	//added for model
	attachmentdescription_mainscene_depthcamera.format = depthFormat;//findDepthFormat();
	//std::cout<<"addDepthAttachment::depthAttachment.format = "<<depthAttachment.format<<std::endl;
	attachmentdescription_mainscene_depthcamera.samples = msaaSamples;
	//std::cout<<"attachment_description_depth.samples = "<<attachment_description_depth.samples<<std::endl;
	attachmentdescription_mainscene_depthcamera.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//VK_ATTACHMENT_LOAD_OP_LOAD;//VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentdescription_mainscene_depthcamera.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_depthcamera.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentdescription_mainscene_depthcamera.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_depthcamera.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentdescription_mainscene_depthcamera.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

}

void CRenderProcess::create_attachmentdescription_mainscene_colorresolve(VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, VkImageLayout imageLayout){  
	//Concept of attachment in Vulkan is like render target in OpenGL
	//Subpass is a procedure to write/read attachments (a render process can has many subpasses, aka a render pass)
	//Subpass is designed for mobile TBDR architecture
	//At the beginning of subpass, attachment is loaded; at the end of attachment, attachment is stored
	//bUseAttachmentColorMultisample = true;

	attachmentdescription_mainscene_colorresolve.format = swapChainImageFormat;
	//std::cout<<"addColorAttachment::colorAttachment.format = "<<colorAttachment.format<<std::endl;
	attachmentdescription_mainscene_colorresolve.samples = msaaSamples;
	//std::cout<<"attachment_description_color_multisample.samples = "<<attachment_description_color_multisample.samples<<std::endl;
	attachmentdescription_mainscene_colorresolve.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentdescription_mainscene_colorresolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentdescription_mainscene_colorresolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentdescription_mainscene_colorresolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_colorresolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentdescription_mainscene_colorresolve.finalLayout = imageLayout;

	m_msaaSamples_renderProcess = msaaSamples;
	//std::cout<<"Color Attachment added. "<<std::endl;
}

void CRenderProcess::create_attachmentdescription_mainscene_colorpresent(VkFormat swapChainImageFormat){  
	//bUseAttachmentColorPresent = true;

	attachmentdescription_mainscene_colorpresent.format = swapChainImageFormat;
	attachmentdescription_mainscene_colorpresent.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentdescription_mainscene_colorpresent.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentdescription_mainscene_colorpresent.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentdescription_mainscene_colorpresent.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentdescription_mainscene_colorpresent.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentdescription_mainscene_colorpresent.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentdescription_mainscene_colorpresent.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}



/*********
* Subpasses
**********/
void CRenderProcess::createSubpass_shadowmap(){
	clearValues_shadowmap.push_back({1.0f, 0});

	VkSubpassDescription subpass{};

	attachmentRef_shadowmap_lightdepth.attachment = 0;//iMainSceneAttachmentDepthLight; 
	attachmentRef_shadowmap_lightdepth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.colorAttachmentCount = 0;
	subpass.pColorAttachments = nullptr;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;	
	subpass.pDepthStencilAttachment = &attachmentRef_shadowmap_lightdepth;

	subpasses_shadowmap.push_back(subpass);
}

void CRenderProcess::createSubpass_mainscene(int attachment_id_to_observe){ 
	if(iMainSceneAttachmentDepthLight >= 0) clearValues.push_back({1.0f, 0}); 
	if(iMainSceneAttachmentDepthCamera >= 0) clearValues.push_back({1.0f, 0}); 
	if(iMainSceneAttachmentColorResovle >= 0) clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});
	if(iMainSceneAttachmentColorPresent >= 0) clearValues.push_back({0.0f, 0.0f, 0.0f, 1.0f});

	if(bEnableMainSceneRenderpassSubpassShadowmap) createSubpass_mainscene_lightdepth();	
	if(bEnableMainSceneRenderpassSubpassDraw) createSubpass_mainscene_draw();	
	if(bEnableMainSceneRenderpassSubpassObserve) createSubpass_mainscene_observe(attachment_id_to_observe);	
}

void CRenderProcess::createSubpass_mainscene_lightdepth(){ //assume depth and MSAA are enabled
	VkSubpassDescription subpass{};//to generate light depth for shadowmap

	if(iMainSceneAttachmentDepthLight >= 0){
		attachmentRef_mainscene_lightdepth.attachment = iMainSceneAttachmentDepthLight; 
		attachmentRef_mainscene_lightdepth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format
		subpass.pDepthStencilAttachment = &attachmentRef_mainscene_lightdepth;
	}

	subpass.colorAttachmentCount = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
	subpasses_mainscene.push_back(subpass);
}

void CRenderProcess::createSubpass_mainscene_draw(){ 
	VkSubpassDescription subpass{};

	// if(iAttachmentDepthLight >= 0 && iAttachmentDepthCamera >= 0){
	// 	attachmentRef_input_draw[0].attachment = iAttachmentDepthCamera;
	// 	attachmentRef_input_draw[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	// 	attachmentRef_input_draw[1].attachment = iAttachmentDepthLight;
	// 	subpass.pInputAttachments = attachmentRef_input_draw.data();
	// 	subpass.inputAttachmentCount = 2;
	// }

	if(iMainSceneAttachmentDepthLight >= 0){
		attachmentRef_mainscene_draw_input[0].attachment = iMainSceneAttachmentDepthLight;
		attachmentRef_mainscene_draw_input[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		subpass.pInputAttachments = &attachmentRef_mainscene_draw_input[0];
		subpass.inputAttachmentCount = 1;
	}

	if(iMainSceneAttachmentDepthCamera >= 0){
		attachmentRef_mainscene_draw_depth.attachment = iMainSceneAttachmentDepthCamera; 
		attachmentRef_mainscene_draw_depth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //normal depth image format: for shadowmapping, this is input and output
		subpass.pDepthStencilAttachment = &attachmentRef_mainscene_draw_depth;
	}
	if(iMainSceneAttachmentColorPresent >= 0){
		attachmentRef_mainscene_draw_color.attachment = iMainSceneAttachmentColorPresent; 
		attachmentRef_mainscene_draw_color.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_mainscene_draw_color;
		subpass.colorAttachmentCount = 1;
	}
	if(iMainSceneAttachmentColorResovle >= 0){
		attachmentRef_mainscene_draw_color_multisample.attachment = iMainSceneAttachmentColorResovle; 
		attachmentRef_mainscene_draw_color_multisample.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_mainscene_draw_color_multisample; //use multi sampler color attachment to override pColorAttachments
		subpass.pResolveAttachments = &attachmentRef_mainscene_draw_color;
	}

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
	subpasses_mainscene.push_back(subpass);
}

void CRenderProcess::createSubpass_mainscene_observe(int attachment_id_to_observe){ 
	VkSubpassDescription subpass{};

	attachmentRef_mainscene_observe.attachment = attachment_id_to_observe; //iMainSceneAttachmentDepthCamera or iMainSceneAttachmentDepthLight
	attachmentRef_mainscene_observe.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; //depth image format for output
	subpass.pInputAttachments = &attachmentRef_mainscene_observe;
	subpass.inputAttachmentCount = 1;

	if(iMainSceneAttachmentColorPresent >= 0){
		attachmentRef_mainscene_observe_color.attachment = iMainSceneAttachmentColorPresent; 
		attachmentRef_mainscene_observe_color.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_mainscene_observe_color;
		subpass.colorAttachmentCount = 1;
	}
	if(iMainSceneAttachmentColorResovle >= 0){
		attachmentRef_mainscene_observe_color_multisample.attachment = iMainSceneAttachmentColorResovle; 
		attachmentRef_mainscene_observe_color_multisample.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &attachmentRef_mainscene_observe_color_multisample;
		subpass.pResolveAttachments = &attachmentRef_mainscene_observe_color;
	}

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses_mainscene.push_back(subpass);
}


/*********
* Dependency
**********/
void CRenderProcess::createDependency_shadowmap(){
	//TODO
}

void CRenderProcess::createDependency_mainscene(){  
	if(!bEnableMainSceneRenderpassSubpassShadowmap && bEnableMainSceneRenderpassSubpassDraw && !bEnableMainSceneRenderpassSubpassObserve){ //single subpass
		dependencies_mainscene.resize(1);

		//external->0
		if (iMainSceneAttachmentDepthCamera < 0) {
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies_mainscene[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies_mainscene[0].dstSubpass = 0;
			dependencies_mainscene[0].srcStageMask = srcPipelineStageFlag;
			dependencies_mainscene[0].srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependencies_mainscene[0].dstStageMask = dstPipelineStageFlag;
			dependencies_mainscene[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}else{
			VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies_mainscene[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies_mainscene[0].dstSubpass = 0;
			dependencies_mainscene[0].srcStageMask = srcPipelineStageFlag;
			dependencies_mainscene[0].srcAccessMask = 0;//VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			dependencies_mainscene[0].dstStageMask = dstPipelineStageFlag;
			dependencies_mainscene[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			dependencies_mainscene[0].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
	}

	if(!bEnableMainSceneRenderpassSubpassShadowmap && bEnableMainSceneRenderpassSubpassDraw && bEnableMainSceneRenderpassSubpassObserve){ //two subpasses
		dependencies_mainscene.resize(2);

		//0->1
        dependencies_mainscene[0].srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependencies_mainscene[0].dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)

		dependencies_mainscene[0].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependencies_mainscene[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependencies_mainscene[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependencies_mainscene[0].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependencies_mainscene[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance
	}

	if(bEnableMainSceneRenderpassSubpassShadowmap && bEnableMainSceneRenderpassSubpassDraw && bEnableMainSceneRenderpassSubpassObserve){
		dependencies_mainscene.resize(3);

		//0->1
        dependencies_mainscene[0].srcSubpass = 0;//subpass0 is the src subpass, write in color image, output depth image
		dependencies_mainscene[0].dstSubpass = 1;//subpass1 is the dst subpass, input depth image, output color image(fragment shader convert depth image to color image)
		//
		dependencies_mainscene[0].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this is to make sure subpass0 finishes depth/color, then begin subpass1
		dependencies_mainscene[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //subpass0 need write depth image, subpass1 cant not access depth image before subpass 0 finish it
		dependencies_mainscene[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //this is to say subpass1's fragment shader can read depth image that subpass1 wrote(as input attachment)
		dependencies_mainscene[0].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; //subpass1 need to read input attachment(that subpass0 generate)

		dependencies_mainscene[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; //sync region instead of all framebuffer, to improve performance

		//1->2
		dependencies_mainscene[1].srcSubpass = 1;
		dependencies_mainscene[1].dstSubpass = 2;
		//
		dependencies_mainscene[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies_mainscene[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies_mainscene[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies_mainscene[1].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		dependencies_mainscene[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		//0->2
		dependencies_mainscene[2].srcSubpass = 0;
		dependencies_mainscene[2].dstSubpass = 2;

		dependencies_mainscene[2].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies_mainscene[2].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies_mainscene[2].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies_mainscene[2].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;

		dependencies_mainscene[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	}
}


/*********
* Renderpass
**********/
void CRenderProcess::createRenderPass_shadowmap(){ //pAttachments order must match CSwapchain::CreateFramebuffer_shadowmap0/1()
	//std::cout<<"Begin create renderpass"<<std::endl;
	VkResult result = VK_SUCCESS;

	//attachment descriptions in renderpass must match the same order as in framebuffer:
	//#1: a color attachment with sampler number = 1
	//#2: if depth test is enabled, a depth attachment with sampler number = n
	//#3: if msaa is enabled, a color attachment with sampler number = n
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	//if(iMainSceneAttachmentDepthLight >= 0) 
	attachmentDescriptions.push_back(attachmentdescription_shadowmap_depthlight);

	//std::cout<<"Begin prepare renderpass info"<<std::endl;
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data(); //1
	renderPassInfo.subpassCount = subpasses_shadowmap.size();//1;
	renderPassInfo.pSubpasses = subpasses_shadowmap.data();//&subpass;//2
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = nullptr; //&dependency;//3
	//std::cout<<"Done prepare renderpass info: "<< subpasses.size()<<", "<<attachmentDescriptions.size()<<std::endl;

	result = vkCreateRenderPass(CContext::GetHandle().GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass_shadowmap);
	//std::cout<<"Done create renderpass"<<std::endl;
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");	 

}

void CRenderProcess::createRenderPass_mainscene(){ //pAttachments order must match CSwapchain::CreateFramebuffer_mainscene()
	//std::cout<<"Begin create renderpass"<<std::endl;
	VkResult result = VK_SUCCESS;

	//attachment descriptions in renderpass must match the same order as in framebuffer:
	//#1: a color attachment with sampler number = 1
	//#2: if depth test is enabled, a depth attachment with sampler number = n
	//#3: if msaa is enabled, a color attachment with sampler number = n
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	if(iMainSceneAttachmentDepthLight >= 0) attachmentDescriptions.push_back(attachmentdescription_mainscene_depthlight);
	if(iMainSceneAttachmentDepthCamera >= 0) attachmentDescriptions.push_back(attachmentdescription_mainscene_depthcamera);
	if(iMainSceneAttachmentColorResovle >= 0) attachmentDescriptions.push_back(attachmentdescription_mainscene_colorresolve);
	if(iMainSceneAttachmentColorPresent >= 0) attachmentDescriptions.push_back(attachmentdescription_mainscene_colorpresent);

	//std::cout<<"Begin prepare renderpass info"<<std::endl;
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data(); //1
	renderPassInfo.subpassCount = subpasses_mainscene.size();//1;
	renderPassInfo.pSubpasses = subpasses_mainscene.data();//&subpass;//2
	renderPassInfo.dependencyCount = dependencies_mainscene.size();
	renderPassInfo.pDependencies = dependencies_mainscene.data(); //&dependency;//3
	//std::cout<<"Done prepare renderpass info: subpasses_mainscene.size()="<< subpasses_mainscene.size()<<", attachmentDescriptions.size()="<<attachmentDescriptions.size()<<std::endl;

	result = vkCreateRenderPass(CContext::GetHandle().GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass_mainscene);
	//std::cout<<"Done create renderpass"<<std::endl;
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");	 


}


/*********
* Help variables and functions
**********/
void CRenderProcess::addColorBlendAttachment(VkBlendOp colorBlendOp, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
											 VkBlendOp alphaBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor){
	bUseColorBlendAttachment = true;

	//Blend Algorithm
	//oldColor: the color already in framebuffer
	//newColor: the color output from fragment shader
	//if blendEnable:
	//  finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb)
	//  finalColor.a   = (srcAlphaBlendFactor * newColor.a  ) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a  )
	//else:
	//  finalColor = newColor

	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendAttachment.blendEnable = VK_TRUE;

	colorBlendAttachment.colorBlendOp = colorBlendOp;
	colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
	colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
	colorBlendAttachment.alphaBlendOp = alphaBlendOp;
	colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
	colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
}

 /*******************************
* Pipeline Layouts and Pipelines
********************************/
void CRenderProcess::createComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout){
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) 
		throw std::runtime_error("failed to create compute pipeline layout!");
}
void CRenderProcess::createComputePipeline(VkShaderModule &computeShaderModule){
	bCreateComputePipeline = true;
	
	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStageInfo.module = computeShaderModule;
	computeShaderStageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = computePipelineLayout;
	pipelineInfo.stage = computeShaderStageInfo;

	if (vkCreateComputePipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}
}

void CRenderProcess::createRaytracingPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout){
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &raytracingPipelineLayout) != VK_SUCCESS) 
		throw std::runtime_error("failed to create raytracing pipeline layout!");
}
// void CRenderProcess::createRaytracingPipeline(VkShaderModule &raytracingShaderModule){
// 	bCreateRaytracingPipeline = true;
	
// 	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
// 	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
// 	computeShaderStageInfo.module = raytracingShaderModule;
// 	computeShaderStageInfo.pName = "main";

// 	VkComputePipelineCreateInfo pipelineInfo{};
// 	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
// 	pipelineInfo.layout = raytracingPipelineLayout;
// 	pipelineInfo.stage = computeShaderStageInfo;

// 	if (vkCreateComputePipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &raytracingPipeline) != VK_SUCCESS) {
// 		throw std::runtime_error("failed to create raytracing pipeline!");
// 	}
// }

bool CRenderProcess::LoadRayTracingFunctions_process(){
	fpCreateRayTracingPipelinesKHR =
        reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(CContext::GetHandle().GetLogicalDevice(), "vkCreateRayTracingPipelinesKHR"));

	// fpCmdTraceRaysKHR =
    //     reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
    //         vkGetDeviceProcAddr(CContext::GetHandle().GetLogicalDevice(), "vkCmdTraceRaysKHR"));

	bool ok = true;

    if (!fpCreateRayTracingPipelinesKHR) {
        //logger->Log("Missing vkCreateRayTracingPipelinesKHR\n");
        ok = false;
    }
	// if (!fpCmdTraceRaysKHR) {
    //     //logger->Log("Missing vkCmdTraceRaysKHR\n");
    //     ok = false;
    // }

	return ok;
}
void CRenderProcess::createRaytracingPipeline(VkShaderModule& rgenModule, VkShaderModule& primaryMissModule, VkShaderModule& shadowMissModule, VkShaderModule& primaryRchitModule, VkShaderModule& shadowRchitModule){
    bCreateRaytracingPipeline = true;

    std::array<VkPipelineShaderStageCreateInfo, 5> stages{};

    // stage 0 : raygen
    stages[0] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stages[0].stage  = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    stages[0].module = rgenModule;
    stages[0].pName  = "main";

    // stage 1 : primary miss
    stages[1] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stages[1].stage  = VK_SHADER_STAGE_MISS_BIT_KHR;
    stages[1].module = primaryMissModule;
    stages[1].pName  = "main";

    // stage 2 : shadow miss
    stages[2] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stages[2].stage  = VK_SHADER_STAGE_MISS_BIT_KHR;
    stages[2].module = shadowMissModule;
    stages[2].pName  = "main";

    // stage 3 : primary closest hit
    stages[3] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stages[3].stage  = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    stages[3].module = primaryRchitModule;
    stages[3].pName  = "main";

	// stage 4 : shadow closest hit
    stages[4] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stages[4].stage  = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    stages[4].module = shadowRchitModule;
    stages[4].pName  = "main";

    std::array<VkRayTracingShaderGroupCreateInfoKHR, 5> groups{};

    // group 0 : raygen
    groups[0] = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
    groups[0].type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    groups[0].generalShader      = 0;
    groups[0].closestHitShader   = VK_SHADER_UNUSED_KHR;
    groups[0].anyHitShader       = VK_SHADER_UNUSED_KHR;
    groups[0].intersectionShader = VK_SHADER_UNUSED_KHR;

    // group 1 : primary miss
    groups[1] = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
    groups[1].type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    groups[1].generalShader      = 1;
    groups[1].closestHitShader   = VK_SHADER_UNUSED_KHR;
    groups[1].anyHitShader       = VK_SHADER_UNUSED_KHR;
    groups[1].intersectionShader = VK_SHADER_UNUSED_KHR;

    // group 2 : shadow miss
    groups[2] = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
    groups[2].type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    groups[2].generalShader      = 2;
    groups[2].closestHitShader   = VK_SHADER_UNUSED_KHR;
    groups[2].anyHitShader       = VK_SHADER_UNUSED_KHR;
    groups[2].intersectionShader = VK_SHADER_UNUSED_KHR;

    // group 3 : triangles hit group
    groups[3] = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
    groups[3].type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    groups[3].generalShader      = VK_SHADER_UNUSED_KHR;
    groups[3].closestHitShader   = 3;
    groups[3].anyHitShader       = VK_SHADER_UNUSED_KHR;
    groups[3].intersectionShader = VK_SHADER_UNUSED_KHR;

	// group 4: shadow hit group
	groups[4] = { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
	groups[4].type 					= VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
	groups[4].generalShader 		= VK_SHADER_UNUSED_KHR;
	groups[4].closestHitShader 		= 4;
	groups[4].anyHitShader 			= VK_SHADER_UNUSED_KHR;
	groups[4].intersectionShader 	= VK_SHADER_UNUSED_KHR;

    VkRayTracingPipelineCreateInfoKHR pipelineInfo{
        VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR
    };
    pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
    pipelineInfo.pStages    = stages.data();
    pipelineInfo.groupCount = static_cast<uint32_t>(groups.size());
    pipelineInfo.pGroups    = groups.data();
    pipelineInfo.maxPipelineRayRecursionDepth = 2;
    pipelineInfo.layout = raytracingPipelineLayout;

    if (!LoadRayTracingFunctions_process()) {
        std::cout << "failed to load ray tracing functions!" << std::endl;
        throw std::runtime_error("failed to load ray tracing functions!");
    }

    VkResult result = fpCreateRayTracingPipelinesKHR(
        CContext::GetHandle().GetLogicalDevice(),
        VK_NULL_HANDLE,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &raytracingPipeline);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create ray tracing pipeline!");
    }
}

void CRenderProcess::createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id){
	VkPushConstantRange dummyPushConstantRange;
	createGraphicsPipelineLayout(descriptorSetLayouts, dummyPushConstantRange, false, graphicsPipelineLayout_id);
}
void CRenderProcess::createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id){
	VkResult result = VK_SUCCESS;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	// if (pt == PIPELINE_COMPUTE) {
	// 	pipelineLayoutInfo.setLayoutCount = 0;
	// 	pipelineLayoutInfo.pSetLayouts = nullptr;
	// }else {
	//std::cout<<"DEBUG: create graphics pipeline layout: descriptorSetLayouts.size()="<<descriptorSetLayouts.size()<<std::endl;
	pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();//  descriptorSetLayout;//todo: LAYOUT
	//}

	if(bUsePushConstant){
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	}

	//Create Graphics Pipeline Layout
	VkPipelineLayout newlayout;
	graphicsPipelineLayouts.push_back(newlayout);
	//std::cout<<"before vkCreatePipelineLayout()"<<std::endl;
	result = vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &graphicsPipelineLayouts[graphicsPipelineLayout_id]);
	//std::cout<<"after vkCreatePipelineLayout()"<<std::endl;
	//result = vkCreatePipelineLayout(CContext::GetHandle().GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout);
	
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout!");
	//REPORT("vkCreatePipelineLayout");
}

void CRenderProcess::createGraphicsPipeline(GetBindingDescFunc getBindingDesc, GetAttributeDescFunc getAttributeDesc,
	VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer, bool bUseInstanceBuffer,
	VkRenderPass renderPass, int graphcisPipeline_id, AppInfo *appInfo){
	bool bVerbose = false;
	//HERE_I_AM("CreateGraphicsPipeline");
	bCreateGraphicsPipeline = true;

	VkResult result = VK_SUCCESS;

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	

	/*********1 Asemble Shader**********/
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	
	/*********2 Asemble Vertex Info**********/
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	VkVertexInputBindingDescription bindingDescription{}; //must declare these outside of blocks
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions; //must declare these outside of blocks

	if(bVerbose) std::cout<<"createGraphicsPipeline(): bUseInstanceBuffer = "<<bUseInstanceBuffer<<", bUseVertexBuffer = "<<bUseVertexBuffer<<std::endl;
	std::array<VkVertexInputBindingDescription, 2> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> vertexAttributes;
	std::array<VkVertexInputAttributeDescription, 4> instanceAttributes;
	if(bUseInstanceBuffer){ //use two bindings together
		bindingDescriptions = std::array{
			TextQuadVertex::getBindingDescription(),
			TextInstanceData::getBindingDescription()
		};
		// auto bindingDescriptions = std::array{
		// 	TextQuadVertex::getBindingDescription(),
		// 	TextInstanceData::getBindingDescription()
		// };
		vertexAttributes = TextQuadVertex::getAttributeDescriptions();
		instanceAttributes = TextInstanceData::getAttributeDescriptions();
		//auto vertexAttributes = TextQuadVertex::getAttributeDescriptions();
		//auto instanceAttributes = TextInstanceData::getAttributeDescriptions();
		
		attributeDescriptions.insert(attributeDescriptions.end(), vertexAttributes.begin(), vertexAttributes.end());
		attributeDescriptions.insert(attributeDescriptions.end(), instanceAttributes.begin(), instanceAttributes.end());

		//vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	}else if(bUseVertexBuffer){ //use one binding
		bindingDescription = getBindingDesc();
		attributeDescriptions = getAttributeDesc();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}   
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineInfo.pVertexInputState = &vertexInputInfo;

	/*********3 Assemble**********/
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	pipelineInfo.pInputAssemblyState = &inputAssembly;

	/*********4 Viewport**********/
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	pipelineInfo.pViewportState = &viewportState;
	

	/*********5 Rasterizazer**********/
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	bool bEnableDepthBias = appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_renderpasses_shadowmap;
	if(bEnableDepthBias){
		//std::cout<< "Enable depthBiasEnable for rasterizer" << std::endl;
		rasterizer.depthBiasEnable = VK_TRUE; // for hardware depthibias shadowmap
		rasterizer.depthBiasConstantFactor = 1.25f;   // for hardware depthibias shadowmap
		rasterizer.depthBiasClamp = 0.0f; // for hardware depthibias shadowmap
		rasterizer.depthBiasSlopeFactor = 1.75f;      // for hardware depthibias shadowmap
	}else{
		//std::cout<< "Disable depthBiasEnable" << std::endl;
		rasterizer.depthBiasEnable = VK_FALSE;
	}
	pipelineInfo.pRasterizationState = &rasterizer;
	

	/*********6 Multisample**********/
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	if(bEnableDepthBias)
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //for hardware depthbias shadowmap, we use 1 sample
	else
		multisampling.rasterizationSamples = m_msaaSamples_renderProcess;
	pipelineInfo.pMultisampleState = &multisampling; 

	/*********7 Color Blend**********/
	//VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	//bUseColorBlendAttachment: global blend switch
	//blendEnable: pipeline blend switch
	//if either switch is off, disable blend
	if(!appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_blend_enable || !bUseColorBlendAttachment){
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE; //todo?
	}

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
	pipelineInfo.pColorBlendState = &colorBlending;	//7 
	

	/*********8**********/
	//tell gpu which part will be changed, so need update these for each frame
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	if(bEnableDepthBias){
		dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
	}
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();
	pipelineInfo.pDynamicState = &dynamicState;	//8 
		

	/*********9 Layout(Vulkan Special Concept)**********/
	//CreateLayout(descriptorSetLayout);
	pipelineInfo.layout = graphicsPipelineLayouts[graphcisPipeline_id];	//9
	//pipelineInfo.layout = graphicsPipelineLayout;	//9
	
	
	/*********10 Renderpass Layout(Vulkan Special Concept)**********/
	//Renderpass is to specify what kind of data goes to graphics pipeline
	pipelineInfo.renderPass = renderPass;	//10 
	pipelineInfo.subpass = appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_subpasses_subpass_id; 
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	/*********11**********/
	if (iMainSceneAttachmentDepthCamera >= 0) {
		//bool bSkybox = false;
		//if(graphicsPipelines.size() == skyboxID) bSkybox = true;
		//std::cout<<"bSkybox="<<bSkybox<<"(skyboxID="<<skyboxID<<")"<<std::endl;
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_depth_test_enable;// VK_TRUE; //change this to true will make text invisible
		depthStencil.depthWriteEnable = appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_depth_write_enable;// VK_TRUE;
		if(bEnableDepthBias)
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; //for hardware depthbias shadowmap
		else
			depthStencil.depthCompareOp = appInfo->GraphicsPipelines[graphcisPipeline_id].graphics_pipeline_skybox ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		pipelineInfo.pDepthStencilState = &depthStencil;
	}

	/*********Create Graphics Pipeline**********/
	VkPipeline newpipeline;
	graphicsPipelines.push_back(newpipeline);
	if(bVerbose) std::cout<<"begin create graphics pipeline... "<<std::endl;
	result = vkCreateGraphicsPipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipelines[graphcisPipeline_id]);
	if(bVerbose) std::cout<<"done create graphcis pipeline "<<std::endl;
	//result = vkCreateGraphicsPipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");
	//REPORT("vkCreateGraphicsPipelines");


	/*********Clean up**********/
	

	//HERE_I_AM("DrawFrame() will begin");
}


void CRenderProcess::Cleanup(){
	if(renderPass_shadowmap != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass_shadowmap, nullptr);
	if(renderPass_mainscene != VK_NULL_HANDLE)
		vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderPass_mainscene, nullptr);

	if(bCreateGraphicsPipeline){
		for(int i = 0; i < graphicsPipelines.size(); i++){
			vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), graphicsPipelines[i], nullptr);
			vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), graphicsPipelineLayouts[i], nullptr);
		}
	}

	if(bCreateComputePipeline){
		vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), computePipeline, nullptr);
    	vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), computePipelineLayout, nullptr);
	}

	if(bCreateRaytracingPipeline){
		vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), raytracingPipeline, nullptr);
		vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), raytracingPipelineLayout, nullptr);
	}
	
}

}//namespace