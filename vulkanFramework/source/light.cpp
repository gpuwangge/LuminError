#include "../include/light.h"
//#include "../include/application.h"

void CLight::Update(float deltaTime, int currentFrame, Camera &mainCamera, Camera &lightCamera){
    if(!bRegistered) return;
    if(!bUpdate) return;

    //update camera pos to ubo, then memcpy to GPU memory
    if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightCameraProj = lightCamera.matrices.projection;
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightCameraView = lightCamera.matrices.view;

        //update intensity to ubo
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].ambientIntensity = m_intensity[0];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].diffuseIntensity = m_intensity[1];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].specularIntensity = m_intensity[2];
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].dimmerSwitch = m_intensity[3];
        
        //update camera to ubo
        CGraphicsDescriptorManager::m_lightingUBO.mainCameraPos = glm::vec4(mainCamera.Position, 0);

        //update light pos, direction and color
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightPos = glm::vec4(m_position, 0);
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightDir = glm::vec4(lightCamera.DirectionFront, 0);
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].lightColor = glm::vec4(m_color, 1.0f);
  
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].spotInnerAngle = cos(glm::radians(m_spotOuterAngle)); 
        CGraphicsDescriptorManager::m_lightingUBO.lights[m_light_id].spotOuterAngle = cos(glm::radians(m_spotInnerAngle));


        //memcpy to GPU memory 
        memcpy(CGraphicsDescriptorManager::m_lightingUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::m_lightingUBO, sizeof(CGraphicsDescriptorManager::m_lightingUBO));
    }

}

void CLight::Register(std::string lightName, int lightId, glm::vec3 lightPos, std::vector<float> &lightIntensity, glm::vec3 lightColor, float spotInnerAngle, float spotOuterAngle){
    m_name = lightName;
    m_light_id = lightId;
    SetLightPosition(lightPos);
    SetLightIntensity(lightIntensity);
    m_color = lightColor;
    m_spotInnerAngle = spotInnerAngle;
    m_spotOuterAngle = spotOuterAngle;
    bRegistered = true;
}