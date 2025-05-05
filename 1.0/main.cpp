#include<graphics.h>
#include<vector>
#include"util.h"
#include"atlas.h"
#include"scene.h"
#include"menu_scene.h"
#include"game.scene.h"
#include"scene_manager.h"
#include"selector_scene.h"
#pragma comment(lib,"Winmm.lib")
#include"player.h"
#include"image.h"
#include"platform.h"
#include"bullet.h"


bool is_debug = false;//ģʽ
Scene* menu_scene = nullptr;
Scene * game_scene = nullptr;
Scene* selector_scene = nullptr;
Scene* network_menu_scene = nullptr;
Camera main_camera;
SceneManager scene_manager;
vector<Platform>platform_list;
Player* player_1 = nullptr;
Player* player_2 = nullptr;
vector<Bullet*>bullet_list;
IMAGE* img_player_1_avatar = nullptr;
IMAGE* img_player_2_avatar = nullptr;

void flip_atlas(Atlas& src, Atlas& dst)
{
	dst.clear();
	for (int i = 0; i < src.get_size(); i++)
	{
		IMAGE img_flpipped;
		flip_image(src.get_image(i), &img_flpipped);
		dst.add_image(img_flpipped);
	}
}
void load_game_resources()
{
	
}
int main()
{
	ExMessage msg;
	const int FPS = 60;
	load_game_resources();
	initgraph(1280, 720, EW_SHOWCONSOLE);
	settextstyle(28, 0, _T("IPix"));
	setbkmode(TRANSPARENT);
	BeginBatchDraw();
	menu_scene = new MenuScene();
	game_scene = new GameScene();
	selector_scene = new SelectorScene();
	scene_manager.set_current_scene(menu_scene);
	while (1)
	{
		DWORD frame_start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			scene_manager.on_input(msg);
		}
		static DWORD last_tick_time = GetTickCount();
		DWORD current_tick_time = GetTickCount();
		DWORD delta_tick = current_tick_time - last_tick_time;
		scene_manager.on_update(delta_tick);
		last_tick_time = current_tick_time;
		cleardevice();
		scene_manager.on_draw(main_camera);
		FlushBatchDraw();
		DWORD frame_end_time = GetTickCount();
		DWORD frame_delta_time = frame_end_time - frame_start_time;
		if (frame_delta_time < 1000 / FPS)
			Sleep(1000 / FPS - frame_delta_time);
	}
	EndBatchDraw();
	delete menu_scene;
	delete game_scene;
	delete selector_scene;
	return 0;
}












