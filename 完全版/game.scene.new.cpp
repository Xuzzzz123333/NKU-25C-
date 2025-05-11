#include "game.scene.h"
#include "ai_controller.h"
 
// 全局变量定义 
int g_ai_difficulty = 1; // 默认为普通难度
AIDifficulty aiDifficulty = AIDifficulty::NORMAL;
AIController* aiController = nullptr;
// current_game_mode已在main.cpp中定义，此处不再重复定义 