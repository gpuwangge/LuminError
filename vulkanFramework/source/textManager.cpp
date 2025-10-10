#include "../include/textManager.h"
#include "../include/application.h"

/*************
* Change from CCharacter(no use) to CTextBox
**************/

void CTextbox::CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
    //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;
    if(samplers.size() < 1) return;
    
    int descriptorSize = samplers.size();//getDescriptorSize();
    //std::cout<<"createTextureDescriptorSets::samplers.size(): "<<samplers.size()<<std::endl;
    //std::cout<<"Object Set(Sampler) size = "<<descriptorSize<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_graphics_texture_image_sampler.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets_graphics_texture_image_sampler.data());
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        std::vector<VkWriteDescriptorSet> descriptorWrites;

        descriptorWrites.resize(descriptorSize);
        //int counter = 0;

        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform

        std::vector<VkDescriptorImageInfo> imageInfo{}; //for texture sampler
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT){
        imageInfo.resize(samplers.size());
        for(int j = 0; j < samplers.size(); j++){
            //std::cout<<"CreateTextureDescriptorSets::samplers:"<<j<<std::endl;
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            //imageInfo[j].imageView = textureImages[j].m_textureImageBuffer.view;
            //imageInfo[j].sampler = samplers[j];

            //if(b_isText){
                //if(j < m_text_ids.size()){
                    imageInfo[j].imageView = p_textImageManager->textureImages[0].m_textureImageBuffer.view; //TODO: now we have only one text image(ascII), maybe add more later
                    imageInfo[j].sampler = samplers[p_textImageManager->textureImages[0].m_sampler_id]; 
                //}else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
                //    imageInfo[j].imageView = p_textImageManager->textureImages[m_text_ids[0]].m_textureImageBuffer.view;
                //    imageInfo[j].sampler = samplers[p_textImageManager->textureImages[m_text_ids[0]].m_sampler_id]; 
                //}
            // }else{
            //     if(j < m_texture_ids.size()){
            //         imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            //     }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //         imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            //     }
            // }


            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptorSets_graphics_texture_image_sampler[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pImageInfo = &imageInfo[j];
            //counter++;
        }
        //}

        //Step 4
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    //std::cout<<"Done set descriptor. "<<std::endl;
}

void CTextbox::Draw(){
    if(!bRegistered) return;
    if(p_controlNode != NULL && !p_controlNode->bVisible) return;

    //std::cout<<"Drawing TextBox ID: "<<m_textBoxID<<", text: "<<m_text_content<<std::endl;
    VkPipelineLayout *p_graphicsPipelineLayout = &(p_renderProcess->graphicsPipelineLayouts[m_default_graphics_pipeline_id]);
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[m_default_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //std::cout<<"TextBox ID: "<<m_textBoxID<<", instanceCount: "<<m_instanceCount<<std::endl;

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    if(CGraphicsDescriptorManager::getSetSize_General() > 0) dsSets.push_back(*p_descriptorSets_graphics_general); 
    if(CGraphicsDescriptorManager::textureImageSamplers.size() > 0) dsSets.push_back(descriptorSets_graphics_texture_image_sampler); 

    //std::cout<<"TextBox ID: "<<m_textBoxID<<", descriptorSets size: "<<dsSets.size()<<std::endl;

    if(dsSets.size() > 0){
        int dynamicTextboxMVPOffset = m_textBoxID; //use offset for textboxID=0
        p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, 0, dynamicTextboxMVPOffset);
    }

  

    //std::cout<<"TextBox ID: "<<m_textBoxID<<", binding vertex and index buffer."<<"modelID: "<<m_model_id<<std::endl;
    //std::cout<<"vertex size:"<<p_renderer->vertexDataBuffers.size()<<" instance size: "<<p_renderer->instanceDataBuffers.size()<<std::endl;
    //p_renderer->BindVertexInstanceBuffer(m_model_id, m_textBoxID);
    p_renderer->BindVertexInstanceBuffer(m_model_id, *this);
      //hack
    //if(m_textBoxID != 0) return;

    //std::cout<<"TextBox ID: "<<m_textBoxID<<", binding index buffer and drawing."<<std::endl;
    p_renderer->BindIndexBuffer(m_model_id);
    //std::cout<<"TextBox ID: "<<m_textBoxID<<", drawing indexed."<<std::endl;
    //p_renderer->DrawInstanceIndexed(m_model_id, instanceData.size());
    p_renderer->DrawInstanceIndexed(m_model_id, m_currentCharCount);
    //std::cout<<"TextBox ID: "<<m_textBoxID<<" drawn."<<std::endl;
}


/******************
* TextBox
*******************/
//CTextBox::CTextBox(){}
void CTextbox::Register(CApplication *p_app){
    bRegistered = true;
    //m_textBoxID = textbox_id;
    //m_text_content = text_content;

    //m_characters.resize(1);

    //m_model_id = model_id;
    //m_default_graphics_pipeline_id = default_graphics_pipeline_id;
    //m_instanceCount = p_app->textManager.GetInstanceCount();

    //for(auto& ch : m_characters){
    //ch.SetInstanceCount(m_instanceCount);
    p_renderer = &(p_app->renderer);
    p_renderProcess = &(p_app->renderProcess);
    p_descriptorSets_graphics_general = &(p_app->graphicsDescriptorManager.descriptorSets_general);
    //ch.descriptorSets_graphics_texture_image_sampler;
    p_textImageManager = &(p_app->textImageManager);
    //p_textManager = &(p_app->textManager); //set this outside register function

    CreateDescriptorSets_TextureImageSampler(
        CGraphicsDescriptorManager::graphicsDescriptorPool,
        CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler,
        CGraphicsDescriptorManager::textureImageSamplers
    );
    //}
    //std::cout<<"TextBox ID: "<<m_textBoxID<<" registered."<<std::endl;
    //CreateTextInstanceData(&(p_app->textManager));
    //std::cout<<"TextBox ID: "<<m_textBoxID<<" instance data created."<<std::endl;
    //p_app->modelManager.CreateTextModel( p_app->textManager.textQuadVertices, textInstanceData, p_app->textManager.indices3D);
    //std::cout<<"TextBox ID: "<<m_textBoxID<<" model created."<<std::endl;

    SetTextContent(m_text_content);
}

void CTextbox::AdvanceHighlightedChar(){
    // m_highlightedIndex++;
    // if (m_highlightedIndex >= m_maxCharPerRow) {
    //     m_highlightedIndex = 0;
    // }

    //no reverse
    for(int i = m_highlightedIndex.size()-1; i > 0; i--)
        m_highlightedIndex[i] = m_highlightedIndex[i-1];

    m_highlightedIndex[0]++;
    if(m_highlightedIndex[0] >= m_maxCharPerRow) m_highlightedIndex[0] = 0;


    //reverse version
    /*
    for(int i = m_highlightedIndex.size()-1; i > 0; i--)
        m_highlightedIndex[i] = m_highlightedIndex[i-1];
    if(!b_reverseHighlight){
        m_highlightedIndex[0]++;
        //if(m_highlightedIndex[0] >= (m_currentCharCount-1)) b_reverseHighlight = true;
        if(m_highlightedIndex[0] >= m_maxCharPerRow) b_reverseHighlight = true;
        
    }else{
        m_highlightedIndex[0]--;
        if(m_highlightedIndex[0] <= 0) b_reverseHighlight = false;
    }*/
}

void CTextbox::SetTextContent(std::string text_content){
    //std::string text = "Hello123abcdABCD";
    float penX = 0.0f;
    float penY = 0.0f;

    //glm::vec3 color = m_textColor;
    float sx = 2.0f / WINDOW_WIDTH; // scale to NDC
    float sy = 2.0f / WINDOW_HEIGHT;

    if(!bInitialized) instanceData.resize(m_maxCharperTextbox);
        //m_text_content = p_textManager->ascII;//"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    
    if(text_content.size() != 0) m_text_content = text_content;
    m_currentCharCount = m_text_content.size();

    //float sum_scale_x = 0;

    //std::cout<<"Creating text instance data for text: "<<m_text_content<<std::endl;
    int rowCharCount = 0;
    for (int i = 0; i < m_text_content.size(); i++) {
        char ch = m_text_content[i];
        GlyphTexture &glyph = p_textManager->glyphMap[ch];

        glm::vec2 posOffset(penX * sx, penY * sy);
        glm::vec2 scale(glyph.size.x/glyph.size.y, 1.0f); //keep the height to be 1.0f*quad.heigth(0.15) in NDC
        //glm::vec2 scale(glyph.size * glm::vec2(sx, sy));

        //sum_scale_x += scale.x;

        //std::cout<<"TextBox ID: "<<m_textBoxID<<", char: "<<ch<<", posOffset: "<<posOffset.x<<","<<posOffset.y<<", scale: "<<scale.x<<","<<scale.y<<", glyph.advance: "<<glyph.advance<<std::endl;
        //std::cout<<"TextBox ID: "<<m_textBoxID<<", char: "<<ch<<", posOffset: "<<posOffset.x<<","<<posOffset.y<<", glyph.size: "<<glyph.size.x<<","<<glyph.size.y<<", scale: "<<scale.x<<","<<scale.y<<", glyph.advance: "<<glyph.advance<<std::endl;
        instanceData[i].offset = posOffset;

        if(bFlash){
            // std::cout<<"TextBox ID: "<<m_textBoxID<<", char: "<<ch<<", highlightedIndex: "<<m_highlightedIndex<<std::endl;
            // if(i == m_highlightedIndex){
            //     instanceData[i].color = glm::vec4(0.2f, 0.7f, 1.0f, 1.0f);
            // }
            // else{
            //     instanceData[i].color = m_textColor;
            // }

            float colorCoff = 0;
            for(int j = 0; j < m_highlightedIndex.size(); j++) {
                if(i == m_highlightedIndex[j]){
                    colorCoff = 1.0f - 0.07f * j;
                    break;
                }
            }
            //if(colorCoff == 0) instanceData[i].color = m_textColor;
            //else 
            instanceData[i].color = glm::vec4(1-colorCoff*0.8, 1-colorCoff*0.3, 1, 1);//glm::vec4(colorCoff, 0, 0, 1);
            
        }
        else instanceData[i].color = m_textColor;

        instanceData[i].uvRect = glyph.uvRect;
        instanceData[i].scale = scale;
        
        penX += glyph.advance;
        rowCharCount++;
        if (rowCharCount >= m_maxCharPerRow) { //move to next line
            penX = 0;
            penY -= glyph.size.y;
            rowCharCount = 0;
        }

        float margin = 1.3f;
        float current_textwidth= penX * sx * Scale.x * margin;
        if(p_controlNode && current_textwidth > p_controlNode->Scale.x){
            break;
        }
    }
    if(bFlash) AdvanceHighlightedChar();
     

    // float textline_width = 0.1 * sum_scale_x; //0.1 is quad height in NDC
    // if(p_controlNode != NULL){
    //     if(textline_width > p_controlNode->Scale.x){
    //         //std::cout<<"TextBox ID: "<<m_textBoxID<<", text line width: "<<textline_width<<", larger than control node scale x: "<<p_controlNode->Scale.x<<", ";
    //         //std::cout<<"text content: \""<<m_text_content<<"\", char count: "<<m_currentCharCount<<std::endl; 
    //         //p_controlNode->Scale.x = textline_width;
    //         if(p_controlNode->Scale.x < penX * sx * Scale.x * 1.2){ //1.2 is a little margin
    //             p_controlNode->Scale.x = penX * sx * Scale.x * 1.2;
    //             std::cout<<"TextBox ID: "<<m_textBoxID<<", text line width: "<<textline_width<<", larger than control node scale x: "<<p_controlNode->Scale.x<<std::endl;
    //         }
    //     }
    // }
    
    // if(p_controlNode && m_textBoxID == 23){
    //     float textWidth = penX * sx;
    //     std::cout<<"TextBox ID: "<<m_textBoxID<<", text content: \""<<m_text_content<<"\", char count: "<<m_currentCharCount<<std::endl;
    //     std::cout<<"    ControlNode Name: \""<<p_controlNode->Name<<"\", Length: "<<p_controlNode->Length.x<<","<<p_controlNode->Length.y<<","<<p_controlNode->Length.z<<std::endl;
    //     std::cout<<"    scale sum x: "<<sum_scale_x<<", calculated textbox width: "<< 0.1 * sum_scale_x<<std::endl;
    //     std::cout<<"    "<<p_controlNode->m_pObjects[0]->Name<<", length: "<<p_controlNode->m_pObjects[0]->Length.x<<","<<p_controlNode->m_pObjects[0]->Length.y<<","<<p_controlNode->m_pObjects[0]->Length.z<<std::endl;
    //     std::cout<<"    penX: "<<penX<<", sx: "<<sx<<", textWidth: "<<textWidth<<std::endl;
    // }


    if(!bInitialized) {
        VkDeviceSize bufferSize = sizeof(instanceData[0]) * instanceData.size();
        VkResult result = instanceDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }

    instanceDataBuffer.fill((void *)(instanceData.data()));
    
    bInitialized = true;
}

void CTextbox::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    if(!bRegistered) return;
    if(p_controlNode != NULL && !p_controlNode->bVisible) return;

    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
    /********************************
    * Calculate model matrix based on Translation, Rotation and Scale
    ********************************/
   if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP){
        if(p_controlNode == NULL)  CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].model = ModelMatrix;
        else CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].model = p_controlNode->TransRotation * ModelMatrix; //textbox follow control node translation and rotation, but not scale

        //std::cout<< "TextBox ID: " << m_textBoxID << " Model Matrix: " << glm::to_string(CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].model) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TranslateMatrix: " << glm::to_string(TranslateMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " RotateMatrix: " << glm::to_string(RotationMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " ScaleMatrix: " << glm::to_string(ScaleMatrix) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " Velocity: " << glm::to_string(Velocity) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " AngularVelocity: " << glm::to_string(AngularVelocity) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempVelocity: " << glm::to_string(TempVelocity[0]) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempAngularVelocity: " << glm::to_string(TempAngularVelocity[0]) << std::endl;
        // std::cout<< "TextBox ID: " << m_textBoxID << " TempMoveAngularVelocity: " << glm::to_string(TempMoveAngularVelocity) << std::endl;
        //std::cout<< "Delta Time: " << deltaTime << std::endl;

        if(!bSticker){
            CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraProj = mainCamera.matrices.projection;
            CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraView = mainCamera.matrices.view;
        }else{
            CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraProj = glm::mat4(1.0f);
            CGraphicsDescriptorManager::textMVPUBO.mvpData[m_textBoxID].mainCameraView = glm::mat4(1.0f);
        }

        memcpy(CGraphicsDescriptorManager::textMVPUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::textMVPUBO, sizeof(CGraphicsDescriptorManager::textMVPUBO));
   }
    //for(auto& ch : m_characters){ 
        //ch.Update();
    //}
}

/******************
* TextManager
*******************/
CTextManager::CTextManager(){
}

void CTextManager::CreateTextFonts(){
    if (TTF_Init() == -1) std::cout << "SDL_ttf could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    m_font = TTF_OpenFont("NotoSansCJK-VF.otf.ttc", m_fontSize);
    //m_font = TTF_OpenFont("arial.ttf", m_fontSize);
    if (!m_font) std::cout << "Failed to load font! SDL_Error: " << SDL_GetError() << std::endl;

    TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);
}

void CTextManager::CreateTextImage(){
    //Step 1: Create Text Resources
    CreateTextFonts();

    SDL_Color outlineColor = { 
        static_cast<Uint8>(m_outlinecolor.r),
        static_cast<Uint8>(m_outlinecolor.g),
        static_cast<Uint8>(m_outlinecolor.b),
        static_cast<Uint8>(m_outlinecolor.a)
    };
    SDL_Color textColor = { 
        static_cast<Uint8>(m_textcolor.r),
        static_cast<Uint8>(m_textcolor.g),
        static_cast<Uint8>(m_textcolor.b),
        static_cast<Uint8>(m_textcolor.a)
    };

    TTF_SetFontOutline(m_font, 0);
    SDL_Surface* outlineSurface = TTF_RenderText_Blended(m_font, ascII.c_str(), 0, outlineColor);
    TTF_SetFontOutline(m_font, 0);
    SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, ascII.c_str(), 0, textColor);
    SDL_Rect dst;
    dst.x = (outlineSurface->w - textSurface->w) / 2;
    dst.y = (outlineSurface->h - textSurface->h) / 2;
    /* Disable outline blit for now
    SDL_BlitSurface(
        textSurface, //src
        NULL, //srcRect: NULL = whole surface
        outlineSurface, //dst
        &dst //destRect: copy to the center
    );
    */

    //SDL_Surface* textSurface = TTF_RenderText_Shaded(m_font, ascII.c_str(), 0, sdlColor, sdlColor);

    SDL_Surface* conv = SDL_ConvertSurface(outlineSurface, SDL_PIXELFORMAT_RGBA32);
    if (!conv) {
        std::cerr << "SDL_ConvertSurfaceFormat failed: " << SDL_GetError() << std::endl;
    }
    
    int width  = conv->w;
    int height = conv->h;
    void* pixels = conv->pixels; //this saves the pixel data of the rendered text
    int pitch = conv->pitch;

    //std::cout << "Text Surface '" << 123 << "' Width: " << width << ", Height: " << height << ", Pitch: " << pitch << std::endl;

    CTextureImage textureImage;
    textureImage.m_imageFormat = VK_FORMAT_R8G8B8A8_UNORM;//VK_FORMAT_R8G8B8A8_SRGB;
    textureImage.m_mipLevels = 1;
    textureImage.m_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    textureImage.m_pCommandPool = &(p_renderer->commandPool);

    //textureImage.GetTexels(texturePath);
    textureImage.m_pTexels = pixels;
    textureImage.m_texWidth = width;
    textureImage.m_texHeight = height;
    textureImage.m_texChannels = 4;// STBI_rgb_alpha, RGBA
    textureImage.m_texBptpc = 8;

    textureImage.CreateTextureImage(false); //false: not use STBI to free pixels
    SDL_DestroySurface(conv);
    SDL_DestroySurface(textSurface);
    textureImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

    textureImage.m_sampler_id = m_samplerID;

    //std::cout<<"Text Image Created: "<<std::endl;

    p_textImageManager->textureImages.push_back(textureImage);
}


void CTextManager::CreateGlyphMap() {
    //std::string asciiChars;
    //for (int i = 32; i < 127; i++) asciiChars.push_back((char)i);

    SDL_Surface* glyphSurface;
    int penX = 0, penY = 0;
    int maxRowHeight = 0;
    int atlasWidth = 0, atlasHeight = 0;

    int charCount = 0;
    for (char ch : ascII) {
        glyphSurface = TTF_RenderGlyph_Blended(m_font, ch, {255, 255, 255, 255});
        if (!glyphSurface) continue;

        int minx, maxx, miny, maxy, advance;
        TTF_GetGlyphMetrics(m_font, ch, &minx, &maxx, &miny, &maxy, &advance);

        atlasWidth += advance;
        
        if (glyphSurface->h > maxRowHeight) maxRowHeight = glyphSurface->h;

        SDL_DestroySurface(glyphSurface);

        charCount++;
    }
    atlasHeight = maxRowHeight;
    float averageAdvance = static_cast<float>(atlasWidth) / charCount;
    //std::cout<<"averageAdvance: "<<averageAdvance<<std::endl;

    //std::cout << "Creating Glyph Map with atlas size: " << atlasWidth << "x" << atlasHeight << std::endl;

    penX = 0;
    for (char ch : ascII) {
        glyphSurface = TTF_RenderGlyph_Blended(m_font, ch, {255, 255, 255, 255});
        if (!glyphSurface) continue;

        int minx, maxx, miny, maxy, advance;
        if (TTF_GetGlyphMetrics(m_font, ch, &minx, &maxx, &miny, &maxy, &advance) == 0) {
            std::cout << "Failed to get glyph metrics for '" << ch << "'" << std::endl;
            SDL_DestroySurface(glyphSurface);
            continue;
        }

        // std::cout << "Char '" << ch << "' Metrics: minx=" << minx << ", maxx=" << maxx 
        //           << ", miny=" << miny << ", maxy=" << maxy << ", advance=" << advance 
        //           << ", glyphSurface->w=" << glyphSurface->w << ", glyphSurface->h=" << glyphSurface->h << ", ";
        
        GlyphTexture glyph;
        //glyph.pos = { penX + minx + 1, penY};
        glyph.size = { maxx - minx + 1, glyphSurface->h };
        glm::vec4 texelRect = {
            penX + minx + 1,
            penY + glyphSurface->h,
            std::min(maxx - minx + 1, advance),
            penY
        };
        glyph.uvRect = {
            texelRect.x / atlasWidth,
            1.0f - texelRect.y / atlasHeight,
            texelRect.z / atlasWidth,
            1.0f - texelRect.w / atlasHeight
        };
        glyph.advance = std::max(advance, (int)(averageAdvance*1.3f));

        glyphMap[ch] = glyph;

        //std::cout << "advance: " << advance << " size: " << glyph.size.x << ", " << glyph.size.y << std::endl;
        penX += advance;
        //std::cout << "Next penX: " << penX << std::endl;
        SDL_DestroySurface(glyphSurface);
    }

    //std::cout << "Atlas size: " << atlasWidth << "x" << atlasHeight << std::endl;
}

/*There are two resources: quad pos/uv and instance offset/color*/
void CTextManager::CreateTextResource(){
    //int w = 51; //font character size
    //int h = 73;
    //float aspect = (float)w / (float)h; // 51/73 ≈ 0.6986
    //float half_h = 0.15f;         //expected quad height
    //float half_w = half_h * aspect; // width scaled proportionally 0.6986*0.15f ≈ 0.10479
    float half_h = 0.15f;
    float half_w = 0.1f;

    float x_min = -half_w;  //quad size
    float x_max =  half_w; 
    float y_min = -half_h; 
    float y_max =  half_h; 

    textQuadVertices.push_back({ {x_min, y_min}, {0.0f, 0.0f} }); // left bottom
    textQuadVertices.push_back({ {x_max, y_min}, {1.0f, 0.0f} }); // right bottom
    textQuadVertices.push_back({ {x_max, y_max}, {1.0f, 1.0f} }); // right up
    textQuadVertices.push_back({ {x_min, y_max}, {0.0f, 1.0f} }); // left up

    p_modelManager->CreateTextQuadModel(textQuadVertices, indices3D);


    std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

    p_modelManager->CreateCustomModel3D(vertices3D, indices3D, true);

}

void CTextManager::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    for (int i = 0; i < m_textBoxes.size(); i++) {
        m_textBoxes[i].Update(deltaTime, currentFrame, mainCamera);
    }
}



