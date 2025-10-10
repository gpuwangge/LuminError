#ifndef H_CONTROLNODE
#define H_CONTROLNODE

#include "entity.h"
#include "object.h"
#include "textManager.h"

class CApplication;

/******************
* ControlNode
*******************/
enum MagnetTypes {
    MAGNETDISABLE = 0,
    MAGNETLEFT,
    MAGNETRIGHT,
    MAGNETTOP,
    MAGNETBOTTOM
};

class CControlNode : public CEntity{
public:
    CControlNode(){
        //std::cout<<"Creating Control Node"<<std::endl;
        Length_original = glm::vec3(1, 1, 0);
        SetRotation(0, 0, 0);
        //SetScaleRectangleXY(0.0, -1.0, 0.5, -0.5); //set position, scale and length
    }
    ~CControlNode(){}

    CApplication *m_pApp;
    bool bVisible = true;
    int m_object_count = 0;
    int m_textbox_count = 0;
    int m_light_count = 0;
    //MagnetTypes m_magnetType = MAGNETRIGHT;
    void Magnet(MagnetTypes type);

    std::vector<CObject*> m_pObjects;
    std::vector<CTextbox*> m_pTextboxes;

    void Register(CApplication *p_app);
    virtual void RegisterObject(int startIndex);
    virtual void RegisterTextbox(int startIndex);
    virtual void Update();
};

/******************
* ControlPerfMetric
*******************/
class CControlPerfMetric : public CControlNode{
public:
    CControlPerfMetric();
    ~CControlPerfMetric(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlAttachment
*******************/
class CControlAttachment : public CControlNode{
public:
    CControlAttachment();
    ~CControlAttachment(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlGraphicsUniform
*******************/
class CControlGraphicsUniform : public CControlNode{
public:
    CControlGraphicsUniform();
    ~CControlGraphicsUniform(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlComputeUniform
*******************/
class CControlComputeUniform : public CControlNode{
public:
    CControlComputeUniform();
    ~CControlComputeUniform(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};


/******************
* ControlSubpass
*******************/
class CControlSubpass : public CControlNode{
public:
    CControlSubpass();
    ~CControlSubpass(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlHotkey
*******************/
class CControlHotkey : public CControlNode{
public:
    CControlHotkey();
    ~CControlHotkey(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlFeature
*******************/
class CControlFeature : public CControlNode{
public:
    CControlFeature();
    ~CControlFeature(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};

/******************
* ControlStatistics
*******************/
class CControlStatistics : public CControlNode{
public:
    CControlStatistics();
    ~CControlStatistics(){}

    void RegisterObject(int startIndex) override;
    void RegisterTextbox(int startIndex) override;
    void Update() override;
};




#endif