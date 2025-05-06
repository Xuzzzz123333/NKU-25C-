#pragma once
#include<graphics.h>
#include"util.h"
#include"atlas.h"
#include"scene.h"
#include"menu_scene.h"
#include"game.scene.h"
#include"scene_manager.h"
#include"selector_scene.h"
#pragma comment(lib,"Winmm.lib")


IMAGE img_VS;

//1P 文本

IMAGE img_1P;

//2P 文本

IMAGE img_2P;

//1P 键位描述

IMAGE img_1P_desc;

//2P 键位描述

IMAGE img_2P_desc;

//朝向左的墓碑

IMAGE img_gravestone_left;

//朝向右的墓碑

IMAGE img_gravestone_right;

//选角界面提示信息文本

IMAGE img_selector_tip;

//选角界面背景图

IMAGE img_selector_background;

//1P 向左选择按钮默认状态

IMAGE img_1P_selector_btn_idle_left;

//1P 向右选择按钮默认状态

IMAGE img_1P_selector_btn_idle_right;

//1P 向左选择按钮按下状态

IMAGE img_1P_selector_btn_down_left;

//1P 向右选择按钮按下状态

IMAGE img_1P_selector_btn_down_right;

//2P 向左选择按钮默认状态

IMAGE img_2P_selector_btn_idle_left;

//2P 向右选择按钮默认状态

IMAGE img_2P_selector_btn_idle_right;

//2P 向左选择按钮按下状态

IMAGE img_2P_selector_btn_down_left;

//2P 向右选择按钮按下状态

IMAGE img_2P_selector_btn_down_right;

//选角界面朝向左的豌豆射手

IMAGE img_peashooter_selector_background_left;

//选角界面朝向右的豌豆射手

IMAGE img_peashooter_selector_background_right;

//选角界面朝向左的龙日葵

IMAGE img_sunflower_selector_background_left;

//选角界面朝向右的龙日葵

IMAGE img_sunflower_selector_background_right;



//天空

IMAGE img_sky;

//山脉

IMAGE img_hills;

//大型平台

IMAGE img_platform_large;

//小型平台

IMAGE img_platform_small;



//1P 指示光标

IMAGE img_1P_cursor;

//2P 指示光标

IMAGE img_2P_cursor;



// 豌豆射手图集

//朝向左的默认动画

Atlas atlas_peashooter_idle_left;

//朝向右的默认动画

Atlas atlas_peashooter_idle_right;

//朝向左的奔跑动画

Atlas atlas_peashooter_run_left;

//朝向右的奔跑动画

Atlas atlas_peashooter_run_right;

//朝向左的特殊攻击动画

Atlas atlas_peashooter_attack_ex_left;

//朝向右的特殊攻击动画

Atlas atlas_peashooter_attack_ex_right;

//朝向左的死亡动画

Atlas atlas_peashooter_die_left;

//朝向右的死亡动画

Atlas atlas_peashooter_die_right;



//龙日葵图集

//朝向左的默认动画

Atlas atlas_sunflower_idle_left;

//朝向右的默认动画

Atlas atlas_sunflower_idle_right;

//朝向左的奔跑动画

Atlas atlas_sunflower_run_left;

//朝向右的奔跑动画

Atlas atlas_sunflower_run_right;

//朝向左的特殊攻击动画

Atlas atlas_sunflower_attack_ex_left;

//朝向右的特殊攻击动画

Atlas atlas_sunflower_attack_ex_right;

//朝向左的死亡动画

Atlas atlas_sunflower_die_left;

//朝向右的死亡动画
Atlas atlas_sunflower_die_right;
//豌豆图片
IMAGE img_pea;
//豌豆破碎动画图集
Atlas atlas_pea_break;
//日光动画图集
Atlas atlas_sun;
//日光爆炸动画图集
Atlas atlas_sun_explode;
//特殊日光动画图集
Atlas atlas_sun_ex;
//特殊日光爆炸动画图集
Atlas atlas_sun_ex_explode;
//"日"文本动画图集
Atlas atlas_sun_text;
//奔跑特效动画图集
Atlas atlas_run_effect;
//跳跃特效动画图集
Atlas atlas_jump_effect;
//落地特效动画图集
Atlas atlas_land_effect;
//1P 获胜文本图片
IMAGE img_1P_winner;
//2P 获胜文本图片
IMAGE img_2P_winner;
//获胜玩家文本背景图片
IMAGE img_winner_bar;
//豌豆射手头像图片
IMAGE img_avatar_peashooter;
//龙日葵头像图片
IMAGE img_avatar_sunflower;
IMAGE img_menu_background;
