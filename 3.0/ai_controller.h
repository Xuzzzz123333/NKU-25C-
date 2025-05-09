#pragma once
#include "player.h"
#include "game_mode.h"  // 添加game_mode.h的引用
#include <random>
#include <chrono>

// AI难度级别
enum class AIDifficulty {
    EASY,    // 简单
    NORMAL,  // 普通
    HARD     // 困难
};

class AIController {
private:
    Player* controlledPlayer;      // AI控制的玩家
    Player* targetPlayer;          // 目标玩家(通常是玩家1)
    AIDifficulty difficulty;       // AI难度
    std::mt19937 rng;              // 随机数生成器
    
    // AI思考计时器
    int thinkTimer;                // AI重新思考行动的时间
    int actionDuration;            // 当前行动的持续时间
    int jumpCooldown;              // 跳跃冷却时间
    int attackCooldown;            // 普通攻击冷却时间
    int specialAttackCooldown;     // 特殊攻击冷却时间
    int idleTimer;                 // 呆滞时间 - 增加AI随机呆滞的时间
    int missedAttackChance;        // 错过攻击机会的概率
    int wrongMoveChance;           // 做出错误移动决策的概率
    
    // 当前行动的状态
    bool movingLeft;
    bool movingRight;
    bool shouldJump;
    bool shouldAttack;
    bool shouldSpecialAttack;
    bool isIdle;                   // 是否处于呆滞状态
    
    // 随机选择行动的方法
    void chooseRandomAction() {
        // 减少随机呆滞的可能性
        int idleChance = 15; // 降低到15%（原来是25%）
        if (difficulty == AIDifficulty::EASY) {
            idleChance = 15; // 简单难度也降低呆滞概率（原来是25%）
        }
        
        if (std::uniform_int_distribution<>(0, 100)(rng) < idleChance) {
            movingLeft = false;
            movingRight = false;
            shouldJump = false;
            shouldAttack = false;
            shouldSpecialAttack = false;
            isIdle = true;
            // 进一步减少呆滞时间
            idleTimer = static_cast<int>(std::uniform_int_distribution<>(150, 350)(rng));  // 减少呆滞时间（原来是250-600）
            return;
        }
        
        isIdle = false;
        
        // 重置所有行动状态
        movingLeft = false;
        movingRight = false;
        shouldJump = false;
        shouldAttack = false;
        shouldSpecialAttack = false;
        
        // 随机决定下一个行动
        int actionChoice = std::uniform_int_distribution<>(0, 100)(rng);
        
        // 移动决策 - 提高简单难度下的移动概率
        if (actionChoice < 40) {  // 提高到40%（原来是35%）
            // 在简单难度下，有时会做出错误的移动决策（往相反方向移动）
            if (difficulty == AIDifficulty::EASY && std::uniform_int_distribution<>(0, 100)(rng) < wrongMoveChance) {
                if (controlledPlayer->get_position().x > targetPlayer->get_position().x) {
                    movingRight = true;  // 错误方向
                } else {
                    movingLeft = true;   // 错误方向
                }
            } else {
                // 正常决策：朝着目标玩家的方向移动
                if (controlledPlayer->get_position().x > targetPlayer->get_position().x) {
                    movingLeft = true;
                } else {
                    movingRight = true;
                }
            }
        }
        
        // 跳跃决策 - 提高简单难度下的跳跃概率
        if (jumpCooldown <= 0 && std::uniform_int_distribution<>(0, 100)(rng) < 15) {  // 提高到15%（原来是10%）
            shouldJump = true;
            jumpCooldown = static_cast<int>(std::uniform_int_distribution<>(60, 120)(rng));  // 减少跳跃冷却时间（原来是80-150）
        }
        
        // 普通攻击决策 - 提高简单难度下的攻击概率
        if (attackCooldown <= 0 && std::uniform_int_distribution<>(0, 100)(rng) < 20) {  // 提高到20%（原来是15%）
            // 在简单难度下，有时会错过攻击机会
            if (difficulty == AIDifficulty::EASY && std::uniform_int_distribution<>(0, 100)(rng) < missedAttackChance) {
                shouldAttack = false;  // 故意错过攻击
            } else {
                shouldAttack = true;
                attackCooldown = static_cast<int>(std::uniform_int_distribution<>(50, 100)(rng));  // 减少攻击冷却时间（原来是60-120）
            }
        }
        
        // 特殊攻击决策 - 提高简单难度下的特殊攻击概率
        if (specialAttackCooldown <= 0 && controlledPlayer->get_mp() >= 100 &&
            std::uniform_int_distribution<>(0, 100)(rng) < 25) {  // 提高到25%（原来是20%）
            // 在简单难度下，有时会错过特殊攻击机会
            if (difficulty == AIDifficulty::EASY && std::uniform_int_distribution<>(0, 100)(rng) < missedAttackChance) {
                shouldSpecialAttack = false;  // 故意错过特殊攻击
            } else {
                shouldSpecialAttack = true;
                specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(100, 180)(rng));  // 减少特殊攻击冷却时间（原来是120-200）
            }
        }
        
        // 设置当前行动的持续时间，减少行动持续时间以提高反应速度
        actionDuration = static_cast<int>(std::uniform_int_distribution<>(20, 70)(rng));  // 减少到20-70（原来是30-80）
    }
    
    // 随机和策略结合的智能决策方法
    void makeStrategicDecision() {
        // 进一步减少随机呆滞的可能性
        int idleChance = 2; // 降低普通难度的呆滞概率（原来是5%）
        
        // 进一步降低困难模式下的呆滞概率
        if (difficulty == AIDifficulty::HARD) {
            idleChance = 0; // 困难模式下完全不会呆滞
        } else if (difficulty == AIDifficulty::NORMAL) {
            idleChance = 2; // 普通模式下2%概率呆滞（原来是5%）
        }
        
        if (std::uniform_int_distribution<>(0, 100)(rng) < idleChance) {
            movingLeft = false;
            movingRight = false;
            shouldJump = false;
            shouldAttack = false;
            shouldSpecialAttack = false;
            isIdle = true;
            
            // 进一步减少呆滞时间
            int minIdleTime = 50, maxIdleTime = 150; // 减少普通难度的呆滞时间（原来是100-300）
            if (difficulty == AIDifficulty::HARD) {
                minIdleTime = 0;  // 困难模式下呆滞时间为0
                maxIdleTime = 0;
            }
            
            idleTimer = static_cast<int>(std::uniform_int_distribution<>(minIdleTime, maxIdleTime)(rng));
            return;
        }
        
        isIdle = false;
    
        float distanceX = abs(controlledPlayer->get_position().x - targetPlayer->get_position().x);
        float distanceY = abs(controlledPlayer->get_position().y - targetPlayer->get_position().y);
        
        // 提高AI追击玩家的积极性
        // 困难模式下更积极地追击玩家
        float farDistance = 650; // 增加普通难度的追击距离（原来是600）
        if (difficulty == AIDifficulty::HARD) {
            farDistance = 900; // 困难模式下，AI会从更远距离追击玩家（原来是800）
        } else if (difficulty == AIDifficulty::NORMAL) {
            farDistance = 750; // 普通难度下的追击距离也增加（原来是650）
        }
        
        if (distanceX > farDistance) {
            // 随机徘徊而不是直接追击玩家
            int randomMoveChance = 30; // 降低普通难度的随机移动概率（原来是40%）
            if (difficulty == AIDifficulty::HARD) {
                randomMoveChance = 5; // 困难模式下只有5%概率随机移动（原来是10%），更专注于追击
            } else if (difficulty == AIDifficulty::NORMAL) {
                randomMoveChance = 20; // 普通难度下的随机移动概率也降低（原来是30%）
            }
            
            if (std::uniform_int_distribution<>(0, 100)(rng) < randomMoveChance) {
                int randomDir = std::uniform_int_distribution<>(0, 1)(rng);
                movingLeft = (randomDir == 0);
                movingRight = (randomDir == 1);
            } else {
                movingLeft = controlledPlayer->get_position().x > targetPlayer->get_position().x;
                movingRight = !movingLeft;
            }
            
            // 提高远距离跳跃频率
            int jumpChance = 20; // 增加普通难度的跳跃概率（原来是15%）
            if (difficulty == AIDifficulty::HARD) {
                jumpChance = 45; // 困难模式下提高到45%（原来是35%）
            } else if (difficulty == AIDifficulty::NORMAL) {
                jumpChance = 30; // 普通难度下的跳跃概率也增加（原来是20%）
            }
            
            if (jumpCooldown <= 0 && std::uniform_int_distribution<>(0, 100)(rng) < jumpChance) {
                shouldJump = true;
                // 减少跳跃冷却时间
                int minJumpCd = 60, maxJumpCd = 120; // 减少普通难度的跳跃冷却（原来是70-130）
                if (difficulty == AIDifficulty::HARD) {
                    minJumpCd = 20;  // 减少到20（原来是30）
                    maxJumpCd = 60; // 减少到60（原来是80）
                } else if (difficulty == AIDifficulty::NORMAL) {
                    minJumpCd = 50;  // 普通难度的跳跃冷却也减少（原来是60）
                    maxJumpCd = 100; // 原来是120
                }
                jumpCooldown = static_cast<int>(std::uniform_int_distribution<>(minJumpCd, maxJumpCd)(rng));
            }
            
            // 提高远距离攻击概率（进一步增强）
            if (difficulty == AIDifficulty::HARD && attackCooldown <= 0 && 
                std::uniform_int_distribution<>(0, 100)(rng) < 35) { // 原来是25%
                shouldAttack = true;
                attackCooldown = static_cast<int>(std::uniform_int_distribution<>(20, 50)(rng)); // 原来是30-60
            } else if (difficulty == AIDifficulty::NORMAL && attackCooldown <= 0 && 
                std::uniform_int_distribution<>(0, 100)(rng) < 25) { // 原来是15%
                shouldAttack = true;
                attackCooldown = static_cast<int>(std::uniform_int_distribution<>(40, 80)(rng)); // 原来是50-100
            }
        }
        // 提高中近距离攻击判断能力
        // 困难模式下扩大攻击距离和提高攻击频率
        else if (distanceX < (difficulty == AIDifficulty::HARD ? 240 : (difficulty == AIDifficulty::NORMAL ? 200 : 120))) {
            // 增强困难模式下的预判能力
            // 如果目标玩家在移动，AI会预判其移动方向并提前攻击
            if (difficulty == AIDifficulty::HARD && 
                std::uniform_int_distribution<>(0, 100)(rng) < 70) { // 70%概率使用预判
                // 这里简化实现，假设如果目标玩家速度大于0，就说明在向右移动
                if (targetPlayer->get_velocity().x > 0) {
                    // 预判玩家向右移动，AI会更倾向于向右移动以跟上
                    if (controlledPlayer->get_position().x < targetPlayer->get_position().x) {
                        // 如果AI在玩家左侧，加速向右追击
                        movingRight = true;
                        movingLeft = false;
                    }
                } else if (targetPlayer->get_velocity().x < 0) {
                    // 预判玩家向左移动，AI会更倾向于向左移动以跟上
                    if (controlledPlayer->get_position().x > targetPlayer->get_position().x) {
                        // 如果AI在玩家右侧，加速向左追击
                        movingLeft = true;
                        movingRight = false;
                    }
                }
            }
            
            // 如果MP足够，使用特殊攻击
            // 普通难度和困难难度下，提高特殊攻击概率
            int specialAttackChance = 45; // 提高普通难度的特殊攻击概率（原来是35%）
            if (difficulty == AIDifficulty::NORMAL) {
                specialAttackChance = 65;  // 提高到65%（原来是50%）
            } else if (difficulty == AIDifficulty::HARD) {
                specialAttackChance = 95;  // 提高到95%（原来是85%）
            }
            
            if (specialAttackCooldown <= 0 && controlledPlayer->get_mp() >= 100 &&
                std::uniform_int_distribution<>(0, 100)(rng) < specialAttackChance) {
                // 在简单难度下，有时会错过特殊攻击机会
                if (difficulty == AIDifficulty::EASY && std::uniform_int_distribution<>(0, 100)(rng) < missedAttackChance) {
                    shouldSpecialAttack = false;  // 故意错过特殊攻击
                } else {
                    shouldSpecialAttack = true;
                    // 减少特殊攻击冷却时间
                    if (difficulty == AIDifficulty::NORMAL) {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(40, 90)(rng)); // 减少普通难度冷却时间（原来是60-120）
                    } else if (difficulty == AIDifficulty::HARD) {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(20, 45)(rng)); // 大幅减少冷却时间（原来是30-60）
                    } else {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(120, 200)(rng)); // 维持简单难度冷却时间
                    }
                }
            }
            // 否则使用普通攻击
            // 普通难度和困难难度下，提高普通攻击概率
            int normalAttackChance = 35; // 提高普通难度的普通攻击概率（原来是25%）
            if (difficulty == AIDifficulty::NORMAL) {
                normalAttackChance = 60;  // 提高到60%（原来是45%）
            } else if (difficulty == AIDifficulty::HARD) {
                normalAttackChance = 90;  // 提高到90%（原来是75%）
            }
            
            // 进一步增强困难模式下的连招能力
            if (difficulty == AIDifficulty::HARD && 
                controlledPlayer->get_mp() < 100 && // 如果MP不足以使用特殊攻击
                std::uniform_int_distribution<>(0, 100)(rng) < normalAttackChance) {
                shouldAttack = true;
                // 进一步减少攻击冷却时间
                attackCooldown = static_cast<int>(std::uniform_int_distribution<>(10, 30)(rng)); // 原来是20-50
            } else if (difficulty == AIDifficulty::NORMAL && 
                      controlledPlayer->get_mp() < 100 && // 如果MP不足以使用特殊攻击
                      std::uniform_int_distribution<>(0, 100)(rng) < normalAttackChance) {
                shouldAttack = true;
                // 减少普通难度的攻击冷却时间
                attackCooldown = static_cast<int>(std::uniform_int_distribution<>(30, 60)(rng)); // 原来是40-80
            } else if (difficulty == AIDifficulty::EASY && 
                      controlledPlayer->get_mp() < 100 && 
                      std::uniform_int_distribution<>(0, 100)(rng) < 20) {
                // 简单难度有时会错过攻击机会
                if (std::uniform_int_distribution<>(0, 100)(rng) < missedAttackChance) {
                    shouldAttack = false;
                } else {
                    shouldAttack = true;
                    attackCooldown = static_cast<int>(std::uniform_int_distribution<>(60, 120)(rng));
                }
            }
            
            // 困难和普通模式增加近距离躲避闪避能力
            if (difficulty == AIDifficulty::HARD || difficulty == AIDifficulty::NORMAL) {
                // 如果对手正在攻击且距离很近，增加跳跃闪避概率
                // 简化实现：检测对手MP是否在50-99之间作为可能即将使用特殊攻击的信号
                // 实际游戏中可能需要更复杂的判断逻辑
                if (targetPlayer->get_mp() >= 50 && targetPlayer->get_mp() < 100 && 
                    distanceX < 80 && jumpCooldown <= 0) {
                    
                    int dodgeChance = (difficulty == AIDifficulty::HARD) ? 85 : 65;
                    if (std::uniform_int_distribution<>(0, 100)(rng) < dodgeChance) {
                        shouldJump = true;
                        jumpCooldown = static_cast<int>(std::uniform_int_distribution<>(
                            (difficulty == AIDifficulty::HARD) ? 15 : 30, 
                            (difficulty == AIDifficulty::HARD) ? 40 : 60)(rng));
                    }
                }
            }
        }
        // 处理中距离情况（新增更智能的中距离行为）
        else {
            // 在中距离情况下，困难模式AI会更积极地靠近玩家
            if (difficulty == AIDifficulty::HARD || difficulty == AIDifficulty::NORMAL) {
                // 确定是否需要接近玩家
                bool shouldApproach = true;
                
                // 如果AI的血量低于玩家，困难模式AI有时会选择拉开距离
                if (difficulty == AIDifficulty::HARD && 
                    controlledPlayer->get_hp() < targetPlayer->get_hp() * 0.7 && // HP低于玩家的70%（原来是60%）
                    std::uniform_int_distribution<>(0, 100)(rng) < 55) { // 55%概率选择拉开距离（原来是40%）
                    shouldApproach = false;
                    
                    // 如果MP接近满，即使血量低也会考虑主动攻击
                    if (controlledPlayer->get_mp() >= 90 && 
                        std::uniform_int_distribution<>(0, 100)(rng) < 70) {
                        shouldApproach = true;
                    }
                }
                
                if (shouldApproach) {
                    // 接近玩家
                    movingLeft = controlledPlayer->get_position().x > targetPlayer->get_position().x;
                    movingRight = !movingLeft;
                    
                    // 中距离的跳跃概率增加
                    int midRangeJumpChance = (difficulty == AIDifficulty::HARD) ? 40 : 30;
                    if (jumpCooldown <= 0 && std::uniform_int_distribution<>(0, 100)(rng) < midRangeJumpChance) {
                        shouldJump = true;
                        int minJumpCd = (difficulty == AIDifficulty::HARD) ? 30 : 50;
                        int maxJumpCd = (difficulty == AIDifficulty::HARD) ? 70 : 100;
                        jumpCooldown = static_cast<int>(std::uniform_int_distribution<>(minJumpCd, maxJumpCd)(rng));
                    }
                } else {
                    // 拉开距离
                    movingLeft = controlledPlayer->get_position().x < targetPlayer->get_position().x;
                    movingRight = !movingLeft;
                    
                    // 拉开距离时更频繁跳跃
                    if (jumpCooldown <= 0 && std::uniform_int_distribution<>(0, 100)(rng) < 60) { // 原来是45%
                        shouldJump = true;
                        jumpCooldown = static_cast<int>(std::uniform_int_distribution<>(20, 50)(rng)); // 原来是30-70
                    }
                    
                    // 困难模式在拉开距离时也会尝试进行远程攻击
                    if (difficulty == AIDifficulty::HARD && attackCooldown <= 0 && 
                        std::uniform_int_distribution<>(0, 100)(rng) < 50) {
                        shouldAttack = true;
                        attackCooldown = static_cast<int>(std::uniform_int_distribution<>(30, 60)(rng));
                    }
                }
            }
            
            // 中距离特殊攻击能力优化
            // 改进AI在中距离使用特殊技能的决策逻辑
            if (specialAttackCooldown <= 0 && controlledPlayer->get_mp() >= 100) {
                int specialAttackMidRangeChance = 0;
                
                if (difficulty == AIDifficulty::HARD) {
                    specialAttackMidRangeChance = 65; // 提高到65%概率（原来是50%）
                    
                    // 当对手处于有利地形位置时，困难模式AI更倾向于使用特殊技能
                    if (targetPlayer->get_position().y < controlledPlayer->get_position().y) {
                        specialAttackMidRangeChance += 20; // 增加20%概率（原来是15%）
                    }
                    
                    // 如果AI的HP低于对手，更积极使用特殊技能
                    if (controlledPlayer->get_hp() < targetPlayer->get_hp()) {
                        specialAttackMidRangeChance += 25; // 增加25%概率（原来是20%）
                    }
                } else if (difficulty == AIDifficulty::NORMAL) {
                    specialAttackMidRangeChance = 45; // 提高到45%概率（原来是35%）
                    
                    // 普通难度也根据战况调整，但幅度较小
                    if (targetPlayer->get_position().y < controlledPlayer->get_position().y) {
                        specialAttackMidRangeChance += 15; // 增加15%概率（原来是10%）
                    }
                    
                    if (controlledPlayer->get_hp() < targetPlayer->get_hp()) {
                        specialAttackMidRangeChance += 20; // 增加20%概率（原来是15%）
                    }
                } else {
                    specialAttackMidRangeChance = 15; // 简单难度下的基础概率（原来是10%）
                }
                
                // 最终决策
                if (std::uniform_int_distribution<>(0, 100)(rng) < specialAttackMidRangeChance) {
                    shouldSpecialAttack = true;
                    
                    // 根据难度设置冷却时间
                    if (difficulty == AIDifficulty::NORMAL) {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(50, 100)(rng)); // 原来是60-120
                    } else if (difficulty == AIDifficulty::HARD) {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(30, 60)(rng)); // 原来是40-80
                    } else {
                        specialAttackCooldown = static_cast<int>(std::uniform_int_distribution<>(90, 160)(rng)); // 原来是100-180
                    }
                }
            }
        }
    }

public:
    AIController(Player* controlledPlayer, Player* targetPlayer, AIDifficulty difficulty = AIDifficulty::NORMAL)
        : controlledPlayer(controlledPlayer), targetPlayer(targetPlayer), difficulty(difficulty),
          thinkTimer(0), actionDuration(0), jumpCooldown(0), attackCooldown(0), specialAttackCooldown(0),
          idleTimer(0), missedAttackChance(0), wrongMoveChance(0), movingLeft(false), movingRight(false), 
          shouldJump(false), shouldAttack(false), shouldSpecialAttack(false), isIdle(false) {
        // 初始化随机数生成器
        unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
        rng.seed(seed);
        
        // 根据难度设置错过攻击机会和错误移动决策的概率
        switch (difficulty) {
            case AIDifficulty::EASY:
                missedAttackChance = 45;  // 降低到45%概率错过攻击机会（原来是50%）
                wrongMoveChance = 30;     // 降低到30%概率做出错误移动决策（原来是35%）
                break;
            case AIDifficulty::NORMAL:
                missedAttackChance = 8;  // 降低到8%概率错过攻击机会（原来是15%）
                wrongMoveChance = 5;     // 降低到5%概率做出错误移动决策（原来是10%）
                break;
            case AIDifficulty::HARD:
                missedAttackChance = 0;   // 降低到0%概率错过攻击机会（原来是1%）
                wrongMoveChance = 0;      // 维持0%概率做出错误移动决策
                break;
        }
    }
    
    void update(int deltaTime) {
        // 更新所有计时器
        thinkTimer -= deltaTime;
        actionDuration -= deltaTime;
        jumpCooldown -= deltaTime;
        attackCooldown -= deltaTime;
        specialAttackCooldown -= deltaTime;
        
        // 如果处于呆滞状态，更新呆滞计时器
        if (isIdle) {
            idleTimer -= deltaTime;
            if (idleTimer <= 0) {
                isIdle = false;
                // 困难模式下，从呆滞恢复后立即进行决策，不等待下一个thinkTimer
                if (difficulty == AIDifficulty::HARD) {
                    thinkTimer = 0;
                }
            } else {
                // 呆滞状态下不做任何操作
                return;
            }
        }
        
        // 存储上一帧的移动状态，用于发送KEYUP消息
        bool wasMovingLeft = movingLeft;
        bool wasMovingRight = movingRight;
        
        // 如果需要重新思考或当前行动已经结束，则选择新的行动
        if (thinkTimer <= 0 || actionDuration <= 0) {
            if (difficulty == AIDifficulty::EASY) {
                chooseRandomAction();  // 简单难度AI会随机选择行动
            } else {
                makeStrategicDecision();  // 普通/困难难度会使用更智能的决策
            }
            
            // 进一步减少思考间隔时间，提高AI反应速度
            switch (difficulty) {
                case AIDifficulty::EASY:
                    thinkTimer = static_cast<int>(std::uniform_int_distribution<>(50, 120)(rng));  // 减少思考间隔（原来是80-150）
                    break;
                case AIDifficulty::NORMAL:
                    thinkTimer = static_cast<int>(std::uniform_int_distribution<>(15, 40)(rng));  // 减少思考间隔（原来是25-60）
                    break;
                case AIDifficulty::HARD:
                    thinkTimer = static_cast<int>(std::uniform_int_distribution<>(3, 10)(rng));  // 大幅减少思考间隔（原来是5-20）
                    break;
            }
        }
        
        // 创建输入消息并发送给被控制的玩家
        ExMessage msg;
        memset(&msg, 0, sizeof(ExMessage));
        
        // 处理移动键的释放
        if (wasMovingLeft && !movingLeft) {
            msg.message = WM_KEYUP;
            msg.vkcode = VK_LEFT;  // 左方向键 - P2向左移动
            controlledPlayer->on_input(msg);
        }
        
        if (wasMovingRight && !movingRight) {
            msg.message = WM_KEYUP;
            msg.vkcode = VK_RIGHT;  // 右方向键 - P2向右移动
            controlledPlayer->on_input(msg);
        }
        
        // 移动指令
        if (movingLeft) {
            msg.message = WM_KEYDOWN;
            msg.vkcode = VK_LEFT;  // 左方向键 - P2向左移动
            controlledPlayer->on_input(msg);
        }
        
        if (movingRight) {
            msg.message = WM_KEYDOWN;
            msg.vkcode = VK_RIGHT;  // 右方向键 - P2向右移动
            controlledPlayer->on_input(msg);
        }
        
        // 跳跃指令
        if (shouldJump) {
            msg.message = WM_KEYDOWN;
            msg.vkcode = VK_UP;  // 上方向键 - P2跳跃
            controlledPlayer->on_input(msg);
            shouldJump = false;  // 跳跃指令只发送一次
        }
        
        // 普通攻击指令
        if (shouldAttack) {
            msg.message = WM_KEYDOWN;
            msg.vkcode = VK_OEM_PERIOD;  // 句号键 - P2普通攻击
            controlledPlayer->on_input(msg);
            shouldAttack = false;  // 攻击指令只发送一次
        }
        
        // 特殊攻击指令
        if (shouldSpecialAttack) {
            msg.message = WM_KEYDOWN;
            msg.vkcode = VK_OEM_2;  // 斜杠键 - P2特殊攻击
            controlledPlayer->on_input(msg);
            shouldSpecialAttack = false;  // 特殊攻击指令只发送一次
        }
    }
    
    void setDifficulty(AIDifficulty newDifficulty) {
        difficulty = newDifficulty;
        
        // 更新错过攻击机会和错误移动决策的概率
        switch (difficulty) {
            case AIDifficulty::EASY:
                missedAttackChance = 45;  // 降低到45%概率错过攻击机会（原来是50%）
                wrongMoveChance = 30;     // 降低到30%概率做出错误移动决策（原来是35%）
                break;
            case AIDifficulty::NORMAL:
                missedAttackChance = 8;  // 降低到8%概率错过攻击机会（原来是15%）
                wrongMoveChance = 5;     // 降低到5%概率做出错误移动决策（原来是10%）
                break;
            case AIDifficulty::HARD:
                missedAttackChance = 0;   // 降低到0%概率错过攻击机会（原来是1%）
                wrongMoveChance = 0;      // 维持0%概率做出错误移动决策
                break;
        }
    }
    
    AIDifficulty getDifficulty() const {
        return difficulty;
    }
}; 