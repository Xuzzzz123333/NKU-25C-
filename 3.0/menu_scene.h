#pragma once
#include"scene.h"
#include<iostream>
#include<graphics.h>
#include"animation.h"
#include"atlas.h"
#include"scene_manager.h"
#include"camera.h"
#include"timer.h"
#include"game_mode.h"  // 引入GameMode枚举定义
using namespace std;
extern IMAGE img_menu_background;
extern SceneManager scene_manager;

// 引用在main.cpp中定义的变量
extern Scene* selector_scene;

// 引用在main.cpp中定义的GameMode变量
extern GameMode current_game_mode;

// 添加全局变量，用于防止场景切换冲突
extern bool prevent_scene_switch;
extern int scene_switch_delay;

class MenuScene:public Scene
{
public:
	MenuScene() : selectedOption(0) {}
	
	~MenuScene() = default;
	void on_enter() 
	{
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
		// 重置场景切换保护标志
		prevent_scene_switch = false;
		scene_switch_delay = 0;
	}
	void on_update(int delta) {
		
	}
	void on_draw(const Camera &camera) 
	{
		putimage(0, 0, &img_menu_background);
		
		// 绘制菜单标题
		settextcolor(RGB(255, 255, 0)); // 改为黄色，使标题更醒目
		settextstyle(48, 0, _T("IPix"));
		outtextxy(400, 100, _T("植物明星大乱斗"));
		
		// 绘制菜单选项
		settextstyle(36, 0, _T("IPix"));
		
		// 双人对战选项
		if (selectedOption == 0) {
			settextcolor(RGB(255, 100, 100)); // 红色更醒目
		} else {
			settextcolor(RGB(0, 255, 255)); // 青色更醒目
		}
		outtextxy(400, 250, _T("双人对战"));
		
		// 人机对战选项
		if (selectedOption == 1) {
			settextcolor(RGB(255, 100, 100)); // 红色更醒目
		} else {
			settextcolor(RGB(0, 255, 255)); // 青色更醒目
		}
		outtextxy(400, 350, _T("人机对战"));
		
		// 退出游戏选项
		if (selectedOption == 2) {
			settextcolor(RGB(255, 100, 100)); // 红色更醒目
		} else {
			settextcolor(RGB(0, 255, 255)); // 青色更醒目
		}
		outtextxy(400, 450, _T("退出游戏"));
		
		// 绘制操作提示
		settextcolor(RGB(255, 200, 100)); // 橙黄色更醒目
		settextstyle(24, 0, _T("IPix"));
		outtextxy(350, 550, _T("上下选择,Enter确认"));
		
		// 显示场景切换状态提示(如果处于保护状态)
		if (prevent_scene_switch) {
			settextcolor(RGB(255, 150, 150));
			settextstyle(20, 0, _T("IPix"));
			outtextxy(500, 650, _T("请稍候..."));
		}
	}
	void on_input(const ExMessage&msg) 
	{
		if (msg.message == WM_KEYDOWN)
		{
			// 如果场景切换保护标志已设置，忽略所有输入
			if (prevent_scene_switch) {
				return;
			}
			
			switch (msg.vkcode) {
				case VK_UP:
					selectedOption = (selectedOption > 0) ? (selectedOption - 1) : 2;
					mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
					break;
				case VK_DOWN:
					selectedOption = (selectedOption < 2) ? (selectedOption + 1) : 0;
					mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);
					break;
				case VK_RETURN:
					if (!prevent_scene_switch) {
						prevent_scene_switch = true; // 设置保护标志，防止连续场景切换
						scene_switch_delay = 500;    // 设置500毫秒的延迟
						mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
						
						switch (selectedOption) {
							case 0: // 双人对战
								current_game_mode = GameMode::PVP;
								scene_manager.set_current_scene(selector_scene);
								break;
							case 1: // 人机对战
								current_game_mode = GameMode::PVE;
								scene_manager.set_current_scene(selector_scene);
								break;
							case 2: // 退出游戏
								exit(0);
								break;
						}
					}
					break;
			}
		}
	}
	void on_exit() {  }
private:
	int selectedOption;
};
