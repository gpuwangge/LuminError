#ifndef H_TEXTMANAGER
#define H_TEXTMANAGER

#include "renderProcess.h"
#include "camera.hpp"

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;
class CRenderer;
class CTextImageManager;
class CModelManager;
class CControlNode;

/******************
* TextBox
*******************/
class CTextManager;

class CTextbox : public CEntity {
    CRenderer *p_renderer;
    CRenderProcess *p_renderProcess;
    std::vector<VkDescriptorSet> *p_descriptorSets_graphics_general;
    std::vector<VkDescriptorSet> descriptorSets_graphics_texture_image_sampler;
    CTextImageManager *p_textImageManager;

    std::vector<int> m_highlightedIndex = std::vector<int>(10, -1);
    //int m_highlightedIndex = -1;
    bool b_reverseHighlight = false;
public:
    std::string m_text_content = "";
    std::vector<TextInstanceData> instanceData;
    CWxjBuffer instanceDataBuffer;

    CTextManager *p_textManager;

    CControlNode *p_controlNode = NULL;

    int m_default_graphics_pipeline_id = 0;
    int m_model_id = 0;
    int m_textBoxID = 0;
    
    bool bSticker = false; //if true, the text box will not be affected by camera view and projection matrix

    int m_maxCharPerRow = 25;
    int m_maxCharperTextbox = 200;
    int m_currentCharCount = 0;

    bool bRegistered = false;
    bool bInitialized = false;
    bool bFlash = false;
    glm::vec4 m_boxColor = glm::vec4(255.0f);
    glm::vec4 m_textColor = glm::vec4(0.0f);
    //std::vector<CCharacter> m_characters;
    void CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews = NULL);

    CTextbox(){}
    //~CTextbox(){}
    void Destroy(){instanceDataBuffer.DestroyAndFree();}

    //void SetText(std::string text_content){m_text_content = text_content;}
    void SetBoxColor(glm::vec4 color){m_boxColor = color;}
    void SetTextColor(glm::vec4 color){m_textColor = color;}
    void SetTextContent(std::string text_content = "");
    //void SetHighlightedChar(int index){m_highlightedIndex = index;}
    void AdvanceHighlightedChar();
    void Register(CApplication *p_app);
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);
    void Draw();
};

/******************
* TextManager
*******************/

class CTextManager {
    int m_samplerID = 0;
    glm::vec4 m_outlinecolor = glm::vec4(255.0f);
    glm::vec4 m_textcolor = glm::vec4(0.0f);
    
public:
    std::vector<CTextbox> m_textBoxes;

    CRenderer *p_renderer = NULL;
    CTextImageManager *p_textImageManager = NULL;
    CModelManager *p_modelManager = NULL;

    const std::string ascII = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    // Map to store glyph texture data for a font
    std::map<char, GlyphTexture> glyphMap;

    CTextManager();
    //~CTextManager();
    void Destroy(){
        for(int i = 0; i < m_textBoxes.size(); i++) m_textBoxes[i].Destroy();
    }

    void SetFontSize(int fontSize){m_fontSize = fontSize;}
    void SetSamplerID(int samplerID){m_samplerID = samplerID;}
    void SetOutlineColor(glm::vec4 color){m_outlinecolor = color;}
    void SetTextColor(glm::vec4 color){m_textcolor = color;}

    //int GetInstanceCount() const { return m_instanceCount; }
    std::vector<TextQuadVertex> textQuadVertices;
    std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

    TTF_Font* m_font = nullptr;
    int m_fontSize = 20;
    void CreateTextFonts();

    void CreateTextImage();
    void CreateTextResource();

    void CreateGlyphMap();

    //void Initialize();
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);
    void Draw(){
        //m_textBoxes[0].Draw();
        for(auto& textBox : m_textBoxes){textBox.Draw();}
    };

};

#endif