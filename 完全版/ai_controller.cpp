#include "ai_controller.h"
#include "player.h"
#include <graphics.h> // 为ExMessage结构体
#include <cmath> // 为sqrt和pow函数

// AI控制器Update方法实现
void AIController::update(int deltaTime) {
    // 更新AI行动计时器
    aiActionTimer += deltaTime;
    
    // 更新攻击冷却计时器
    if (attackCooldown > 0) {
        attackCooldown -= deltaTime;
    }
    
    // 更新AI卡住检测计时器
    stuckTimer += deltaTime;
    
    // 更新主动行动计时器
    proactiveTimer += deltaTime;
    
    // 添加动作延迟器，但只应用于移动决策，不影响攻击决策
    if (actionDelayTimer > 0) {
        actionDelayTimer -= deltaTime;
    }
    
    // 安全类型转换
    Player* aiPlayer = static_cast<Player*>(controlledPlayer);
    Player* humanPlayer = static_cast<Player*>(targetPlayer);
    
    if (!aiPlayer || !humanPlayer) return;
    
    // 获取玩家和AI位置
    const Vector2& playerPos = humanPlayer->get_position();
    const Vector2& aiPos = aiPlayer->get_position();
    const Vector2& aiVelocity = aiPlayer->get_velocity();
    const Vector2& aiSize = aiPlayer->get_size();
    
    // 更新目标玩家的速度估计
    updateTargetVelocityEstimate(playerPos, deltaTime);
    
    // 计算与玩家的水平距离
    float horizontalDistance = std::abs(playerPos.x - aiPos.x);
    
    // 判断玩家是否在AI的左侧或右侧（用于确定移动和攻击方向）
    bool playerIsOnLeft = playerPos.x < aiPos.x;
    
    // 检查当前面向方向
    bool isFacingLeft = aiPlayer->is_facing_left();
    bool faceDirectionCorrect = (playerIsOnLeft == isFacingLeft);
    
    // 更新调试变量 - 对于卡住和方向判断问题很有帮助
    if (!faceDirectionCorrect && horizontalDistance < optimalDistance * 1.2f) {
        // 如果面向不正确且距离足够近，可能影响攻击判断
        // 考虑在必要时修正面向
    }
    
    // 检查是否卡在墙角 - 如果位置很久没变化且正在尝试移动
    if (stuckTimer >= 1000) { // 每秒检查一次
        stuckTimer = 0;
        
        // 检查位置是否变化很小
        float positionDiff = std::sqrt(
            std::pow(static_cast<float>(aiPos.x - lastPosition.x), 2.0f) + 
            std::pow(static_cast<float>(aiPos.y - lastPosition.y), 2.0f)
        );
        
        if (positionDiff < 5.0f && (isMovingLeft || isMovingRight)) {
            // AI可能卡住了
            stuckCounter++;
            
            if (stuckCounter >= 3) { // 连续3次检测到卡住
                // 尝试跳跃以脱离卡住状态
                ExMessage jumpMsg;
                jumpMsg.message = WM_KEYDOWN;
                jumpMsg.vkcode = VK_UP;
                aiPlayer->on_input(jumpMsg);
                
                // 反向移动
                if (isMovingLeft) {
                    // 停止向左移动，开始向右移动
                    ExMessage releaseLeftMsg;
                    releaseLeftMsg.message = WM_KEYUP;
                    releaseLeftMsg.vkcode = VK_LEFT;
                    aiPlayer->on_input(releaseLeftMsg);
                    
                    ExMessage rightMsg;
                    rightMsg.message = WM_KEYDOWN;
                    rightMsg.vkcode = VK_RIGHT;
                    aiPlayer->on_input(rightMsg);
                    
                    isMovingLeft = false;
                    isMovingRight = true;
                    stuckCounter = 0;
                    stuckEscapeTime = 500; // 设置脱离时间
                }
                else if (isMovingRight) {
                    // 停止向右移动，开始向左移动
                    ExMessage releaseRightMsg;
                    releaseRightMsg.message = WM_KEYUP;
                    releaseRightMsg.vkcode = VK_RIGHT;
                    aiPlayer->on_input(releaseRightMsg);
                    
                    ExMessage leftMsg;
                    leftMsg.message = WM_KEYDOWN;
                    leftMsg.vkcode = VK_LEFT;
                    aiPlayer->on_input(leftMsg);
                    
                    isMovingRight = false;
                    isMovingLeft = true;
                    stuckCounter = 0;
                    stuckEscapeTime = 500; // 设置脱离时间
                }
            }
        }
        else {
            // 正常移动，重置计数器
            stuckCounter = 0;
        }
        
        // 更新上次位置
        lastPosition = aiPos;
    }
    
    // 判断是否正在尝试脱离卡住状态
    if (stuckEscapeTime > 0) {
        stuckEscapeTime -= deltaTime;
        // 在脱离期间不执行常规AI逻辑
        return;
    }
    
    // 每30ms做一次决策，进一步提高决策频率
    // 或者当超过主动决策时间(1000ms)时强制做出决策
    if (aiActionTimer >= 30 || proactiveTimer >= 1000) {
        // 重置计时器
        aiActionTimer = 0;
        if (proactiveTimer >= 1000) {
            proactiveTimer = 0;
            // 强制执行决策，忽略动作延迟
            actionDelayTimer = 0;
        }
        
        // 向日葵角色特殊处理，使用弹道预测调整最佳距离
        if (characterType == CharacterType::SUNFLOWER) {
            // 定期重新计算最佳攻击距离
            if (proactiveTimer % 200 == 0 || proactiveTimer == 0) {
                float calculatedDistance = calculateSunflowerOptimalDistance(playerPos, targetVelocityEstimate);
                
                // 平滑过渡到新的最佳距离而不是突变
                optimalDistance = optimalDistance * 0.8f + calculatedDistance * 0.2f;
                
                // 确保距离在合理范围内
                if (optimalDistance < 220.0f) optimalDistance = 220.0f;
                if (optimalDistance > 380.0f) optimalDistance = 380.0f;
            }
        }
        
        // 为向日葵角色特殊处理，需要更精确的距离控制
        float minDistanceRatio = 0.8f;
        float maxDistanceRatio = 1.2f;
        
        if (characterType == CharacterType::SUNFLOWER) {
            // 向日葵需要更严格的距离控制
            minDistanceRatio = 0.9f;  // 更严格的最小距离比例
            maxDistanceRatio = 1.1f;  // 更严格的最大距离比例
        }
        
        // 边缘检测 - 防止AI跳下悬崖
        bool isNearLeftEdge = false;
        bool isNearRightEdge = false;
        
        // 简单边缘检测：检查是否接近屏幕边缘
        const float edgeDetectionDistance = 50.0f; // 减小边缘检测距离，使AI更勇敢
        
        // 检查AI是否站在地面上
        bool isGrounded = aiVelocity.y == 0; // 垂直速度为0表示AI站在地面上
        
        if (isGrounded) {
            // 检查左侧边缘
            if (aiPos.x < edgeDetectionDistance) {
                isNearLeftEdge = true; // 靠近左侧地图边界
            }
            
            // 检查右侧边缘
            if (aiPos.x > (getwidth() - aiSize.x - edgeDetectionDistance)) {
                isNearRightEdge = true; // 靠近右侧地图边界
            }
        }
        
        // 计算是否处于最佳攻击距离范围
        bool inOptimalRange = horizontalDistance >= (optimalDistance * minDistanceRatio) && 
                              horizontalDistance <= (optimalDistance * maxDistanceRatio);
        
        // ======= 攻击决策 - 始终执行，不受动作延迟影响 =======
        if (attackCooldown <= 0) {
            // 获取难度相关的攻击概率
            int attackChance;
            
            // 更高难度下进一步提高攻击频率
            switch (difficulty) {
                case AIDifficulty::EASY:
                    attackChance = 85;           // 提高到85%
                    attackCooldown = 200;        // 保持200ms
                    break;
                case AIDifficulty::NORMAL:
                    attackChance = 90;           // 提高到90%
                    attackCooldown = 150;        // 保持150ms
                    break;
                case AIDifficulty::HARD:
                    attackChance = 95;           // 提高到95%
                    attackCooldown = 100;        // 保持100ms
                    break;
                case AIDifficulty::HELL:
                    attackChance = 97;           // 恢复到97%
                    attackCooldown = 80;         // 恢复到80ms
                    break;
                default:
                    attackChance = 85;
                    attackCooldown = 200;
                    break;
            }
            
            // 向日葵角色特有的攻击冷却调整
            if (characterType == CharacterType::SUNFLOWER) {
                // 向日葵的冷却时间更短，让它能更频繁发射太阳光弹
                attackCooldown = static_cast<int>(attackCooldown * 0.8f);
            }
            
            // 决定是否攻击 - 在最佳范围内或玩家靠得很近时概率更高
            std::uniform_int_distribution<> actionDist(1, 100);
            int action = actionDist(rng);
            bool shouldAttack = action < attackChance || aiPlayer->get_mp() >= 100;
            
            // 如果是向日葵角色，计算当前位置的命中概率
            int hitProbability = 0;
            if (characterType == CharacterType::SUNFLOWER) {
                hitProbability = predictSunflowerHitProbability(aiPos, playerPos, playerIsOnLeft, targetVelocityEstimate);
                
                // 根据命中概率调整攻击决策
                // 命中概率越高，攻击意愿越强
                int adjustedAttackChance = attackChance + hitProbability / 2; // 最多+50%攻击几率
                if (adjustedAttackChance > 100) adjustedAttackChance = 100;
                
                shouldAttack = action < adjustedAttackChance || aiPlayer->get_mp() >= 100;
                
                // 如果命中概率极低，减少攻击意愿
                if (hitProbability < 10 && aiPlayer->get_mp() < 100) {
                    shouldAttack = false;
                }
            } else {
                // 非向日葵角色，使用常规逻辑
                // 如果不在最佳距离但玩家已经很近，提高攻击概率
                if (!inOptimalRange && horizontalDistance < optimalDistance && action < 95) {
                    shouldAttack = true;
                }
            }
            
            // 必定使用特殊攻击，如果能量满了
            if (aiPlayer->get_mp() >= 100) {
                shouldAttack = true;
            }
            
            // 在决定攻击前，检查并修正面向
            if (shouldAttack) {
                // 检查面向是否正确
                bool isFacingCorrectly = (aiPlayer->is_facing_left() == playerIsOnLeft);
                
                if (!isFacingCorrectly) {
                    // 如果面向不正确，先修正面向
                    // 先停止所有移动
                    StopMoving(aiPlayer);
                    
                    // 短暂按下正确方向键以修正面向
                    ExMessage directionMsg;
                    directionMsg.message = WM_KEYDOWN;
                    directionMsg.vkcode = playerIsOnLeft ? VK_LEFT : VK_RIGHT;
                    aiPlayer->on_input(directionMsg);
                    
                    // 短暂延迟
                    int delayCounter = 0;
                    while (!isFacingCorrectly && delayCounter < 5) {
                        // 检查面向是否已修正
                        isFacingCorrectly = (aiPlayer->is_facing_left() == playerIsOnLeft);
                        delayCounter++;
                    }
                    
                    // 释放方向键
                    ExMessage releaseMsg;
                    releaseMsg.message = WM_KEYUP;
                    releaseMsg.vkcode = playerIsOnLeft ? VK_LEFT : VK_RIGHT;
                    aiPlayer->on_input(releaseMsg);
                }
                
                // 尝试攻击，总是先停止所有移动
                StopMoving(aiPlayer);
                
                // 确保始终正确朝向后再攻击
                bool attackSuccess = TryAttack(aiPlayer, playerIsOnLeft);
                if (attackSuccess) {
                    actionDelayTimer = 80; // 攻击后较短的延迟
                }
            }
        }
        
        // ======= 移动决策 - 受动作延迟影响 =======
        if (actionDelayTimer <= 0) {
            // 根据与玩家的距离决定移动行为
            bool needToMoveAway = horizontalDistance < (optimalDistance * minDistanceRatio * 0.8f); // 降低拉开距离的阈值
            bool needToMoveCloser = horizontalDistance > (optimalDistance * maxDistanceRatio); // 保持靠近阈值不变
            bool shouldMove = true; // 默认应该移动
            
            // 向日葵特有的移动策略：如果当前命中率很高，尽量保持不动
            if (characterType == CharacterType::SUNFLOWER) {
                int hitProbability = predictSunflowerHitProbability(aiPos, playerPos, playerIsOnLeft, targetVelocityEstimate);
                
                // 如果命中概率高且在合理距离范围内，优先保持位置
                if (hitProbability > 70 && !needToMoveAway && !needToMoveCloser) {
                    // 增加保持当前位置的概率
                    std::uniform_int_distribution<> stayStillDist(1, 100);
                    if (stayStillDist(rng) < 80) { // 80%概率保持不动
                        shouldMove = false;
                    }
                }
            }
            
            // 偶尔添加主动移动行为，即使距离适中
            std::uniform_int_distribution<> moveDecisionDist(1, 100);
            int proactiveMoveChance = 30; // 30%概率主动移动
            
            // 如果是向日葵，降低主动移动概率，更专注于找到最佳射击位置
            if (characterType == CharacterType::SUNFLOWER) {
                proactiveMoveChance = 20; // 降低到20%
            }
            
            // 如果玩家在不停靠近，增加一个随机因素决定是否继续远离
            if (needToMoveAway) {
                std::uniform_int_distribution<> moveAwayDist(1, 100);
                // 难度越高，越可能坚持保持距离
                int moveAwayThreshold = 70; // 提高到70%概率远离
                switch (difficulty) {
                    case AIDifficulty::EASY: moveAwayThreshold = 40; break;
                    case AIDifficulty::NORMAL: moveAwayThreshold = 60; break;
                    case AIDifficulty::HARD: moveAwayThreshold = 80; break;
                    case AIDifficulty::HELL: moveAwayThreshold = 90; break;
                }
                needToMoveAway = moveAwayDist(rng) <= moveAwayThreshold;
            }
            
            // AI偶尔会主动追击玩家，特别是在更高难度
            std::uniform_int_distribution<> aggressiveDist(1, 100);
            int aggressiveThreshold = 20; // 提高到20%概率主动追击
            switch (difficulty) {
                case AIDifficulty::EASY: aggressiveThreshold = 20; break;
                case AIDifficulty::NORMAL: aggressiveThreshold = 40; break;
                case AIDifficulty::HARD: aggressiveThreshold = 60; break;
                case AIDifficulty::HELL: aggressiveThreshold = 80; break; // 恢复到80%
            }
            
            bool beAggressive = aggressiveDist(rng) <= aggressiveThreshold;
            
            // 如果正在最佳距离范围内且不需要移动，可能进行随机移动
            if (!needToMoveAway && !needToMoveCloser) {
                if (moveDecisionDist(rng) <= proactiveMoveChance || beAggressive) {
                    // 随机选择移动方向
                    std::uniform_int_distribution<> directionDist(0, 1);
                    bool moveLeft = directionDist(rng) == 0;
                    
                    // 向日葵特有：选择可能提高命中率的方向移动
                    if (characterType == CharacterType::SUNFLOWER) {
                        Vector2 leftTestPos = aiPos;
                        leftTestPos.x -= 30.0f; // 测试左移一小段距离
                        
                        Vector2 rightTestPos = aiPos;
                        rightTestPos.x += 30.0f; // 测试右移一小段距离
                        
                        int currentProb = predictSunflowerHitProbability(aiPos, playerPos, playerIsOnLeft, targetVelocityEstimate);
                        int leftProb = predictSunflowerHitProbability(leftTestPos, playerPos, playerIsOnLeft, targetVelocityEstimate);
                        int rightProb = predictSunflowerHitProbability(rightTestPos, playerPos, playerIsOnLeft, targetVelocityEstimate);
                        
                        // 选择更好的方向
                        if (leftProb > rightProb && leftProb > currentProb + 5) {
                            moveLeft = true;
                        } else if (rightProb > leftProb && rightProb > currentProb + 5) {
                            moveLeft = false;
                        }
                        // 如果两边都没有明显改善，可能选择停止移动
                        else if (currentProb > leftProb && currentProb > rightProb) {
                            shouldMove = false;
                        }
                    }
                    
                    if (moveLeft && !isNearLeftEdge) {
                        needToMoveCloser = true;
                        playerIsOnLeft = true; // 伪装玩家在左边，让AI向左移动
                    } else if (!isNearRightEdge) {
                        needToMoveCloser = true;
                        playerIsOnLeft = false; // 伪装玩家在右边，让AI向右移动
                    }
                } else {
                    shouldMove = false; // 不需要移动
                }
            } else if (beAggressive && !needToMoveAway) {
                // 如果决定主动追击，将needToMoveCloser设为true
                needToMoveCloser = true;
            }
            
            // 如果需要移动
            if (shouldMove) {
                // 如果需要拉开距离
                if (needToMoveAway) {
                    if (playerIsOnLeft) {
                        // 玩家在AI左侧，向右移动拉开距离
                        if (!isMovingRight && !isNearRightEdge) { // 添加边缘检测
                            // 如果之前没有向右移动，先释放左移键
                            if (isMovingLeft) {
                                ExMessage releaseMsg;
                                releaseMsg.message = WM_KEYUP;
                                releaseMsg.vkcode = VK_LEFT;
                                aiPlayer->on_input(releaseMsg);
                                isMovingLeft = false;
                            }
                            
                            // 开始向右移动
                            ExMessage rightMsg;
                            rightMsg.message = WM_KEYDOWN;
                            rightMsg.vkcode = VK_RIGHT;
                            aiPlayer->on_input(rightMsg);
                            isMovingRight = true;
                            
                            // 添加动作延迟，防止频繁改变决策
                            actionDelayTimer = 120; // 进一步减少延迟
                        }
                    } else {
                        // 玩家在AI右侧，向左移动拉开距离
                        if (!isMovingLeft && !isNearLeftEdge) { // 添加边缘检测
                            // 如果之前没有向左移动，先释放右移键
                            if (isMovingRight) {
                                ExMessage releaseMsg;
                                releaseMsg.message = WM_KEYUP;
                                releaseMsg.vkcode = VK_RIGHT;
                                aiPlayer->on_input(releaseMsg);
                                isMovingRight = false;
                            }
                            
                            // 开始向左移动
                            ExMessage leftMsg;
                            leftMsg.message = WM_KEYDOWN;
                            leftMsg.vkcode = VK_LEFT;
                            aiPlayer->on_input(leftMsg);
                            isMovingLeft = true;
                            
                            // 添加动作延迟
                            actionDelayTimer = 120;
                        }
                    }
                }
                // 如果需要靠近玩家
                else if (needToMoveCloser) {
                    if (playerIsOnLeft) {
                        // 玩家在AI左侧，向左移动靠近
                        if (!isMovingLeft && !isNearLeftEdge) { // 添加边缘检测
                            // 如果之前没有向左移动，先释放右移键
                            if (isMovingRight) {
                                ExMessage releaseMsg;
                                releaseMsg.message = WM_KEYUP;
                                releaseMsg.vkcode = VK_RIGHT;
                                aiPlayer->on_input(releaseMsg);
                                isMovingRight = false;
                            }
                            
                            // 开始向左移动
                            ExMessage leftMsg;
                            leftMsg.message = WM_KEYDOWN;
                            leftMsg.vkcode = VK_LEFT;
                            aiPlayer->on_input(leftMsg);
                            isMovingLeft = true;
                            
                            // 添加动作延迟
                            actionDelayTimer = 120;
                        }
                    } else {
                        // 玩家在AI右侧，向右移动靠近
                        if (!isMovingRight && !isNearRightEdge) { // 添加边缘检测
                            // 如果之前没有向右移动，先释放左移键
                            if (isMovingLeft) {
                                ExMessage releaseMsg;
                                releaseMsg.message = WM_KEYUP;
                                releaseMsg.vkcode = VK_LEFT;
                                aiPlayer->on_input(releaseMsg);
                                isMovingLeft = false;
                            }
                            
                            // 开始向右移动
                            ExMessage rightMsg;
                            rightMsg.message = WM_KEYDOWN;
                            rightMsg.vkcode = VK_RIGHT;
                            aiPlayer->on_input(rightMsg);
                            isMovingRight = true;
                            
                            // 添加动作延迟
                            actionDelayTimer = 120;
                        }
                    }
                }
            } else {
                // 如果不应该移动，确保停止所有移动
                StopMoving(aiPlayer);
            }
        }
    }
    
    // 大幅降低AI随机释放按键的概率，使AI移动更连贯
    if (aiActionTimer % 25 == 0) { // 从50降低到25，更频繁检查但更低概率释放
        Player* aiPlayer = static_cast<Player*>(controlledPlayer);
        if (!aiPlayer) return;
        
        if (isMovingLeft) {
            // 减少AI随机释放按键的概率
            std::uniform_int_distribution<> releaseDist(1, 100);
            int releaseChance = wrongMoveChance / 5; // 进一步降低错误概率
            if (releaseChance < 1) releaseChance = 1;
            
            if (releaseDist(rng) < releaseChance) {
                ExMessage releaseMsg;
                releaseMsg.message = WM_KEYUP;
                releaseMsg.vkcode = VK_LEFT;
                aiPlayer->on_input(releaseMsg);
                isMovingLeft = false;
            }
        }
        
        if (isMovingRight) {
            // 减少AI随机释放按键的概率
            std::uniform_int_distribution<> releaseDist(1, 100);
            int releaseChance = wrongMoveChance / 5; // 进一步降低错误概率
            if (releaseChance < 1) releaseChance = 1;
            
            if (releaseDist(rng) < releaseChance) {
                ExMessage releaseMsg;
                releaseMsg.message = WM_KEYUP;
                releaseMsg.vkcode = VK_RIGHT;
                aiPlayer->on_input(releaseMsg);
                isMovingRight = false;
            }
        }
    }
}

// 停止所有移动
void AIController::StopMoving(Player* aiPlayer) {
    if (isMovingLeft) {
        ExMessage releaseMsg;
        releaseMsg.message = WM_KEYUP;
        releaseMsg.vkcode = VK_LEFT;
        aiPlayer->on_input(releaseMsg);
        isMovingLeft = false;
    }
    if (isMovingRight) {
        ExMessage releaseMsg;
        releaseMsg.message = WM_KEYUP;
        releaseMsg.vkcode = VK_RIGHT;
        aiPlayer->on_input(releaseMsg);
        isMovingRight = false;
    }
}

// 强制朝向后尝试攻击，如果成功返回true
bool AIController::TryAttack(Player* aiPlayer, bool playerIsOnLeft) {
    const int MAX_ATTEMPTS = 3; // 最多尝试3次确保正确朝向
    
    // 先直接检查当前朝向是否正确
    bool shouldFaceLeft = playerIsOnLeft;
    bool isFacingLeft = aiPlayer->is_facing_left();

    // 为豌豆射手角色添加额外确认，确保射击方向正确
    if (characterType == CharacterType::PEASHOOTER) {
        // 确保完全停止移动并重置方向键状态
        StopMoving(aiPlayer);
        
        // 如果面向不正确，强制修正
        if (isFacingLeft != shouldFaceLeft) {
            // 按下正确方向键修正面向
            ExMessage dirMsg;
            dirMsg.message = WM_KEYDOWN;
            dirMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
            aiPlayer->on_input(dirMsg);
            
            // 短暂延迟确保处理完成
            for (int i = 0; i < 10; i++) {
                // 空循环模拟延迟
            }
            
            // 释放按键
            ExMessage releaseMsg;
            releaseMsg.message = WM_KEYUP;
            releaseMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
            aiPlayer->on_input(releaseMsg);
            
            // 更新面向状态
            isFacingLeft = aiPlayer->is_facing_left();
        }
    }
    
    // 如果朝向正确，直接攻击
    if (isFacingLeft == shouldFaceLeft) {
        // 直接进行攻击
        if (aiPlayer->get_mp() >= 100) {
            // 使用特殊攻击
            ExMessage exAttackMsg;
            exAttackMsg.message = WM_KEYDOWN;
            exAttackMsg.vkcode = VK_OEM_2;
            aiPlayer->on_input(exAttackMsg);
            
            // 立即释放按键，避免卡键
            ExMessage exReleaseMsg;
            exReleaseMsg.message = WM_KEYUP;
            exReleaseMsg.vkcode = VK_OEM_2;
            aiPlayer->on_input(exReleaseMsg);
        } else {
            // 使用普通攻击
            ExMessage attackMsg;
            attackMsg.message = WM_KEYDOWN;
            attackMsg.vkcode = VK_OEM_PERIOD;
            aiPlayer->on_input(attackMsg);
            
            // 立即释放按键，避免卡键
            ExMessage releaseAttackMsg;
            releaseAttackMsg.message = WM_KEYUP;
            releaseAttackMsg.vkcode = VK_OEM_PERIOD;
            aiPlayer->on_input(releaseAttackMsg);
        }
        return true;
    }
    
    // 朝向不正确时进行多次尝试
    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        // 确保所有方向键先释放
        ExMessage releaseLeftMsg;
        releaseLeftMsg.message = WM_KEYUP;
        releaseLeftMsg.vkcode = VK_LEFT;
        aiPlayer->on_input(releaseLeftMsg);
        
        ExMessage releaseRightMsg;
        releaseRightMsg.message = WM_KEYUP;
        releaseRightMsg.vkcode = VK_RIGHT;
        aiPlayer->on_input(releaseRightMsg);
        
        // 按下正确的方向键，保持更长时间
        ExMessage dirMsg;
        dirMsg.message = WM_KEYDOWN;
        dirMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
        aiPlayer->on_input(dirMsg);
        
        // 增加延迟模拟，让角色有足够时间转向
        for (int delay = 0; delay < 5; delay++) {
            // 检查朝向是否已经正确
            isFacingLeft = aiPlayer->is_facing_left();
            if (isFacingLeft == shouldFaceLeft) {
                break;
            }
        }
        
        // 短暂按下后释放
        ExMessage releaseMsg;
        releaseMsg.message = WM_KEYUP;
        releaseMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
        aiPlayer->on_input(releaseMsg);
        
        // 再次检查朝向是否正确
        isFacingLeft = aiPlayer->is_facing_left();
        if (isFacingLeft == shouldFaceLeft) {
            // 朝向正确，进行攻击
            if (aiPlayer->get_mp() >= 100) {
                // 使用特殊攻击
                ExMessage exAttackMsg;
                exAttackMsg.message = WM_KEYDOWN;
                exAttackMsg.vkcode = VK_OEM_2;
                aiPlayer->on_input(exAttackMsg);
                
                // 立即释放按键
                ExMessage exReleaseMsg;
                exReleaseMsg.message = WM_KEYUP;
                exReleaseMsg.vkcode = VK_OEM_2;
                aiPlayer->on_input(exReleaseMsg);
            } else {
                // 使用普通攻击
                ExMessage attackMsg;
                attackMsg.message = WM_KEYDOWN;
                attackMsg.vkcode = VK_OEM_PERIOD;
                aiPlayer->on_input(attackMsg);
                
                // 立即释放按键
                ExMessage releaseAttackMsg;
                releaseAttackMsg.message = WM_KEYUP;
                releaseAttackMsg.vkcode = VK_OEM_PERIOD;
                aiPlayer->on_input(releaseAttackMsg);
            }
            return true;
        }
    }
    
    // 如果多次尝试后朝向仍然不正确，强制设置朝向再攻击
    // 但这已经是对困难情况的处理，理想情况下不应该走到这里
    
    // 先尝试最后一次方向键按键
    ExMessage finalDirMsg;
    finalDirMsg.message = WM_KEYDOWN;
    finalDirMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
    aiPlayer->on_input(finalDirMsg);
    
    // 按一段时间后释放
    ExMessage finalReleaseMsg;
    finalReleaseMsg.message = WM_KEYUP;
    finalReleaseMsg.vkcode = shouldFaceLeft ? VK_LEFT : VK_RIGHT;
    aiPlayer->on_input(finalReleaseMsg);
    
    // 无论如何都进行攻击
    if (aiPlayer->get_mp() >= 100) {
        // 使用特殊攻击
        ExMessage exAttackMsg;
        exAttackMsg.message = WM_KEYDOWN;
        exAttackMsg.vkcode = VK_OEM_2;
        aiPlayer->on_input(exAttackMsg);
        
        // 立即释放按键
        ExMessage exReleaseMsg;
        exReleaseMsg.message = WM_KEYUP;
        exReleaseMsg.vkcode = VK_OEM_2;
        aiPlayer->on_input(exReleaseMsg);
    } else {
        // 使用普通攻击
        ExMessage attackMsg;
        attackMsg.message = WM_KEYDOWN;
        attackMsg.vkcode = VK_OEM_PERIOD;
        aiPlayer->on_input(attackMsg);
        
        // 立即释放按键
        ExMessage releaseAttackMsg;
        releaseAttackMsg.message = WM_KEYUP;
        releaseAttackMsg.vkcode = VK_OEM_PERIOD;
        aiPlayer->on_input(releaseAttackMsg);
    }
    
    return false; // 返回失败，表示朝向可能不正确
} 