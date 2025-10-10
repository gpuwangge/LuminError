#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CApplication{
//a changeing color triangle on the screen
public:
	struct CustomUniformBufferObject {
		glm::vec4 color;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	void initialize(){
		appInfo.Uniform.GraphicsCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.GraphicsCustom.Binding = CustomUniformBufferObject::GetBinding();
		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUBO.color = {(sin(elapseTime*3) + 1.0f) / 2.0f, (cos(elapseTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, customUBO);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		objects[0].Draw(0, 3);
	}	
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

