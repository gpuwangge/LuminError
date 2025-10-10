#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
	struct CustomUniformBufferObject {
		glm::vec3 cameraPos;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
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
		customUBO.cameraPos = mainCamera.Position;
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, customUBO);

		objects[0].SetVelocity(glm::cross(objects[0].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[1].SetVelocity(glm::cross(objects[1].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[2].SetVelocity(glm::cross(objects[2].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[3].SetVelocity(glm::cross(objects[3].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[4].SetVelocity(glm::cross(objects[4].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
		objects[5].SetVelocity(glm::cross(objects[5].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif