#pragma once
#include "game_mode.h"
#include "vector2.h"  // 直接包含Vector2的头文件
#include <random>
#include <chrono>
#include <string>  // 添加string头文件用于角色类型识别
#include <algorithm> // 为 std::max 添加

// AI难度等级
enum class AIDifficulty {
    EASY,    // 简单
    NORMAL,  // 普通
    HARD,    // 困难
    HELL     // 地狱
};

// 全局整型变量，表示当前AI难度
// 这个变量在game.scene.cpp中定义，解决了player.h和ai_controller.h的循环依赖问题
// 0=简单(EASY), 1=普通(NORMAL), 2=困难(HARD), 3=地狱(HELL)
extern int g_ai_difficulty;

// 前向声明以避免循环依赖
class Player;
struct ExMessage;

// 简单的角色类型枚举
enum class CharacterType {
    UNKNOWN,
    PEASHOOTER,
    SUNFLOWER
};

class AIController {
private:
    void* controlledPlayer;      // AI控制的玩家，使用void*避免类型问题
    void* targetPlayer;          // 目标玩家(通常是玩家1)，使用void*避免类型问题
    AIDifficulty difficulty;     // AI难度
    std::mt19937 rng;            // 随机数生成器
    
    // 基本属性
    int missedAttackChance;      // 错过攻击机会的概率
    int wrongMoveChance;         // 做出错误移动决策的概率
    
    // AI行为状态
    int aiActionTimer = 0;       // AI行动计时器
    int proactiveTimer = 0;      // 主动行动计时器，驱动AI做出主动决策
    bool isMovingLeft = false;   // 是否向左移动
    bool isMovingRight = false;  // 是否向右移动
    int actionDelayTimer = 0;    // 动作延迟计时器，用于减少AI频繁更改决策
    
    // 卡住检测相关
    Vector2 lastPosition;        // 上一次检测时的位置，使用默认构造函数初始化
    int stuckTimer = 0;         // 卡住检测计时器
    int stuckCounter = 0;       // 卡住计数器
    int stuckEscapeTime = 0;    // 脱离卡住状态的剩余时间
    
    // 攻击行为相关
    float optimalDistance = 200.0f;   // AI与玩家的最佳攻击距离
    int attackCooldown = 0;           // 攻击冷却计时器
    CharacterType characterType = CharacterType::UNKNOWN;  // 角色类型
    
    // 向日葵弹道相关
    const float sunProjectileGravity = 1.6e-3f;  // 与玩家重力相同
    const Vector2 sunProjectileVelocity = {0.25f, -0.5f}; // 向日葵子弹初始速度
    Vector2 lastTargetPosition;   // 上次记录的目标位置
    Vector2 targetVelocityEstimate; // 目标速度估计
    int targetVelocityUpdateTimer = 0; // 目标速度更新计时器
    
    // 辅助函数
    void StopMoving(Player* aiPlayer); // 停止所有移动
    bool TryAttack(Player* aiPlayer, bool playerIsOnLeft); // 尝试攻击，返回是否成功
    
    // 设置角色类型
    void setCharacterType(CharacterType type) {
        characterType = type;
        // 调整最佳距离
        adjustOptimalDistanceByCharacter();
    }
    
    // 根据角色类型调整最佳距离
    void adjustOptimalDistanceByCharacter() {
        // 基础距离已根据难度设置，这里进行角色特定的调整
        float distanceMultiplier = 1.0f;
        
        switch (characterType) {
            case CharacterType::SUNFLOWER:
                // 向日葵需要更大的距离才能有效命中
                distanceMultiplier = 1.5f;
                break;
            case CharacterType::PEASHOOTER:
                // 豌豆射手可以使用标准距离
                distanceMultiplier = 1.1f;
                break;
            default:
                distanceMultiplier = 1.0f;
                break;
        }
        
        // 应用乘数调整距离
        optimalDistance *= distanceMultiplier;
    }
    
    // 预测向日葵弹道的命中概率，返回0-100的分值，100表示最可能命中
    int predictSunflowerHitProbability(const Vector2& aiPos, const Vector2& playerPos, bool playerIsOnLeft, const Vector2& playerVelocity) {
        // 向日葵子弹的初始速度向量
        Vector2 bulletVelocity = {
            playerIsOnLeft ? -sunProjectileVelocity.x : sunProjectileVelocity.x, 
            sunProjectileVelocity.y
        };
        
        // 初始发射位置（大致位于角色中心）
        Vector2 bulletPosition = aiPos;
        
        // 用于检测命中的目标区域（玩家位置上下浮动范围）
        float targetY = playerPos.y;
        float targetYTolerance = 50.0f; // 垂直容错范围
        
        // 预测时间步长
        const float timeStep = 10.0f; // 模拟步长，越小越精确
        const int maxSteps = 500;     // 最大模拟步数，防止无限循环
        
        // 根据难度调整击中概率计算的精度
        float precisionFactor = 1.0f;
        switch (difficulty) {
            case AIDifficulty::EASY:    precisionFactor = 0.7f; break;
            case AIDifficulty::NORMAL:  precisionFactor = 0.85f; break;
            case AIDifficulty::HARD:    precisionFactor = 1.0f; break;
            case AIDifficulty::HELL:    precisionFactor = 1.1f; break;
        }
        
        // 模拟子弹飞行
        Vector2 simBulletPos = bulletPosition;
        Vector2 simBulletVel = bulletVelocity;
        
        // 估计玩家移动的位置
        Vector2 estimatedPlayerPos = playerPos;
        float closestDistance = 1000000.0f; // 用一个很大的初始值
        
        for (int step = 0; step < maxSteps; step++) {
            // 更新子弹位置
            simBulletVel.y += sunProjectileGravity * timeStep;
            simBulletPos.x += simBulletVel.x * timeStep;
            simBulletPos.y += simBulletVel.y * timeStep;
            
            // 更新估计的玩家位置（考虑简单的线性移动）
            estimatedPlayerPos.x += playerVelocity.x * timeStep * 0.2f; // 降低预测系数，玩家可能会改变方向
            
            // 计算子弹与估计玩家位置的水平距离
            float horizontalDistance = std::abs(simBulletPos.x - estimatedPlayerPos.x);
            
            // 记录最接近的距离
            if (horizontalDistance < closestDistance) {
                closestDistance = horizontalDistance;
            }
            
            // 检查是否在玩家高度范围内
            if (simBulletPos.y >= targetY - targetYTolerance && 
                simBulletPos.y <= targetY + targetYTolerance) {
                
                // 如果水平距离足够接近，视为可能命中
                if (horizontalDistance < 30.0f) {
                    // 计算命中概率，越接近目标，概率越高
                    int hitProbability = static_cast<int>((1.0f - (horizontalDistance / 30.0f)) * 100.0f * precisionFactor);
                    
                    // 确保概率在0-100范围内
                    if (hitProbability > 100) hitProbability = 100;
                    if (hitProbability < 0) hitProbability = 0;
                    
                    return hitProbability;
                }
            }
            
            // 如果子弹已经过了玩家位置很远，或者掉落到地面以下，就停止模拟
            if ((playerIsOnLeft && simBulletPos.x < estimatedPlayerPos.x - 200.0f) ||
                (!playerIsOnLeft && simBulletPos.x > estimatedPlayerPos.x + 200.0f) ||
                simBulletPos.y > 600.0f) {  // 假设600是地面高度
                break;
            }
        }
        
        // 根据最接近的距离计算一个基础概率
        int baseProbability = (closestDistance < 100.0f) ? 
                            static_cast<int>((1.0f - (closestDistance / 100.0f)) * 50.0f) : 0;
        
        return baseProbability;
    }
    
    // 计算向日葵的最佳攻击距离
    float calculateSunflowerOptimalDistance(const Vector2& playerPos, const Vector2& playerVelocity) {
        // 基础最佳距离范围
        float minDistance = 220.0f;
        float maxDistance = 380.0f;
        float bestDistance = 280.0f;
        float highestProbability = 0.0f;
        
        // 简单线性搜索最佳距离
        for (float testDistance = minDistance; testDistance <= maxDistance; testDistance += 10.0f) {
            // 假设AI位于玩家左侧
            Vector2 leftTestPos = {playerPos.x - testDistance, playerPos.y};
            int leftProbability = predictSunflowerHitProbability(leftTestPos, playerPos, false, playerVelocity);
            
            // 假设AI位于玩家右侧
            Vector2 rightTestPos = {playerPos.x + testDistance, playerPos.y};
            int rightProbability = predictSunflowerHitProbability(rightTestPos, playerPos, true, playerVelocity);
            
            // 取两侧最高概率
            int maxProbability = (std::max)(leftProbability, rightProbability);
            
            if (maxProbability > highestProbability) {
                highestProbability = maxProbability;
                bestDistance = testDistance;
            }
        }
        
        // 返回找到的最佳距离
        return bestDistance;
    }
    
    // 更新目标玩家的速度估计
    void updateTargetVelocityEstimate(const Vector2& currentTargetPos, int deltaTime) {
        targetVelocityUpdateTimer += deltaTime;
        
        // 每100ms更新一次速度估计
        if (targetVelocityUpdateTimer >= 100) {
            // 避免除以零
            if (targetVelocityUpdateTimer > 0) {
                // 计算位置差除以时间得到速度
                targetVelocityEstimate.x = static_cast<float>(currentTargetPos.x - lastTargetPosition.x) / targetVelocityUpdateTimer;
                targetVelocityEstimate.y = static_cast<float>(currentTargetPos.y - lastTargetPosition.y) / targetVelocityUpdateTimer;
            }
            
            // 更新上次位置和重置计时器
            lastTargetPosition = currentTargetPos;
            targetVelocityUpdateTimer = 0;
        }
    }

public:
    AIController(void* controlledPlayer, void* targetPlayer, AIDifficulty difficulty = AIDifficulty::NORMAL)
        : controlledPlayer(controlledPlayer), targetPlayer(targetPlayer), difficulty(difficulty),
          missedAttackChance(0), wrongMoveChance(0) 
    {
        // 初始化随机数生成器
        unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
        rng.seed(seed);
        
        // 根据难度初始化相关参数 - 降低错过攻击机会的概率
        switch (difficulty) {
            case AIDifficulty::EASY:
                missedAttackChance = 20;  // 原45，降低为20
                wrongMoveChance = 15;     // 原30，降低为15
                optimalDistance = 280.0f; // 提高基础距离，从220.0f到280.0f
                break;
            case AIDifficulty::NORMAL:
                missedAttackChance = 5;   // 原8，降低为5
                wrongMoveChance = 3;      // 原5，降低为3
                optimalDistance = 260.0f; // 提高基础距离，从200.0f到260.0f
                break;
            case AIDifficulty::HARD:
                missedAttackChance = 0;   // 保持0
                wrongMoveChance = 0;      // 保持0
                optimalDistance = 240.0f; // 提高基础距离，从180.0f到240.0f
                break;
            case AIDifficulty::HELL:
                missedAttackChance = 0;   // 保持0
                wrongMoveChance = 0;      // 保持0
                optimalDistance = 220.0f; // 提高基础距离，从160.0f到220.0f
                break;
        }
        
        // 初始化全局难度变量，实现Player类对AI难度的访问而无需直接依赖此类
        g_ai_difficulty = static_cast<int>(difficulty);
        
        // 初始化目标速度估计值
        targetVelocityEstimate = {0.0f, 0.0f};
    }
    
    // AI更新方法
    void update(int deltaTime);
    
    // 设置AI控制的角色类型（从外部调用）
    void setControlledCharacterType(CharacterType type) {
        setCharacterType(type);
    }
    
    void setDifficulty(AIDifficulty newDifficulty) {
        difficulty = newDifficulty;
        // 更新全局难度变量
        g_ai_difficulty = static_cast<int>(difficulty);
        
        // 先重置最佳距离
        switch (difficulty) {
            case AIDifficulty::EASY:
                missedAttackChance = 20;  // 原45，降低为20
                wrongMoveChance = 15;     // 原30，降低为15
                optimalDistance = 280.0f; // 提高基础距离，从220.0f到280.0f
                break;
            case AIDifficulty::NORMAL:
                missedAttackChance = 5;   // 原8，降低为5
                wrongMoveChance = 3;      // 原5，降低为3
                optimalDistance = 260.0f; // 提高基础距离，从200.0f到260.0f
                break;
            case AIDifficulty::HARD:
                missedAttackChance = 0;
                wrongMoveChance = 0;
                optimalDistance = 240.0f; // 提高基础距离，从180.0f到240.0f
                break;
            case AIDifficulty::HELL:
                missedAttackChance = 0;
                wrongMoveChance = 0;
                optimalDistance = 220.0f; // 提高基础距离，从160.0f到220.0f
                break;
        }
        
        // 再次应用角色类型调整
        if (characterType != CharacterType::UNKNOWN) {
            adjustOptimalDistanceByCharacter();
        }
    }
    
    AIDifficulty getDifficulty() const {
        return difficulty;
    }
    
    // 获取当前AI难度对应的伤害倍率
    float get_damage_multiplier() const {
        switch (difficulty) {
            case AIDifficulty::EASY: return 0.7f;   // 从0.6提升到0.7
            case AIDifficulty::NORMAL: return 1.0f; // 从0.9提升到1.0
            case AIDifficulty::HARD: return 1.4f;   // 从1.2提升到1.4
            case AIDifficulty::HELL: return 1.5f;   // 从1.8f降低到1.5f
            default: return 0.7f;                   // 默认值
        }
    }
}; 