#ifndef H_ENTITY
#define H_ENTITY

//redundant?
#include "common.h"
#include <string>

class CEntity{
public:
    CEntity(){}
    ~CEntity(){}

    std::string Name;

    enum EntityType { general, camera }; //for first persion type the roll should be ignored
	EntityType entityType = EntityType::general;

    glm::vec3 Length_original = glm::vec3(); //original is the value before scale
    glm::vec3 LengthMin_original = glm::vec3();
    glm::vec3 LengthMax_original = glm::vec3();
    glm::vec3 Length = glm::vec3(); //length after scale, the true "length"

    /******************
    * Tranformation
    *******************/
    glm::vec3 Position = glm::vec3(0.0f); //absolute position coordinate
    glm::vec3 Rotation = glm::vec3(0.0f); //Pitch, Yaw, Roll
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 TranslateMatrix = glm::mat4(1.0f); //16*4=64 bytes
    glm::mat4 RotationMatrix = glm::mat4(1.0f); 
    glm::mat4 ScaleMatrix = glm::mat4(1.0f);
    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    glm::mat4 TransRotation = glm::mat4(1.0f); //Translation * Rotation, no Scale

    glm::vec3 DirectionFront = glm::vec3(0, 0, 1);
    glm::vec3 DirectionUp = glm::vec3(0, 1, 0);
    glm::vec3 DirectionLeft = glm::vec3(1, 0, 0);

    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 AngularVelocity = glm::vec3(0.0f); //RotationSpeed for x, y and z axis

    enum TranslateDirections {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    //glm::vec4 TempVelocity[6];
    std::array<glm::vec4, 6> TempVelocity{ glm::vec4(0.0f) };
    enum RotationDirections {
        PITCHUP,
        PITCHDOWN,
        YAWLEFT,
        YAWRIGHT,
        ROLLLEFT,
        ROLLRIGHT
    };
    //glm::vec4 TempAngularVelocity[6];
    std::array<glm::vec4, 6> TempAngularVelocity{ glm::vec4(0.0f) };


    void MoveForward(float distance, float speed);
    void MoveBackward(float distance, float speed);
    void MoveLeft(float distance, float speed);
    void MoveRight(float distance, float speed);
    void MoveUp(float distance, float speed);
    void MoveDown(float distance, float speed);

    void PitchUp(float angle, float speed);
    void PitchDown(float angle, float speed);
    void YawLeft(float angle, float speed);
    void YawRight(float angle, float speed);
    void RollLeft(float angle, float speed);
    void RollRight(float angle, float speed);

    void SetPosition(float x, float y, float z);
    void SetPositionX(float x){ Position.x = x; };
    void SetPositionY(float y){ Position.y = y; };
    void SetPositionZ(float z){ Position.z = z; };
    void SetPosition(glm::vec3 v);
    void SetRotation(float pitch, float yaw, float roll);
    void SetRotation(glm::vec3 v);

    glm::vec4 TempMoveVelocity = glm::vec4(0.0f);
    void MoveToPosition(float x, float y, float z, float t);
    glm::vec4 TempMoveAngularVelocity = glm::vec4(0.0f);
    void MoveToRotation(float pitch, float yaw, float roll, float t);

    void SetVelocity(float vx, float vy, float vz);
    void SetVelocity(glm::vec3 v);
    void SetAngularVelocity(float vx, float vy, float vz);

    void SetScale(float scale_x, float scale_y, float scale_z);
    void SetScale(float scale){ SetScale(scale, scale, scale); }
    void SetScale(glm::vec3 v){ SetScale(v.x, v.y, v.z); } 
    void SetScaleRectangleXY(float x0, float y0, float x1, float y1); //set 2d image to rect((x0,y0),(x1,y1))
    void UpdateLength();

    void Update(float deltaTime);
};

#endif