#pragma once
#include"scene.h"
#include<iostream>
#include<graphics.h>
#include"animation.h"
#include"atlas.h"
#include"scene_manager.h"
#include"camera.h"
#include"timer.h"
using namespace std;
extern IMAGE img_menu_background;
extern SceneManager scene_manager;
class MenuScene:public Scene
{
public:
	MenuScene() = default;
	
	~MenuScene() = default;
	void on_enter() 
	{
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
	}
	void on_update(int delta) {
		
	}
	void on_draw(const Camera &camera) 
	{
		putimage(0, 0, &img_menu_background);
		
	}
	void on_input(const ExMessage&msg) 
	{
		if (msg.message == WM_KEYUP)
		{
			mciSendString(_T("play ui_confirm  from 0"), NULL, 0, NULL);
			scene_manager.switch_to(SceneManager::SceneType::Selector);
		}
		
	
	}
	void on_exit() {  }
private:
	
};
