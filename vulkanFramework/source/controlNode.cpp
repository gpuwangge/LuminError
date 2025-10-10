#include "../include/controlNode.h"
#include "../include/application.h"
#include <iostream>

/******************
* ControlNode
*******************/
void CControlNode::Register(CApplication *p_app){
    m_pApp = p_app;
    p_app->objectCountControl += m_object_count;
    p_app->textboxCountControl += m_textbox_count;
    p_app->lightCountControl += m_light_count;
}
void CControlNode::RegisterObject(int startIndex){

}
void CControlNode::RegisterTextbox(int startIndex){

}
void CControlNode::Update(){

}
void CControlNode::Magnet(MagnetTypes type){
   switch(type){
        case MAGNETRIGHT:
            SetPositionX(1.0f - 0.5f * Length.x);
            break;
        case MAGNETLEFT:
            SetPositionX(-1.0f +  0.5f * Length.x);
            break;
        case MAGNETTOP:
            SetPositionY(-1.0f + 0.5f * Length.y);
            break;
        case MAGNETBOTTOM:
            SetPositionY(1.0f - 0.5f * Length.y);
            break;
    }
}


/******************
* ControlPerfMetric
*******************/
CControlPerfMetric::CControlPerfMetric(){
    //std::cout<<"Creating Control PerfMetric Node"<<std::endl;
    Name = "Control PerfMetric Node";
    m_textbox_count = 5;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5f * Length.y);
    Magnet(MAGNETRIGHT);
}

void CControlPerfMetric::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "PerfMetric Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
        //no need to set position/scale, because all object follows control node's model matrix
    }


}

void CControlPerfMetric::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "PerfMetric Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0); //todo: adjust position based on control node scale
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Performance";  //->SetTextContent("PerfMetric");
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlPerfMetric::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);

        for(int i = 1; i <= 4; i++) m_pTextboxes[i]->bFlash = true;

        m_pTextboxes[4]->SetTextContent("Init:" + to_string_prec(m_pApp->totalInitTime) + " ms");

        // if(m_pApp->objects.size()>0) m_pTextboxes[5]->bFlash = true;
        // else m_pTextboxes[5]->SetTextColor(greyColor);
        // m_pTextboxes[5]->SetTextContent("Object:" + std::to_string(m_pApp->objects.size()));

        // if(m_pApp->textManager.m_textBoxes.size()>0) m_pTextboxes[6]->bFlash = true;
        // else m_pTextboxes[6]->SetTextColor(greyColor);
        // m_pTextboxes[6]->SetTextContent("Textbox:" + std::to_string(m_pApp->textManager.m_textBoxes.size()));

        // if(m_pApp->lights.size()>0) m_pTextboxes[7]->bFlash = true;
        // else m_pTextboxes[7]->SetTextColor(greyColor);
        // m_pTextboxes[7]->SetTextContent("Light:" + std::to_string(m_pApp->lights.size()));

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        m_pTextboxes[1]->SetTextContent("FPS:" + std::to_string((int)(1.0f/m_pApp->deltaTime)));
        m_pTextboxes[2]->SetTextContent("Time:" + to_string_prec(m_pApp->elapseTime) + "s");
        m_pTextboxes[3]->SetTextContent("Frame:" + std::to_string(m_pApp->frameCount));

        m_pTextboxes[4]->SetTextContent();
        //for(int i = 4; i <= 7; i++) if(m_pTextboxes[i]->bFlash) m_pTextboxes[i]->SetTextContent(); //to update the text color
            
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlAttachment
*******************/
CControlAttachment::CControlAttachment(){
    Name = "Control Attachment Node";
    m_textbox_count = 8;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 1.5f + 0.5f * Length.y);
    Magnet(MAGNETLEFT);
}

void CControlAttachment::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Attachment Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
    }
}

void CControlAttachment::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Attachment Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Attachment";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 1){
            m_pTextboxes[1]->m_text_content = "Renderpass:Shadowmap";
            m_pTextboxes[1]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 3){
            m_pTextboxes[3]->m_text_content = "Renderpass:MainScene";
            m_pTextboxes[3]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlAttachment::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);
        if(m_pApp->renderProcess.iShadowmapAttachmentDepthLight>=0) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Depth(Light)");

        if(m_pApp->renderProcess.iMainSceneAttachmentDepthLight>=0) m_pTextboxes[4]->bFlash = true;
        else m_pTextboxes[4]->SetTextColor(greyColor);
        m_pTextboxes[4]->SetTextContent("Depth(Light)");

        if(m_pApp->renderProcess.iMainSceneAttachmentDepthCamera>=0) m_pTextboxes[5]->bFlash = true;
        else m_pTextboxes[5]->SetTextColor(greyColor);
        m_pTextboxes[5]->SetTextContent("Depth(Camera)");

        if(m_pApp->renderProcess.iMainSceneAttachmentColorResovle>=0) m_pTextboxes[6]->bFlash = true;
        else m_pTextboxes[6]->SetTextColor(greyColor);
        m_pTextboxes[6]->SetTextContent("Color(Resolve)");

        if(m_pApp->renderProcess.iMainSceneAttachmentColorPresent>=0) m_pTextboxes[7]->bFlash = true;
        else m_pTextboxes[7]->SetTextColor(greyColor);
        m_pTextboxes[7]->SetTextContent("Color(Present)");

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        for(int i = 2; i <= 7; i++){
            if(i == 3) continue;
            if(m_pTextboxes[i]->bFlash) m_pTextboxes[i]->SetTextContent(); //to update the text color
        }
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlGraphicsUniform
*******************/
CControlGraphicsUniform::CControlGraphicsUniform(){
    Name = "Control Graphics Uniform Node";
    m_textbox_count = 8;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 1.0f + 0.5f * Length.y);
    Magnet(MAGNETRIGHT);
}

void CControlGraphicsUniform::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Graphics Uniform Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
    }
}

void CControlGraphicsUniform::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Graphics Uniform Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        //textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Graphics Uniform";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlGraphicsUniform::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_mvp) m_pTextboxes[1]->bFlash = true;
        else m_pTextboxes[1]->SetTextColor(greyColor);
        m_pTextboxes[1]->SetTextContent("MVP");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_text_mvp) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Text MVP");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_custom) m_pTextboxes[3]->bFlash = true;
        else m_pTextboxes[3]->SetTextColor(greyColor);
        m_pTextboxes[3]->SetTextContent("Custom");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_lighting) m_pTextboxes[4]->bFlash = true;
        else m_pTextboxes[4]->SetTextColor(greyColor);
        m_pTextboxes[4]->SetTextContent("Lighting");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_vp) m_pTextboxes[5]->bFlash = true;
        else m_pTextboxes[5]->SetTextColor(greyColor);
        m_pTextboxes[5]->SetTextContent("VP");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_depth_image_sampler) m_pTextboxes[6]->bFlash = true;
        else m_pTextboxes[6]->SetTextColor(greyColor);
        m_pTextboxes[6]->SetTextContent("Depth Img Sp");

        if(m_pApp->appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler) m_pTextboxes[7]->bFlash = true;
        else m_pTextboxes[7]->SetTextColor(greyColor);
        m_pTextboxes[7]->SetTextContent("Lightdepth Img Sp");

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        for(int i = 1; i <= 7; i++){
            if(m_pTextboxes[i]->bFlash) {
                m_pTextboxes[i]->SetTextContent(); //to update the text color
            }
        }
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlComputeUniform
*******************/
CControlComputeUniform::CControlComputeUniform(){
    Name = "Control Compute Uniform Node";
    m_textbox_count = 5;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 1.5f+ 0.5f * Length.y);
    Magnet(MAGNETRIGHT);
}

void CControlComputeUniform::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Compute Uniform Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
    }
}

void CControlComputeUniform::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Compute Uniform Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        //textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Compute Uniform";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlComputeUniform::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);
        //std::cout<<"ControlGraphicsUniform Update"<<std::endl;
        //std::string r = "F";
        if(m_pApp->appInfo.Uniform.b_uniform_compute_custom) m_pTextboxes[1]->bFlash = true;
        else m_pTextboxes[1]->SetTextColor(greyColor);
        m_pTextboxes[1]->SetTextContent("Custom");
        
        if(m_pApp->appInfo.Uniform.b_uniform_compute_storage) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Storage");

        if(m_pApp->appInfo.Uniform.b_uniform_compute_texture_storage) m_pTextboxes[3]->bFlash = true;
        else m_pTextboxes[3]->SetTextColor(greyColor);
        m_pTextboxes[3]->SetTextContent("Texture Storage");

        if(m_pApp->appInfo.Uniform.b_uniform_compute_swapchain_storage) m_pTextboxes[4]->bFlash = true;
        else m_pTextboxes[4]->SetTextColor(greyColor);
        m_pTextboxes[4]->SetTextContent("Swapchain Storage");

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        for(int i = 1; i <= 4; i++){
            if(m_pTextboxes[i]->bFlash) {
                m_pTextboxes[i]->SetTextContent(); //to update the text color
            }
        }
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlSubpass
*******************/
CControlSubpass::CControlSubpass(){
    Name = "Control Subpass Node";
    m_textbox_count = 7;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 1.0f + 0.5f * Length.y);
    Magnet(MAGNETLEFT);
}

void CControlSubpass::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Subpass Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
    }
}

void CControlSubpass::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Attachment Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Subpass";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 1){
            m_pTextboxes[1]->m_text_content = "Renderpass:Shadowmap";
            m_pTextboxes[1]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }else if(i == 3){
            m_pTextboxes[3]->m_text_content = "Renderpass:MainScene";
            m_pTextboxes[3]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlSubpass::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);
        if(m_pApp->renderProcess.bEnableShadowmapRenderpassSubpassShadowmap) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Shadowmap");

        if(m_pApp->renderProcess.bEnableMainSceneRenderpassSubpassShadowmap) m_pTextboxes[4]->bFlash = true;
        else m_pTextboxes[4]->SetTextColor(greyColor);
        m_pTextboxes[4]->SetTextContent("Shadowmap");

        if(m_pApp->renderProcess.bEnableMainSceneRenderpassSubpassDraw) m_pTextboxes[5]->bFlash = true;
        else m_pTextboxes[5]->SetTextColor(greyColor);
        m_pTextboxes[5]->SetTextContent("Draw");

        if(m_pApp->renderProcess.bEnableMainSceneRenderpassSubpassObserve) m_pTextboxes[6]->bFlash = true;
        else m_pTextboxes[6]->SetTextColor(greyColor);
        m_pTextboxes[6]->SetTextContent("Observe");

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        for(int i = 2; i <= 6; i++){
            if(i == 3) continue;
            if(m_pTextboxes[i]->bFlash) m_pTextboxes[i]->SetTextContent(); //to update the text color
        }
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlHotkey
*******************/
CControlHotkey::CControlHotkey(){
    //std::cout<<"Creating Control Hotkey Node"<<std::endl;
    Name = "Control Hotkey Node";
    m_textbox_count = 3;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5f * Length.y);
    Magnet(MAGNETLEFT);
}

void CControlHotkey::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Hotkey Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
        //no need to set position/scale, because all object follows control node's model matrix
    }


}

void CControlHotkey::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Hotkey Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0); //todo: adjust position based on control node scale
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Hotkey";  //->SetTextContent("PerfMetric");
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlHotkey::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);

        for(int i = 1; i <= 2; i++) m_pTextboxes[i]->bFlash = true;

        m_pTextboxes[1]->SetTextContent("Toggle Perf Panel:P");
        m_pTextboxes[2]->SetTextContent("Toggle All Panels:H");

        // if(m_pApp->objects.size()>0) m_pTextboxes[5]->bFlash = true;
        // else m_pTextboxes[5]->SetTextColor(greyColor);
        // m_pTextboxes[5]->SetTextContent("Object:" + std::to_string(m_pApp->objects.size()));

        // if(m_pApp->textManager.m_textBoxes.size()>0) m_pTextboxes[6]->bFlash = true;
        // else m_pTextboxes[6]->SetTextColor(greyColor);
        // m_pTextboxes[6]->SetTextContent("Textbox:" + std::to_string(m_pApp->textManager.m_textBoxes.size()));

        // if(m_pApp->lights.size()>0) m_pTextboxes[7]->bFlash = true;
        // else m_pTextboxes[7]->SetTextColor(greyColor);
        // m_pTextboxes[7]->SetTextContent("Light:" + std::to_string(m_pApp->lights.size()));

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        // m_pTextboxes[1]->SetTextContent("FPS:" + std::to_string((int)(1.0f/m_pApp->deltaTime)));
        // m_pTextboxes[2]->SetTextContent("Time:" + to_string_prec(m_pApp->elapseTime) + "s");
        // m_pTextboxes[3]->SetTextContent("Frame:" + std::to_string(m_pApp->frameCount));
        for(int i = 1; i <= 2; i++) if(m_pTextboxes[i]->bFlash) m_pTextboxes[i]->SetTextContent(); //to update the text color
            
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlFeature
*******************/
CControlFeature::CControlFeature(){
    Name = "Control Feature Node";
    m_textbox_count = 8;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5f + 0.5f * Length.y);
    Magnet(MAGNETLEFT);
}

void CControlFeature::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Feature Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
    }
}

void CControlFeature::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Feature Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        //textbox->SetTextColor(glm::vec4(0.2, 0, 0, 1));
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0);
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Feature";
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlFeature::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);
        //std::cout<<"ControlGraphicsUniform Update"<<std::endl;
        //std::string r = "F";
        if(m_pApp->appInfo.Feature.b_feature_graphics_48pbt) m_pTextboxes[1]->bFlash = true;
        else m_pTextboxes[1]->SetTextColor(greyColor);
        m_pTextboxes[1]->SetTextContent("48bpt");

        if(m_pApp->appInfo.Feature.b_feature_graphics_push_constant) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Push Constant");
        
        if(m_pApp->appInfo.Feature.b_feature_graphics_blend) m_pTextboxes[3]->bFlash = true;
        else m_pTextboxes[3]->SetTextColor(greyColor);
        m_pTextboxes[3]->SetTextContent("Blend");

        if(m_pApp->appInfo.Feature.b_feature_graphics_rainbow_mipmap) m_pTextboxes[4]->bFlash = true;
        else m_pTextboxes[4]->SetTextColor(greyColor);
        m_pTextboxes[4]->SetTextContent("Rainbow Mipmap");

        if(m_pApp->appInfo.Feature.feature_graphics_pipeline_skybox_id != -1) m_pTextboxes[5]->bFlash = true;
        else m_pTextboxes[5]->SetTextColor(greyColor);
        m_pTextboxes[5]->SetTextContent("Skybox");

        if(m_pApp->appInfo.Feature.feature_graphics_observe_attachment_id != -1) m_pTextboxes[6]->bFlash = true;
        else m_pTextboxes[6]->SetTextColor(greyColor);
        m_pTextboxes[6]->SetTextContent("Observe Attachment");

        if(m_pApp->appInfo.Feature.feature_graphics_enable_controls) m_pTextboxes[7]->bFlash = true;
        else m_pTextboxes[7]->SetTextColor(greyColor);
        m_pTextboxes[7]->SetTextContent("Performance Metrics");

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        for(int i = 1; i <= 7; i++){
            if(m_pTextboxes[i]->bFlash) {
                m_pTextboxes[i]->SetTextContent(); //to update the text color
            }
        }
        lastTrigger = currentTrigger;
	}
}

/******************
* ControlStatistics
*******************/
CControlStatistics::CControlStatistics(){
    //std::cout<<"Creating Control Statistics Node"<<std::endl;
    Name = "Control Statistics Node";
    m_textbox_count = 4;
    m_object_count = 1;
    SetScale(0.5, 0.5, 0);
    SetPositionY(-1.0f + 0.5f + 0.5f * Length.y);
    Magnet(MAGNETRIGHT);
}

void CControlStatistics::RegisterObject(int startIndex){
    for(int i = 0; i < m_object_count; i++){
        int index = startIndex+i;
        CObject *object = &(m_pApp->objects[index]);
        m_pObjects.push_back(object);

        object->Name = "Statistics Background";
        object->bSticker = true;
        object->SetPosition(0,0,0);
        object->p_controlNode = this;
        object->m_object_id = index;
        object->m_texture_ids = m_pApp->appInfo.ControlUIContainer.resource_texture_id_list_box;
        object->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_box;
        object->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_box;

        object->Register(m_pApp);
        //no need to set position/scale, because all object follows control node's model matrix
    }


}

void CControlStatistics::RegisterTextbox(int startIndex){
    for(int i = 0; i < m_textbox_count; i++){
        int index = startIndex+i;
        CTextbox *textbox = &(m_pApp->textManager.m_textBoxes[index]);
        m_pTextboxes.push_back(textbox);

        textbox->Name = "Statistics Background Textbox: " + std::to_string(i);
        textbox->bSticker = true;
        textbox->SetScale(0.2f);
        textbox->SetTextColor(glm::vec4(0.75, 0.75, 0.75, 1));
        textbox->SetPosition(0.06-Length.x/2, -0.2+0.05*i, 0); //todo: adjust position based on control node scale
        textbox->p_controlNode = this;
        textbox->m_textBoxID = index;
        textbox->m_text_content = "text_content";
        textbox->m_model_id = m_pApp->appInfo.ControlUIContainer.resource_model_id_text;
        textbox->m_default_graphics_pipeline_id = m_pApp->appInfo.ControlUIContainer.resource_default_graphics_pipeline_id_text;
        
        if(i == 0){
            m_pTextboxes[0]->m_text_content = "Statistics";  //->SetTextContent("PerfMetric");
            m_pTextboxes[0]->SetTextColor(glm::vec4(0.2, 0.7, 1.0, 1)); //title color
        }

        textbox->Register(m_pApp);
    }
}

void CControlStatistics::Update(){
    CEntity::Update(m_pApp->deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix

    static int lastTrigger = 0;
	int currentTrigger = (double)m_pApp->elapseTime / 0.1f;

    static bool firstTime = true;

    if(firstTime){
        glm::vec4 greyColor = glm::vec4(0.25, 0.25, 0.25, 1);

        //for(int i = 1; i <= 4; i++) m_pTextboxes[i]->bFlash = true;

       // m_pTextboxes[4]->SetTextContent("Init:" + to_string_prec(m_pApp->totalInitTime) + " ms");

        if(m_pApp->objects.size()>0) m_pTextboxes[1]->bFlash = true;
        else m_pTextboxes[1]->SetTextColor(greyColor);
        m_pTextboxes[1]->SetTextContent("Object:" + std::to_string(m_pApp->objects.size()));

        if(m_pApp->textManager.m_textBoxes.size()>0) m_pTextboxes[2]->bFlash = true;
        else m_pTextboxes[2]->SetTextColor(greyColor);
        m_pTextboxes[2]->SetTextContent("Textbox:" + std::to_string(m_pApp->textManager.m_textBoxes.size()));

        if(m_pApp->lights.size()>0) m_pTextboxes[3]->bFlash = true;
        else m_pTextboxes[3]->SetTextColor(greyColor);
        m_pTextboxes[3]->SetTextContent("Light:" + std::to_string(m_pApp->lights.size()));

        firstTime = false;
    }

	if(currentTrigger!=lastTrigger) {
        // m_pTextboxes[1]->SetTextContent("FPS:" + std::to_string((int)(1.0f/m_pApp->deltaTime)));
        // m_pTextboxes[2]->SetTextContent("Time:" + to_string_prec(m_pApp->elapseTime) + "s");
        // m_pTextboxes[3]->SetTextContent("Frame:" + std::to_string(m_pApp->frameCount));
        for(int i = 1; i <= 3; i++) if(m_pTextboxes[i]->bFlash) m_pTextboxes[i]->SetTextContent(); //to update the text color
            
        lastTrigger = currentTrigger;
	}
}
