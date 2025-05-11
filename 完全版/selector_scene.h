#pragma once
#include"scene.h"
#include"scene_manager.h"
#include"graphics.h"
#include"atlas.h"
#include"image.h"
#include"animation.h"
#include"camera.h"
#include"util.h"
#include"player.h"
#include"peashooter_player.h"
#include"sunflower_player.h"
#include"player_id.h"
#include"game_mode.h"

extern Player* player_1 ;
extern Player* player_2 ;
extern SceneManager scene_manager;
extern IMAGE* img_player_1_avatar ;
extern IMAGE* img_player_2_avatar ;
extern GameMode current_game_mode;  // 添加对当前游戏模式的引用
extern bool prevent_scene_switch;   // 添加对场景切换保护标志的引用
extern int scene_switch_delay;      // 添加对场景切换延迟计时器的引用

class SelectorScene :public Scene
{
public:
	SelectorScene() = default;
	~SelectorScene() = default;
	void on_enter()
	{
		animation_peashooter.set_atlas(&atlas_peashooter_idle_right);
		animation_sunflower.set_atlas(&atlas_sunflower_idle_right);
		animation_peashooter.set_interval(100);
		animation_sunflower.set_interval(100);
		static const int OFFSET_X = 50;
		pos_img_VS.x = (getwidth() - img_VS.getwidth()) / 2;

		pos_img_VS.y = (getheight() - img_VS.getheight()) / 2;

		pos_img_tip.x = (getwidth() - img_selector_tip.getwidth()) / 2;

		pos_img_tip.y = getheight() - 125;

		pos_img_1P.x = (getwidth() / 2 - img_1P.getwidth()) / 2 - OFFSET_X;

		pos_img_1P.y = 35;

		pos_img_2P.x = getwidth() / 2 + (getwidth() / 2 - img_2P.getwidth()) / 2 + OFFSET_X;

		pos_img_2P.y = pos_img_1P.y;

		pos_img_1P_desc.x = (getwidth() / 2 - img_1P_desc.getwidth()) / 2 - OFFSET_X;

		pos_img_1P_desc.y = getheight() - 150;

		pos_img_2P_desc.x = getwidth() / 2 + (getwidth() / 2 - img_2P_desc.getwidth()) / 2 + OFFSET_X;

		pos_img_2P_desc.y = pos_img_1P_desc.y;

		pos_img_1P_gravestone.x = (getwidth() / 2 - img_gravestone_right.getwidth()) / 2 - OFFSET_X;

		pos_img_1P_gravestone.y = pos_img_1P.y + img_1P.getheight() + 35;

		pos_img_2P_gravestone.x = getwidth() / 2 + (getwidth() / 2 - img_gravestone_left.getwidth()) / 2 + OFFSET_X;

		pos_img_2P_gravestone.y = pos_img_1P_gravestone.y;
		pos_animation_1P.x = (getwidth() / 2 - atlas_peashooter_idle_right.get_image(0)->getwidth()) / 2 - OFFSET_X;

		pos_animation_1P.y = pos_img_1P_gravestone.y + 80;

		pos_animation_2P.x = getwidth() / 2 + (getwidth() / 2 - atlas_peashooter_idle_right.get_image(0)->getwidth()) / 2 + OFFSET_X;

		pos_animation_2P.y = pos_animation_1P.y;

		pos_img_1P_name.y = pos_animation_1P.y + 155;

		pos_img_2P_name.y = pos_img_1P_name.y;
		pos_1P_selector_btn_left.x = pos_img_1P_gravestone.x - img_1P_selector_btn_idle_left.getwidth();

		pos_1P_selector_btn_left.y = pos_img_1P_gravestone.y + (img_gravestone_right.getheight() - img_1P_selector_btn_idle_left.getheight()) / 2;

		pos_1P_selector_btn_right.x = pos_img_1P_gravestone.x + img_gravestone_right.getwidth();

		pos_1P_selector_btn_right.y = pos_1P_selector_btn_left.y;

		pos_2P_selector_btn_left.x = pos_img_2P_gravestone.x - img_2P_selector_btn_idle_left.getwidth();

		pos_2P_selector_btn_left.y = pos_1P_selector_btn_left.y;

		pos_2P_selector_btn_right.x = pos_img_2P_gravestone.x + img_gravestone_left.getwidth();

		pos_2P_selector_btn_right.y = pos_1P_selector_btn_left.y;
		
		// 重置场景切换保护标志，允许从角色选择界面切换到游戏界面
		prevent_scene_switch = false;
		scene_switch_delay = 0;
		
		// 确保从菜单场景中接收到Enter键时不会立即触发场景切换
		enterKeyBlocked = true;
		enterKeyBlockTimer = 300; // 300毫秒内不响应Enter键
	}
	void on_update(int delta) {
		animation_peashooter.on_update(delta);

		animation_sunflower.on_update(delta);
		selector_background_scorll_offset_x += 5;
		if (selector_background_scorll_offset_x >= img_peashooter_selector_background_left.getwidth())
			selector_background_scorll_offset_x = 0;
			
		// 更新Enter键阻塞计时器
		if (enterKeyBlocked && enterKeyBlockTimer > 0) {
			enterKeyBlockTimer -= delta;
			if (enterKeyBlockTimer <= 0) {
				enterKeyBlocked = false;
			}
		}
	}
	void on_draw(const Camera& camera)
	{
		IMAGE* img_p1_selector_background = nullptr;
		IMAGE* img_p2_selector_background = nullptr;
		switch (player_type_2)
		{
		case PlayerType::Peashooter:
			img_p1_selector_background = &img_peashooter_selector_background_right;
			break;
		case PlayerType::Sunflower:
			img_p1_selector_background = &img_sunflower_selector_background_right;
			break;
		default:
			img_p1_selector_background = &img_peashooter_selector_background_right;
			break;
		}
		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			img_p2_selector_background = &img_peashooter_selector_background_left;
			break;
		case PlayerType::Sunflower:
			img_p2_selector_background = &img_sunflower_selector_background_left;
			break;
		default:
			img_p2_selector_background = &img_peashooter_selector_background_left;
			break;
		}
	
		putimage(0, 0, &img_selector_background); 
		putimage_alpha(selector_background_scorll_offset_x - img_p1_selector_background->getwidth(), 0, img_p1_selector_background);
		putimage_alpha(selector_background_scorll_offset_x, 0, img_p1_selector_background->getwidth() - selector_background_scorll_offset_x, 0, img_p1_selector_background,0,0);
		putimage_alpha(getwidth() - img_p2_selector_background->getwidth(), 0, img_p2_selector_background->getwidth() - selector_background_scorll_offset_x, 0, img_p2_selector_background, selector_background_scorll_offset_x, 0);
		putimage_alpha(getwidth() - selector_background_scorll_offset_x, 0, img_p2_selector_background);

		// 添加"植物明星大乱斗"标题
		settextcolor(RGB(255, 255, 0)); // 醒目的黄色
		settextstyle(48, 0, _T("IPix"));
		TCHAR titleText[] = _T("植物明星大乱斗");
		int textWidth = textwidth(titleText);
		int textX = (getwidth() - textWidth) / 2;
		outtextxy(textX, 10, titleText);
		
		// 恢复后续绘制的字体大小
		settextstyle(28, 0, _T("IPix"));

		putimage_alpha(pos_img_VS.x, pos_img_VS.y, &img_VS); putimage_alpha(pos_img_1P.x, pos_img_1P.y, &img_1P);

		putimage_alpha(pos_img_2P.x, pos_img_2P.y, &img_2P);

		putimage_alpha(pos_img_1P_gravestone.x, pos_img_1P_gravestone.y, &img_gravestone_right);

		putimage_alpha(pos_img_2P_gravestone.x, pos_img_2P_gravestone.y, &img_gravestone_left);
		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			animation_peashooter.on_draw(camera, pos_animation_1P.x, pos_animation_1P.y);
			pos_img_1P_name.x = pos_img_1P_gravestone.x + (img_gravestone_right.getwidth() - textwidth(str_peashooter_name)) / 2;
			outtextxy_shaded(pos_img_1P_name.x, pos_img_1P_name.y, str_peashooter_name);
			break;
		case PlayerType::Sunflower:
			animation_sunflower.on_draw(camera, pos_animation_1P.x, pos_animation_1P.y);
			pos_img_1P_name.x = pos_img_1P_gravestone.x + (img_gravestone_right.getwidth() - textwidth(str_sunflower_name)) / 2;
			outtextxy_shaded(pos_img_1P_name.x, pos_img_1P_name.y, str_sunflower_name);
			break;
		}

		switch (player_type_2)
		{
		case PlayerType::Peashooter:
			animation_peashooter.on_draw(camera, pos_animation_2P.x, pos_animation_2P.y);
			pos_img_2P_name.x = pos_img_2P_gravestone.x + (img_gravestone_left.getwidth() - textwidth(str_peashooter_name)) / 2;
			outtextxy_shaded(pos_img_2P_name.x, pos_img_2P_name.y, str_peashooter_name);
			break;
		case PlayerType::Sunflower:
			animation_sunflower.on_draw(camera, pos_animation_2P.x, pos_animation_2P.y);
			pos_img_2P_name.x = pos_img_2P_gravestone.x + (img_gravestone_left.getwidth() - textwidth(str_sunflower_name)) / 2;
			outtextxy_shaded(pos_img_2P_name.x, pos_img_2P_name.y, str_sunflower_name);
			break;
		}
		putimage_alpha(pos_1P_selector_btn_left.x, pos_1P_selector_btn_left.y, is_btn_1P_left_down ? &img_1P_selector_btn_down_left : &img_1P_selector_btn_idle_left);
		putimage_alpha(pos_1P_selector_btn_right.x, pos_1P_selector_btn_right.y, is_btn_1P_right_down ? &img_1P_selector_btn_down_right : &img_1P_selector_btn_idle_right);
		
		// 在PVE模式下，显示提示信息，并允许玩家选择AI角色类型
		if (current_game_mode == GameMode::PVE) {
			// 在PVE模式下不显示左右方向键，但显示切换AI角色的提示
			putimage_alpha(pos_2P_selector_btn_left.x, pos_2P_selector_btn_left.y, &img_2P_selector_btn_idle_left);
			putimage_alpha(pos_2P_selector_btn_right.x, pos_2P_selector_btn_right.y, &img_2P_selector_btn_idle_right);
			
			// 在2P部分显示"AI控制"的提示
			settextcolor(RGB(255, 100, 100));
			settextstyle(28, 0, _T("IPix"));
			TCHAR aiControlText[] = _T("AI控制");
			int textWidth = textwidth(aiControlText);
			int textX = pos_img_2P_gravestone.x + (img_gravestone_left.getwidth() - textWidth) / 2;
			outtextxy(textX, pos_img_2P_gravestone.y - 40, aiControlText);
			
			// 显示TAB键切换AI角色的提示
			settextcolor(RGB(255, 255, 150));
			settextstyle(24, 0, _T("IPix"));
			TCHAR tabSwitchText[] = _T("按TAB键切换AI角色");
			textWidth = textwidth(tabSwitchText);
			textX = pos_img_2P_gravestone.x + (img_gravestone_left.getwidth() - textWidth) / 2;
			outtextxy(textX, pos_img_2P_gravestone.y + img_gravestone_left.getheight() + 20, tabSwitchText);
			
			// 新增：显示F1/F2/F3调整难度的提示
			settextcolor(RGB(150, 255, 150));
			settextstyle(22, 0, _T("IPix"));
			TCHAR difficultyText[] = _T("游戏中按F1/F2/F3调整AI难度");
			textWidth = textwidth(difficultyText);
			textX = pos_img_2P_gravestone.x + (img_gravestone_left.getwidth() - textWidth) / 2;
			outtextxy(textX, pos_img_2P_gravestone.y + img_gravestone_left.getheight() + 50, difficultyText);
			
			// 突出显示当前是AI控制模式的视觉效果
			setlinecolor(RGB(255, 100, 100));
			setlinestyle(PS_SOLID, 3);
			rectangle(pos_img_2P_gravestone.x - 10, pos_img_2P_gravestone.y - 50, 
					 pos_img_2P_gravestone.x + img_gravestone_left.getwidth() + 10, 
					 pos_img_2P_gravestone.y + img_gravestone_left.getheight() + 80);
		} else {
			putimage_alpha(pos_2P_selector_btn_left.x, pos_2P_selector_btn_left.y, is_btn_2P_left_down ? &img_2P_selector_btn_down_left : &img_2P_selector_btn_idle_left);
			putimage_alpha(pos_2P_selector_btn_right.x, pos_2P_selector_btn_right.y, is_btn_2P_right_down ? &img_2P_selector_btn_down_right : &img_2P_selector_btn_idle_right);
		}
		
		putimage_alpha(pos_img_1P_desc.x, pos_img_1P_desc.y, &img_1P_desc);
		putimage_alpha(pos_img_2P_desc.x, pos_img_2P_desc.y, &img_2P_desc); putimage_alpha(pos_img_tip.x, pos_img_tip.y, &img_selector_tip);

		// 在PVE模式下，显示额外提示
		if (current_game_mode == GameMode::PVE) {
			settextcolor(RGB(255, 255, 0)); // 改为黄色，更醒目
			settextstyle(24, 0, _T("IPix"));
			TCHAR pveText[] = _T("人机对战模式：请选择您的角色");
			int textWidth = textwidth(pveText);
			int textX = (getwidth() - textWidth) / 2;
			outtextxy(textX, pos_img_tip.y - 40, pveText);
		}
		
		// 显示场景切换状态提示(如果处于保护状态)
		if (prevent_scene_switch) {
			settextcolor(RGB(255, 150, 150));
			settextstyle(20, 0, _T("IPix"));
			outtextxy(500, 650, _T("请稍候..."));
		}
		
		// 如果Enter键被阻塞，显示提示
		if (enterKeyBlocked) {
			settextcolor(RGB(200, 200, 255));
			settextstyle(20, 0, _T("IPix"));
			outtextxy(500, 600, _T("请稍等片刻..."));
		}
	}
	void on_input(const ExMessage& msg)
	{
		// 如果场景切换保护标志已设置，忽略所有输入
		if (prevent_scene_switch) {
			return;
		}
	
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case 0x41:
				is_btn_1P_left_down = 1;
				break;
			case 0x44:
				is_btn_1P_right_down = 1;
				break;
			case VK_LEFT:
				// 在PVP模式下才允许按方向键控制2P角色选择
				if (current_game_mode == GameMode::PVP) {
					is_btn_2P_left_down = 1;
				}
				break;
			case VK_RIGHT:
				// 在PVP模式下才允许按方向键控制2P角色选择
				if (current_game_mode == GameMode::PVP) {
					is_btn_2P_right_down = 1;
				}
				break;
			// 在PVE模式下添加TAB键切换AI角色类型
			case VK_TAB:
				if (current_game_mode == GameMode::PVE) {
					player_type_2 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_2 + 1) % (int)PlayerType::Invalid);
					mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL); // 播放音效
				}
				break;
			default:
				break;
			}
		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case 0x41:
				is_btn_1P_left_down = 0;
				player_type_1 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_1 - 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);//播放音效
				break;
			case 0x44:
				is_btn_1P_right_down = 0;
				player_type_1 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_1 + 1) % (int)PlayerType::Invalid);
				mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);//播放音效
				break;
			case VK_LEFT:
				if (current_game_mode == GameMode::PVP) {
					is_btn_2P_left_down = 0;
					player_type_2 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_2 - 1) % (int)PlayerType::Invalid);
					mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);//播放音效
				}
				break;
			case VK_RIGHT:
				if (current_game_mode == GameMode::PVP) {
					is_btn_2P_right_down = 0;
					player_type_2 = (PlayerType)(((int)PlayerType::Invalid + (int)player_type_2 + 1) % (int)PlayerType::Invalid);
					mciSendString(_T("play ui_switch from 0"), NULL, 0, NULL);//播放音效
				}
				break;
			case VK_RETURN:
				// 检查Enter键是否被阻塞
				if (enterKeyBlocked) {
					break; // 如果被阻塞，忽略此次按键
				}
				
				// 添加场景切换保护，防止连续切换
				if (!prevent_scene_switch) {
					prevent_scene_switch = true;
					scene_switch_delay = 500; // 设置500毫秒的延迟
					scene_manager.switch_to(SceneManager::SceneType::Game);
					mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
				}
				break;
			default:
				break;
			}

		}


	}
	void on_exit() 
	{  
		switch (player_type_1)
		{
		case PlayerType::Peashooter:
			player_1 = new PeashooterPlayer();
			img_player_1_avatar = &img_avatar_peashooter;
			break;
		case PlayerType::Sunflower:
			player_1 = new SunflowerPlayer();
			img_player_1_avatar = &img_avatar_sunflower;
			break;
	     }
		player_1->set_id(PlayerID::P1);
		
		// 根据游戏模式设置player_2
		if (current_game_mode == GameMode::PVE) {
			// 在人机对战模式下，使用玩家通过TAB键选择的AI角色类型
			switch (player_type_2)
			{
			case PlayerType::Peashooter:
				player_2 = new PeashooterPlayer(0);
				img_player_2_avatar = &img_avatar_peashooter;
				break;
			case PlayerType::Sunflower:
				player_2 = new SunflowerPlayer(0);
				img_player_2_avatar = &img_avatar_sunflower;
				break;
			}
		} else {
			// 在玩家对战模式下，使用玩家选择的角色
			switch (player_type_2)
			{
			case PlayerType::Peashooter:
				player_2 = new PeashooterPlayer(0);
				img_player_2_avatar = &img_avatar_peashooter;
				break;
			case PlayerType::Sunflower:
				player_2 = new SunflowerPlayer(0);
				img_player_2_avatar = &img_avatar_sunflower;
				break;
			}
		}
		player_2->set_id(PlayerID::P2);
		mciSendString(_T("stop bgm_menu"), NULL, 0, NULL);
	}
private:
	enum class PlayerType
	{
		Peashooter=0,
		Sunflower,
		Invalid//閺冪姵鏅?
	};
	POINT pos_img_VS = { 0 };//VS閼圭儤婀崇€涙娴橀悧鍥︾秴缂?

	POINT pos_img_tip = { 0 };//閹绘劗銇氭穱鈩冧紖閺傚洦婀伴崶鍓у娴ｅ秶鐤?

	POINT pos_img_1P = { 0 };//1P 閺傚洦婀伴崶鍓у娴ｅ秶鐤?

	POINT pos_img_2P = { 0 };//2P 閺傚洦婀伴崶鍓у娴ｅ秶鐤?

	POINT pos_img_1P_desc = { 0 };//1P 闁款喕缍呴幓蹇氬牚閸ュ墽澧栨担宥囩枂

	POINT pos_img_2P_desc = { 0 };//2P 闁款喕缍呴幓蹇氬牚閸ュ墽澧栨担宥囩枂

	POINT pos_img_1P_name = { 0 };//1P 鐟欐帟澹婃慨鎾虫倳閸ュ墽澧栨担宥囩枂

	POINT pos_img_2P_name = { 0 };//2P 鐟欐帟澹婃慨鎾虫倳閸ュ墽澧栨担宥囩枂

	POINT pos_animation_1P = { 0 };//1P 閸斻劎鏁炬担宥囩枂

	POINT pos_animation_2P = { 0 };//2P 閸斻劎鏁炬担宥囩枂

	POINT pos_img_1P_gravestone = { 0 };//1P 婢ф挾顣堕崶鍓у娴ｅ秶鐤?

	POINT pos_img_2P_gravestone = { 0 };//2P 婢ф挾顣堕崶鍓у娴ｅ秶鐤?

	POINT pos_1P_selector_btn_left = { 0 };//1P 閸氭垵涔忛崚鍥ㄥ床閹稿鎸虫担宥囩枂

	POINT pos_2P_selector_btn_left = { 0 };//2P 閸氭垵涔忛崚鍥ㄥ床閹稿鎸虫担宥囩枂

	POINT pos_1P_selector_btn_right = { 0 };//1P 閸氭垵褰搁崚鍥ㄥ床閹稿鎸虫担宥囩枂

	POINT pos_2P_selector_btn_right = { 0 };//2P 閸氭垵褰搁崚鍥ㄥ床閹稿鎸虫担宥囩枂
	Animation animation_peashooter;
	Animation animation_sunflower;
	PlayerType player_type_1 = PlayerType::Peashooter;
	PlayerType player_type_2 = PlayerType::Sunflower;
	LPCTSTR str_peashooter_name = _T("豌豆射手");
	LPCTSTR str_sunflower_name = _T("向日葵");
	int selector_background_scorll_offset_x = 0;//閼冲本娅欏姘З鐠烘繄顬?
	bool is_btn_1P_left_down = false;
	bool is_btn_1P_right_down = false;//閸氭垵涔忛幋鏍ф倻閸欒櫕瀵滈柨顔芥Ц閸氾箒顫﹂幐澶夌瑓
	bool is_btn_2P_left_down = false;
	bool is_btn_2P_right_down = false;
	
	// 用于阻止刚进入场景时Enter键的误触发
	bool enterKeyBlocked = true;
	int enterKeyBlockTimer = 0;
	
	void outtextxy_shaded(int x, int y, LPCTSTR str)//缂佹ê鍩楅梼鏉戝閺佸牊鐏?
	{
		settextcolor(RGB(50, 50, 50));
		outtextxy(x + 2, y + 2, str);
		settextcolor(RGB(0, 255, 255)); // 改为青色，更醒目
		outtextxy(x, y, str);
	}
};