#ifndef H_MODELMANAGER
#define H_MODELMANAGER

#include "common.h"
#include "dataBuffer.hpp"

 #ifdef ANDROID
#include "context.h"
 #endif

class CModelManager final{
public:
    CModelManager();
    ~CModelManager();

    /****Custom Model 3D*****/
    class CCustomModel3D{
    public:
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;
        glm::vec3 length;
        glm::vec3 lengthMax;
        glm::vec3 lengthMin;
    };
    std::vector<CCustomModel3D> customModels3D; //use custom 3d id
    void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage = false);


    /****Custom Model 2D*****/
    class CCustomModel2D{
    public:
        std::vector<Vertex2D> vertices;
        std::vector<uint32_t> indices;
        glm::vec3 length;
        glm::vec3 lengthMax;
        glm::vec3 lengthMin;
    };
    std::vector<CCustomModel2D> customModels2D; //use custom 2d id
    void CreateCustomModel2D(std::vector<Vertex2D> &vertices2D);


    /****Text Quad Model*****/
    class CTextQuadModel{
    public:
        std::vector<TextQuadVertex> vertices;
        //std::vector<TextInstanceData> instanceData;
        std::vector<uint32_t> indices;
    };
    std::vector<CTextQuadModel> textQuadModels;
    void CreateTextQuadModel(std::vector<TextQuadVertex> &vertices, std::vector<uint32_t> &indices);


    /****Textbox Image Model*****/
    std::vector<CCustomModel3D> textboxImageModels; //reuse CCustomModel3D
    //void CreateTextboxImageModel(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D); //reuse CreateCustomModel3D()


    /****Utility Functions*****/
    std::vector<glm::vec3> modelLengths; //use global model id
    std::vector<glm::vec3> modelLengthsMax;
    std::vector<glm::vec3> modelLengthsMin;

    void LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);
};

#endif
