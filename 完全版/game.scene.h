#pragma once
#include"scene.h"
#include<iostream>
#include<graphics.h>
#include"scene_manager.h"
#include"atlas.h"
#include"camera.h"
#include"util.h"
#include"animation.h"
#include"platform.h"
#include"player.h"
#include"peashooter_player.h"
#include"sunflower_player.h"
#include"status_bar.h"
#include"ai_controller.h"
#include"game_mode.h"

extern IMAGE* img_player_1_avatar;
extern IMAGE* img_player_2_avatar;
using namespace std;
extern IMAGE img_sky;
extern IMAGE img_hills;
extern IMAGE img_platform_large;
extern IMAGE img_platform_small;
extern SceneManager scene_manager;
extern Camera main_camera;
extern vector<Platform>platform_list;
extern Player* player_1;
extern Player* player_2;
extern IMAGE img_1P_winner;
extern IMAGE img_2P_winner;
extern IMAGE img_winner_bar;
extern Scene* menu_scene;
extern vector<Bullet*>bullet_list;
extern bool is_debug;
extern GameMode current_game_mode;  // 添加对当前游戏模式的引用
extern bool prevent_scene_switch;   // 添加对场景切换保护标志的引用
extern int scene_switch_delay;      // 添加对场景切换延迟计时器的引用

class GameScene :public Scene
{
private:
	// AI控制器和难度
	AIController* aiController;
	AIDifficulty aiDifficulty;
	
	// 暂停菜单相关
	bool isPaused;         // 游戏是否处于暂停状态
	Timer pauseMenuTimer;  // 暂停菜单动画计时器
	int pauseMenuOption;   // 当前选择的暂停菜单选项
	
	// 游戏状态变量
	int is_game_over;
	int is_slide_out_started;
	Vector2 pos_img_winner_bar;
	Vector2 pos_img_winner_text;
	int pos_x_img_winner_bar_dst;
	int pos_x_img_winner_text_dst;
	Vector2 pos_img_sky;
	Vector2 pos_img_hills;
	const float speed_winner_bar = 0.3f;
	const float speed_winner_text = 0.4f;
	Timer timer_winner_slide_in;
	Timer timer_winner_slide_out;
	StatusBar status_bar_1P;
	StatusBar status_bar_2P;
	
	// 帮助提示显示状态
	bool showHelp;
	Timer helpTimer;
	
	// 游戏开始倒计时
	bool showCountdown;
	int countdownValue;
	Timer countdownTimer;
	
	// PVE模式准备时间倒计时
	bool showPvePreparation;
	int pvePreparationTime;
	bool aiActive;              // 标记AI是否处于活动状态
	Timer pvePreparationTimer;
	bool flashAIDifficultyText; // 用于闪烁AI难度选择提示
	Timer flashTimer;           // 控制闪烁计时器

	// 帮助相关方法
	void drawHelpInPauseMenu(); // 在暂停菜单中绘制帮助信息

public:
	GameScene() : aiController(nullptr), aiDifficulty(AIDifficulty::NORMAL),
				 is_game_over(0), is_slide_out_started(0), showHelp(true),
				 showCountdown(true), countdownValue(3), 
				 showPvePreparation(false), pvePreparationTime(5), aiActive(false),
				 flashAIDifficultyText(true), isPaused(false), pauseMenuOption(0) {}
	~GameScene() {
		if (aiController) {
			delete aiController;
			aiController = nullptr;
		}
	}
	void on_enter() 
	{
		is_game_over = 0;
		is_slide_out_started = 0;
		pos_img_winner_bar.x = static_cast<float>(-img_winner_bar.getwidth());
		pos_img_winner_bar.y = static_cast<float>((getheight()-img_winner_bar.getheight()) / 2);
		pos_x_img_winner_bar_dst = (getwidth() - img_winner_bar.getwidth()) / 2;
		pos_img_winner_text.x = pos_img_winner_bar.x;
		pos_img_winner_text.y = static_cast<float>((getheight() - img_1P_winner.getheight()) / 2);
		pos_x_img_winner_text_dst = (getwidth() - img_1P_winner.getwidth()) / 2;

		timer_winner_slide_in.restart();
		timer_winner_slide_in.set_wait_time(2500);
		timer_winner_slide_in.set_one_shot(1);
		timer_winner_slide_in.set_callback([&]()
			{
				is_slide_out_started = 1;
			});
		timer_winner_slide_out.restart();
		timer_winner_slide_out.set_wait_time(1000);
		timer_winner_slide_out.set_one_shot(1);
		timer_winner_slide_out.set_callback([&]()
			{
				// 设置场景切换保护和延迟，确保返回菜单场景不会立即触发其他场景切换
				prevent_scene_switch = true;
				scene_switch_delay = 300;
				scene_manager.set_current_scene(menu_scene);
			});
		status_bar_1P.set_avatar(img_player_1_avatar);
		status_bar_2P.set_avatar(img_player_2_avatar);
		status_bar_1P.set_position(235, 625);
		status_bar_2P.set_position(675, 625);

		player_1->set_position(200, 50);
		player_2->set_position(975, 50);
		pos_img_sky.x = static_cast<float>((getwidth() - img_sky.getwidth()) / 2);
		pos_img_sky.y = static_cast<float>((getheight() - img_sky.getheight()) / 2);
		pos_img_hills.x = static_cast<float>((getwidth() - img_hills.getwidth()) / 2);
		pos_img_hills.y = static_cast<float>((getheight() - img_hills.getheight()) / 2);
		platform_list.resize(4);
		Platform& large_platform = platform_list[0];
		large_platform.img = &img_platform_large;
		large_platform.render_positon.x = 122;
		large_platform.render_positon.y= 455;
		large_platform.shape.left = static_cast<float>(large_platform.render_positon.x) + 30.0f;
		large_platform.shape.right = static_cast<float>(large_platform.render_positon.x) + static_cast<float>(img_platform_large.getwidth()) - 30.0f;
		large_platform.shape.y = static_cast<float>(large_platform.render_positon.y) + 60.0f;
		Platform& small_platform_1 = platform_list[1];
		small_platform_1.img = &img_platform_small;
		small_platform_1.render_positon.x = 175;
		small_platform_1.render_positon.y = 360;
		small_platform_1.shape.left = static_cast<float>(small_platform_1.render_positon.x) + 40.0f;
		small_platform_1.shape.right = static_cast<float>(small_platform_1.render_positon.x) + static_cast<float>(img_platform_small.getwidth()) - 40.0f;
		small_platform_1.shape.y = static_cast<float>(small_platform_1.render_positon.y) + static_cast<float>(img_platform_small.getheight()) / 2.0f;
		Platform& small_platform_2 = platform_list[2];
		small_platform_2.img = &img_platform_small;
		small_platform_2.render_positon.x = 855;
		small_platform_2.render_positon.y = 360;
		small_platform_2.shape.left = static_cast<float>(small_platform_2.render_positon.x) + 40.0f;
		small_platform_2.shape.right = static_cast<float>(small_platform_2.render_positon.x) + static_cast<float>(img_platform_small.getwidth()) - 40.0f;
		small_platform_2.shape.y = static_cast<float>(small_platform_2.render_positon.y) + static_cast<float>(img_platform_small.getheight()) / 2.0f;
		Platform& small_platform_3 = platform_list[3];
		small_platform_3.img = &img_platform_small;
		small_platform_3.render_positon.x = 515;
		small_platform_3.render_positon.y = 225;
		small_platform_3.shape.left = static_cast<float>(small_platform_3.render_positon.x) + 40.0f;
		small_platform_3.shape.right = static_cast<float>(small_platform_3.render_positon.x) + static_cast<float>(img_platform_small.getwidth()) - 40.0f;
		small_platform_3.shape.y = static_cast<float>(small_platform_3.render_positon.y) + static_cast<float>(img_platform_small.getheight()) / 2.0f;
		mciSendString(_T("play bgm_game repeat from 0"), NULL, 0, NULL);

		// 设置帮助提示在游戏开始后显示10秒钟
		showHelp = true;
		helpTimer.restart();
		helpTimer.set_wait_time(10000); // 10秒
		helpTimer.set_one_shot(1);
		helpTimer.set_callback([&]()
			{
				showHelp = false;
			});

		// 清理之前的AI控制器
		if (aiController) {
			delete aiController;
			aiController = nullptr;
		}
		
		// 只在PVE模式下初始化AI控制器
		if (current_game_mode == GameMode::PVE) {
			aiController = new AIController(player_2, player_1, aiDifficulty);
			
			// 设置AI的角色类型
			// 通过检查对象类型，设置正确的角色类型枚举值
			if (dynamic_cast<SunflowerPlayer*>(player_2) != nullptr) {
				aiController->setControlledCharacterType(CharacterType::SUNFLOWER);
			} else if (dynamic_cast<PeashooterPlayer*>(player_2) != nullptr) {
				aiController->setControlledCharacterType(CharacterType::PEASHOOTER);
			}
			
			// 在PVE模式下初始化准备时间倒计时
			showPvePreparation = true;
			pvePreparationTime = 5;
			aiActive = false; // 初始时AI不活动
			
			pvePreparationTimer.restart();
			pvePreparationTimer.set_wait_time(1000); // 每秒更新一次
			pvePreparationTimer.set_one_shot(0);     // 不是一次性的
			pvePreparationTimer.set_callback([&]()
				{
					pvePreparationTime--;
					if (pvePreparationTime <= 0) {
						showPvePreparation = false;
						aiActive = true; // 准备时间结束后激活AI
					}
				});
			
			// 设置AI难度选择提示闪烁计时器
			flashAIDifficultyText = true;
			flashTimer.restart();
			flashTimer.set_wait_time(500); // 每0.5秒闪烁一次
			flashTimer.set_one_shot(0);    // 不是一次性的
			flashTimer.set_callback([&]()
				{
					flashAIDifficultyText = !flashAIDifficultyText;
				});
		} else {
			// 非PVE模式下初始化常规倒计时
			showCountdown = true;
			countdownValue = 3;
			aiActive = true; // 非PVE模式直接激活
		}
		
		// 初始化常规倒计时计时器
		countdownTimer.restart();
		countdownTimer.set_wait_time(1000); // 每秒更新一次
		countdownTimer.set_one_shot(0); // 不是一次性的
		countdownTimer.set_callback([&]()
			{
				countdownValue--;
				if (countdownValue <= 0) {
					showCountdown = false;
				}
			});
			
		// 重置场景切换保护标志
		prevent_scene_switch = false;
		scene_switch_delay = 0;

		// 如果是PVE模式，额外设置准备阶段AI角色临时无敌
		if (current_game_mode == GameMode::PVE) {
			// 重置玩家位置确保公平开局
			player_1->set_position(200, 50);
			player_2->set_position(975, 50);
			
			// 确保玩家不能在准备时间内造成伤害
			player_2->make_invulnerable(); // 使AI角色在准备时间内无敌
		}
	}
	void on_update(int delta)
	{
		player_1->on_update(delta);
		
		// 如果是人机对战模式，且准备时间已结束，则激活AI
		if (current_game_mode == GameMode::PVE && aiController) {
			// 准备时间结束时，移除AI的无敌状态
			if (showPvePreparation && pvePreparationTime <= 0) {
				showPvePreparation = false;
				aiActive = true;
			}
			
			// 只有在AI已激活的情况下才更新AI控制器
			if (aiActive) {
				aiController->update(delta);
			}
		}
		
		player_2->on_update(delta);
		main_camera.on_update(delta);
		bullet_list.erase(remove_if(bullet_list.begin(), bullet_list.end(), [](const Bullet* bullet)
			{
				bool deletable = bullet->check_can_remove();
				if (deletable)delete bullet;
				return deletable;
			}), bullet_list.end());
		for (Bullet* bullet : bullet_list)
			bullet->on_update(delta);
		const Vector2& position_player_1 = player_1->get_position();
		const Vector2& position_player_2 = player_2->get_position();
		if (position_player_1.y >= getheight())
		{
			mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
			mciSendString(_T("play fall from 0"), NULL, 0, NULL);
			player_1->set_hp(0);
		}
		if (position_player_2.y >= getheight())
		{
			mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
			mciSendString(_T("play fall from 0"), NULL, 0, NULL);
			player_2->set_hp(0);
		}
		if (player_1->get_hp() <= 0 || player_2->get_hp() <= 0)
		{
			if (!is_game_over)
			{
				mciSendString(_T("stop fall"), NULL, 0, NULL);
				mciSendString(_T("play ui_win from 0"), NULL, 0, NULL);
			}
			is_game_over = 1;
		}
		status_bar_1P.set_hp(player_1->get_hp());
		status_bar_1P.set_mp(player_1->get_mp());
		status_bar_2P.set_hp(player_2->get_hp());
		status_bar_2P.set_mp(player_2->get_mp());
		if (is_game_over)
		{
			pos_img_winner_bar.x += static_cast<float>(speed_winner_bar * delta);
			pos_img_winner_text.x += static_cast<float>(speed_winner_text * delta);
			if (!is_slide_out_started)
			{
				timer_winner_slide_in.on_update(delta);
				if (pos_img_winner_bar.x > static_cast<float>(pos_x_img_winner_bar_dst))
					pos_img_winner_bar.x = static_cast<float>(pos_x_img_winner_bar_dst);
				if (pos_img_winner_text.x > static_cast<float>(pos_x_img_winner_text_dst))
					pos_img_winner_text.x = static_cast<float>(pos_x_img_winner_text_dst);
			}
			else
				timer_winner_slide_out.on_update(delta);
		}
		
		// 更新帮助提示计时器
		if (showHelp) {
			helpTimer.on_update(delta);
		}
		
		// 更新常规倒计时计时器
		if (showCountdown) {
			countdownTimer.on_update(delta);
		}
		
		// 更新PVE准备时间倒计时计时器
		if (showPvePreparation) {
			pvePreparationTimer.on_update(delta);
			flashTimer.on_update(delta);
		}
	}
	void on_draw(const Camera &camera) {
		putimage_alpha(camera, static_cast<int>(pos_img_sky.x), static_cast<int>(pos_img_sky.y), &img_sky);
		putimage_alpha(camera, static_cast<int>(pos_img_hills.x), static_cast<int>(pos_img_hills.y), &img_hills);
		for (const Platform& platform : platform_list)
			platform.on_draw(camera);
		if (is_debug)
		{
			settextcolor(RGB(255, 0, 0));
			outtextxy(15, 15, _T("调试模式开启 按'Q'键关闭"));
		}
		player_1->on_draw(camera);
		// PVE模式下为玩家1添加红色指示器
		if (current_game_mode == GameMode::PVE && !is_game_over) {
			const Vector2& pos = player_1->get_position();
			const Vector2& size = player_1->get_size();
			int indicator_x = static_cast<int>(pos.x + size.x / 2 - 20 - camera.get_position().x);
			int indicator_y = static_cast<int>(pos.y - 35 - camera.get_position().y);
			settextcolor(RGB(255, 0, 0));
			settextstyle(28, 0, _T("IPix"));
			outtextxy(indicator_x, indicator_y, _T("你"));
			// 向下箭头
			setlinecolor(RGB(255, 0, 0));
			setlinestyle(PS_SOLID, 3);
			line(indicator_x + 15, indicator_y + 28, indicator_x + 15, indicator_y + 45);
			line(indicator_x + 10, indicator_y + 40, indicator_x + 15, indicator_y + 45);
			line(indicator_x + 20, indicator_y + 40, indicator_x + 15, indicator_y + 45);
		}
		player_2->on_draw(camera);
		for (const Bullet* bullet : bullet_list)
			bullet->on_draw(camera);
		if (is_game_over)
		{
			putimage_alpha(static_cast<int>(pos_img_winner_bar.x), static_cast<int>(pos_img_winner_bar.y), &img_winner_bar);
			putimage_alpha(static_cast<int>(pos_img_winner_text.x), static_cast<int>(pos_img_winner_text.y), player_1->get_hp()>0?&img_1P_winner:&img_2P_winner);
		}
		else
		{
			status_bar_1P.on_draw();
			status_bar_2P.on_draw();
		}

		// 只在PVE模式下显示AI难度级别
		if (current_game_mode == GameMode::PVE) {
			settextcolor(WHITE);
			settextstyle(20, 0, _T("IPix"));
			
			TCHAR diffText[30];
			switch (aiDifficulty) {
				case AIDifficulty::EASY:
					_tcscpy_s(diffText, _T("AI难度: 简单"));
					break;
				case AIDifficulty::NORMAL:
					_tcscpy_s(diffText, _T("AI难度: 普通"));
					break;
				case AIDifficulty::HARD:
					_tcscpy_s(diffText, _T("AI难度: 困难"));
					break;
				case AIDifficulty::HELL:
					_tcscpy_s(diffText, _T("AI难度: 地狱"));
					break;
			}
			
			outtextxy(15, 45, diffText);
			
			// 在准备时间内闪烁显示AI难度选择提示
			if (showPvePreparation) {
				if (flashAIDifficultyText) {
					settextcolor(RGB(255, 255, 0)); // 黄色突出显示
					settextstyle(24, 0, _T("IPix")); // 更大字体
					outtextxy(15, 70, _T("按F1/F2/F3/F4 调整AI难度"));
				}
			} else {
				settextcolor(WHITE);
				settextstyle(20, 0, _T("IPix"));
				outtextxy(15, 70, _T("按F1/F2/F3/F4 调整AI难度"));
			}
		}
		
		// 显示PVE模式准备时间倒计时
		if (showPvePreparation && !is_game_over) {
			// 绘制半透明黑色背景
			int bgWidth = 600;
			int bgHeight = 120;
			int bgX = (getwidth() - bgWidth) / 2;
			int bgY = 50;
			
			setfillcolor(RGB(0, 0, 0));
			setfillstyle(BS_SOLID);
			solidrectangle(bgX, bgY, bgX + bgWidth, bgY + bgHeight);
			
			// 设置字体样式
			settextstyle(32, 0, _T("IPix"));
			setbkmode(TRANSPARENT);
			settextcolor(RGB(255, 80, 80));
			
			// 显示准备时间标题
			TCHAR prepTitle[] = _T("准备时间 - 熟悉操作并选择AI难度");
			int titleWidth = textwidth(prepTitle);
			int titleX = bgX + (bgWidth - titleWidth) / 2;
			outtextxy(titleX, bgY + 20, prepTitle);
			
			// 显示倒计时
			settextstyle(52, 0, _T("IPix"));
			settextcolor(RGB(255, 255, 80));
			
			TCHAR prepCountText[20];
			_stprintf_s(prepCountText, _T("%d 秒"), pvePreparationTime);
			int countWidth = textwidth(prepCountText);
			int countX = bgX + (bgWidth - countWidth) / 2;
			outtextxy(countX, bgY + 60, prepCountText);
		}
		// 显示常规倒计时
		else if (showCountdown && !is_game_over) {
			// 设置大字体
			settextstyle(120, 0, _T("IPix"));
			
			TCHAR countText[10];
			if (countdownValue > 0) {
				_stprintf_s(countText, _T("%d"), countdownValue);
			} else {
				_tcscpy_s(countText, _T("开始!"));
			}
			
			// 设置字体颜色和样式
			settextcolor(RGB(255, 255, 100));
			
			// 计算文本位置
			int textWidth = textwidth(countText);
			int textHeight = textheight(countText);
			int textX = (getwidth() - textWidth) / 2;
			int textY = (getheight() - textHeight) / 2 - 50;
			
			// 绘制带描边的文字
			settextcolor(RGB(0, 0, 0));
			for (int i = -3; i <= 3; i++) {
				for (int j = -3; j <= 3; j++) {
					if (i != 0 || j != 0) {
						outtextxy(textX + i, textY + j, countText);
					}
				}
			}
			
			// 绘制主文本
			settextcolor(RGB(255, 255, 100));
			outtextxy(textX, textY, countText);
		}
		
		// 游戏开始时显示操作指南
		if (showHelp && !is_game_over) {
			// 设置背景和文本参数
			int bgWidth = 550;
			int bgHeight = 320;
			int bgX = (getwidth() - bgWidth) / 2;
			int bgY = 120;
			
			// 绘制半透明黑色背景
			setfillcolor(RGB(0, 0, 0));
			setfillstyle(BS_SOLID);
			solidrectangle(bgX, bgY, bgX + bgWidth, bgY + bgHeight);
			
			// 设置文本样式为透明背景
			settextstyle(28, 0, _T("IPix"));
			setbkmode(TRANSPARENT);
			
			// 绘制控制说明
			settextcolor(RGB(255, 200, 100));
			TCHAR titleText[] = _T("操作指南");
			int titleWidth = textwidth(titleText);
			int titleX = bgX + (bgWidth - titleWidth) / 2;
			outtextxy(titleX, bgY + 20, titleText);
			
			// 设置普通文本样式
			settextstyle(22, 0, _T("IPix"));
			settextcolor(RGB(255, 255, 255));
			
			// 1P控制说明
			TCHAR p1Title[] = _T("1P 控制：");
			outtextxy(bgX + 30, bgY + 60, p1Title);
			outtextxy(bgX + 50, bgY + 90, _T("移动: A/D  跳跃: W"));
			outtextxy(bgX + 50, bgY + 120, _T("攻击: F  特殊技: G"));
			
			// 2P控制说明（仅在PVP模式下显示）
			if (current_game_mode == GameMode::PVP) {
				TCHAR p2Title[] = _T("2P 控制：");
				outtextxy(bgX + 30, bgY + 150, p2Title);
				outtextxy(bgX + 50, bgY + 180, _T("移动: ←/→  跳跃: ↑"));
				outtextxy(bgX + 50, bgY + 210, _T("攻击: .  特殊技: /"));
			} else {
				TCHAR aiTitle[] = _T("面对AI对手：");
				outtextxy(bgX + 30, bgY + 150, aiTitle);
				outtextxy(bgX + 50, bgY + 180, _T("利用地形和平台获得优势"));
				outtextxy(bgX + 50, bgY + 210, _T("积攒能量使用特殊技能"));
				outtextxy(bgX + 50, bgY + 240, _T("注意躲避AI的远程攻击"));
			}
			
			// 战斗技巧
			settextcolor(RGB(150, 200, 255));
			TCHAR tipsTitle[] = _T("战斗技巧：");
			outtextxy(bgX + 300, bgY + 60, tipsTitle);
			outtextxy(bgX + 320, bgY + 90, _T("攻击可积累能量"));
			outtextxy(bgX + 320, bgY + 120, _T("能量满时可释放特殊技"));
			outtextxy(bgX + 320, bgY + 150, _T("善用跳跃躲避攻击"));
			outtextxy(bgX + 320, bgY + 180, _T("掉落地图外会失去全部生命"));
			
			// 角色特点
			if (current_game_mode == GameMode::PVE) {
				settextcolor(RGB(255, 180, 180));
				TCHAR charTitle[] = _T("角色特点：");
				outtextxy(bgX + 300, bgY + 210, charTitle);
				outtextxy(bgX + 320, bgY + 240, _T("豌豆射手：远程攻击"));
				outtextxy(bgX + 320, bgY + 270, _T("向日葵：近战爆发"));
			}
			
			// 提示信息
			settextcolor(RGB(150, 255, 150));
			TCHAR hintText[] = _T("按H键显示/隐藏此帮助");
			int hintWidth = textwidth(hintText);
			int hintX = bgX + (bgWidth - hintWidth) / 2;
			outtextxy(hintX, bgY + 285, hintText);
		}
	}
	void on_input(const ExMessage& msg) 
	{
		// 如果场景切换保护标志已设置，忽略所有输入
		if (prevent_scene_switch) {
			return;
		}
	
		// 在PVE模式准备时间内，允许移动但禁止攻击
		if (current_game_mode == GameMode::PVE && showPvePreparation) {
			// 复制消息以进行过滤
			ExMessage filtered_msg = msg;
			
			// 过滤掉玩家的攻击相关按键
			if (msg.message == WM_KEYDOWN) {
				// 玩家1的攻击键 (F和G)
				if (msg.vkcode == 0x46 || msg.vkcode == 0x47) {
					return; // 直接丢弃攻击输入
				}
			}
			
			// 只处理移动相关的输入
			player_1->on_input(filtered_msg);
			return;
		}
		
		player_1->on_input(msg);
		
		// 如果不是人机对战，则正常处理player_2的输入
		if (current_game_mode != GameMode::PVE) {
			player_2->on_input(msg);
		}
		
		switch (msg.message)
		{
		case WM_KEYUP:
			if (msg.vkcode == 0x51) // 'Q'键
			{
				is_debug = !is_debug;
			}
			
			// H键显示/隐藏帮助
			if (msg.vkcode == 0x48) // 'H'键
			{
				showHelp = !showHelp;
				
				// 如果开启帮助显示，重置计时器
				if (showHelp) {
					helpTimer.restart();
				}
			}
				
			// 只在PVE模式下调整AI难度
			if (current_game_mode == GameMode::PVE && aiController) {
				if (msg.vkcode == VK_F1) {
					aiDifficulty = AIDifficulty::EASY;
					aiController->setDifficulty(AIDifficulty::EASY);
					
					// 如果在准备时间内修改难度，播放确认音效
					if (showPvePreparation) {
						mciSendString(_T("play ui_select from 0"), NULL, 0, NULL);
					}
				}
				else if (msg.vkcode == VK_F2) {
					aiDifficulty = AIDifficulty::NORMAL;
					aiController->setDifficulty(AIDifficulty::NORMAL);
					
					// 如果在准备时间内修改难度，播放确认音效
					if (showPvePreparation) {
						mciSendString(_T("play ui_select from 0"), NULL, 0, NULL);
					}
				}
				else if (msg.vkcode == VK_F3) {
					aiDifficulty = AIDifficulty::HARD;
					aiController->setDifficulty(AIDifficulty::HARD);
					
					// 如果在准备时间内修改难度，播放确认音效
					if (showPvePreparation) {
						mciSendString(_T("play ui_select from 0"), NULL, 0, NULL);
					}
				}
				else if (msg.vkcode == VK_F4) {
					aiDifficulty = AIDifficulty::HELL;
					aiController->setDifficulty(AIDifficulty::HELL);
					
					// 如果在准备时间内修改难度，播放确认音效
					if (showPvePreparation) {
						mciSendString(_T("play ui_select from 0"), NULL, 0, NULL);
					}
				}
			}
			break;
		}
	}
	void on_exit() {  }
};
