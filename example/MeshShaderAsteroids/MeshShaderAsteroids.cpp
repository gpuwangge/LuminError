/*把 OBJ 的顶点和索引放进 storage buffer，每个 mesh shader workgroup 固定画 64 个三角形，顶点不复用。
这是能画出完整 OBJ 模型的最简单的 mesh shader 版本，真正的 meshlet 留到下一个 test 再做。
*/

#include "IGame.h"
namespace LuminError{
    class Game : public IGame {
        void Record() override{
            //GameEngine->DrawMeshTasksObject(0); //draw object 0, which is the mesh shader object
            //GameEngine->DrawObjects();
            for(int i = 1; i < GameEngine->GetObjectSize(); i++)
                GameEngine->DrawObject(i);
            GameEngine->DrawTexts();
        }
    };
}
#include "Launcher.hpp"