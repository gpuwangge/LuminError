#ifndef H_LIGHT
#define H_LIGHT

#include "graphicsDescriptor.h"
#include <string>
#include <vector>
#include "camera.hpp"

class CLight {
    std::string m_name;
    int m_light_id;
    glm::vec3 m_position;
    std::vector<float> m_intensity=std::vector<float>(4,0);

    glm::vec3 m_color;
    float m_spotInnerAngle;
    float m_spotOuterAngle;
public:
    CLight() {};

    int GetLightID() {return m_light_id;}
    std::string GetLightName() {return m_name;}
    void SetLightPosition(glm::vec3 lightPos) {m_position = lightPos;}
    void SetLightIntensity(std::vector<float> &lightIntensity) {
        for(int i = 0; i < 4; i++)
            m_intensity[i] = lightIntensity[i];
    }
    glm::vec3 GetLightPosition(){return m_position;};
    float GetIntensity(int i){return m_intensity[i];};

    bool bUpdate = true;
    void Update(float deltaTime, int currentFrame, Camera &mainCamera, Camera &lightCamera);

    bool bRegistered = false;
    void Register(std::string lightName, int lightId, glm::vec3 lightPos, std::vector<float> &lightIntensity, glm::vec3 lightColor, float spotInnerAngle, float spotOuterAngle); 
};

#endif