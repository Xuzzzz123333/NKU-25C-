#pragma once
#include"camera.h"
#include"bullet.h"
#include<graphics.h>
#include"vector2.h"
#include"player_id.h"
#include"animation.h"
#include"platform.h"
#include"particle.h"
#include"util.h"
#include"game_mode.h"
// 不要包含 ai_controller.h，使用全局变量
extern int g_ai_difficulty; // 用整型表示AI难度：0=简单，1=普通，2=困难，3=地狱
extern GameMode current_game_mode; // 添加对当前游戏模式的外部声明

using namespace std;
extern vector<Platform>platform_list;
extern vector<Bullet*>bullet_list;
extern Atlas atlas_run_effect;
extern Atlas atlas_jump_effect;
extern Atlas atlas_land_effect;
extern IMAGE img_1P_cursor;
extern IMAGE img_2P_cursor;

extern bool is_debug;

// 根据当前AI难度获取伤害倍率的辅助函数
inline float get_ai_damage_multiplier() {
	switch (g_ai_difficulty) {
		case 0: return 1.0f;  // EASY - 恢复原始值
		case 1: return 1.5f;  // NORMAL - 恢复原始值
		case 2: return 2.0f;  // HARD - 恢复原始值
		case 3: return 1.7f;  // HELL - 从2.0f进一步降低到1.7f
		default: return 1.0f; // 恢复原始值
	}
}

class Player
{protected:
	Vector2 position;
	Animation animation_idle_left;//閸氭垵涔忛崝銊ф暰
	Animation animation_idle_right;
	Animation animation_run_left;
	Animation animation_run_right;
	Animation* current_animation = nullptr;//瑜版挸澧犻幘顓熸杹閸斻劎鏁?
	Animation animation_jump_effect;
	Animation animation_land_effect;
	Animation animation_die_left;
	Animation animation_die_right;
	bool is_jump_effect_visible = 0;
	bool is_land_effect_visible = 0;
	Vector2 position_jump_effect;
	Vector2 position_land_effect;
	PlayerID id = PlayerID::P1;
	bool is_right_key_down = 0;
	bool is_left_key_down = 0;
	bool is_facing_right = 1;
	bool can_attack = 1;//閺勵垰鎯侀崣顖欎簰闁插﹥鏂侀弨璇插毊
	Timer timer_attack_cd;//
	int attack_cd = 500;
	const float run_velocity = 0.55f;//鐠烘垵濮╅柅鐔峰
	const float gravity = 1.6e-3f;
	int mp = 0;//閼充粙鍣?
	int hp = 100;//鐞涒偓闁?
	bool is_attacking_ex = 0;
	Vector2 velocity;//鐟欐帟澹婇柅鐔峰
	Vector2 size;//閻溾晛顔嶇亸鍝勵嚟
	Vector2 original_size; // 原始大小，用于超级模式还原
	Animation animation_attack_ex_right;
	Animation animation_attack_ex_left;
	const float jump_velocity = -0.85f;
	bool is_invulnerable = 0;//閺勵垰鎯佹径鍕艾閺冪姵鏅悩鑸碘偓?
	bool is_showing_sketch_frame = 0;//閺勵垰鎯佹惔鏃囶嚉閺勫墽銇氶崜顏勫
	Timer timer_invulnerable;//閺冪姵鏅悩鑸碘偓浣哥暰閺冭泛娅?
	Timer timer_invulnerable_blink;
	Timer timer_run_effect_generation;
	Timer timer_die_effect_generation;
	vector<Particle>particle_list;
	IMAGE img_sketch;
	bool is_cursor_visible = 1;
	Timer timer_cursor_visibility;
	Vector2 last_hurt_direction;

	// 超级模式相关变量
	bool is_super_mode = false; // 是否处于超级模式
	Timer timer_super_mode; // 超级模式计时器
	float damage_multiplier = 1.0f; // 伤害倍率
	float damage_reduction = 1.0f; // 伤害减免率
	bool super_mode_used = false;
	
	void move_and_collide(int delta)
	{
		float last_velocity_y = velocity.y;
		velocity.y += gravity * delta;
		position += velocity * (float)delta;
		if (hp <= 0)
			return;
		if (velocity.y > 0)
		{
			for (const Platform& platform : platform_list)//闁秴宸婚獮鍐插酱鐎电钖?
			{
				const Platform::CollisionShape& shape = platform.shape;
				bool is_collide_x = (max(position.x + size.x, shape.right) - min(position.x, shape.left) <= size.x + (shape.right - shape.left));//閻溾晛顔嶉惌鈺佽埌閸滃苯閽╅崣鏉挎躬濮樻潙閽╅弬鐟版倻娑撳﹥妲搁崥锕€褰傞悽鐔侯潾閹?
				bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);
				if (is_collide_x && is_collide_y)
				{
					float delta_pos_y = velocity.y * delta;
					float last_tick_foot_pos_y = position.y + size.y - delta_pos_y;
					if (last_tick_foot_pos_y <= shape.y)
					{
						position.y = shape.y - size.y;
						velocity.y = 0;
						if (last_velocity_y != 0)
							on_land();
						break;
					}
				}
			}
		}
		if (!is_invulnerable)
		{
			for (Bullet* bullet : bullet_list)
			{
				if (!bullet->get_valid() || bullet->get_collide_target() != id)
					continue;
				if (bullet->check_collision(position, size))
				{
					make_invulnerable();
					bullet->on_collide();
					bullet->set_valid(0);
					// 超级模式下受到的伤害减少
					int actual_damage = static_cast<int>(bullet->get_damage() * damage_reduction);
					// PVE模式下，AI（P2）攻击玩家（P1）时根据AI难度调整伤害
					if (current_game_mode == GameMode::PVE) {
						if (id == PlayerID::P1 && bullet->get_collide_target() == PlayerID::P1) {
							// 使用辅助函数获取AI难度对应的伤害倍率
							float ai_damage_mul = get_ai_damage_multiplier();
							actual_damage = static_cast<int>(actual_damage * ai_damage_mul);
						} else {
							// 玩家攻击AI时，伤害变为1/3
							actual_damage = static_cast<int>(actual_damage / 3.0f);
						}
						if (actual_damage < 1) actual_damage = 1;
					}
					hp -= actual_damage;
					last_hurt_direction = bullet->get_position() - position;
					if (hp <= 0)
					{
						velocity.x = last_hurt_direction.x < 0 ? 0.35f : -0.35f;
						velocity.y = -1.0f;
					}
				}
			}
		}

	}
public:
	Player(bool facing_right=1):is_facing_right(facing_right), super_mode_used(false)
	{
		current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
		timer_attack_cd.set_wait_time (attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback([&]()
			{
				can_attack = 1;
			});
		timer_invulnerable.set_wait_time(750);
		timer_invulnerable.set_one_shot(1);
		timer_invulnerable.set_callback([&]() {
			is_invulnerable = 0;
			});
		timer_invulnerable_blink.set_wait_time(75);
		timer_invulnerable_blink.set_callback([&]() {
			is_showing_sketch_frame = !is_showing_sketch_frame;
			});
		timer_run_effect_generation.set_wait_time(75);
		timer_run_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 45);
			});
		timer_die_effect_generation.set_wait_time(35);
		timer_die_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 150);
			});
		animation_jump_effect.set_atlas(&atlas_jump_effect);
		animation_jump_effect.set_interval(25);
		animation_jump_effect.set_loop(0);
		animation_jump_effect.set_callback([&]()
			{
				is_jump_effect_visible = 0;
			});
		animation_land_effect.set_atlas(&atlas_land_effect);
		animation_land_effect.set_interval(50);
		animation_land_effect.set_loop(0);
		animation_land_effect.set_callback([&]()
			{
				is_land_effect_visible = 0;
			});
		timer_cursor_visibility.set_wait_time(2500);
		timer_cursor_visibility.set_one_shot(1);
		timer_cursor_visibility.set_callback([&]()
			{
				is_cursor_visible = 0;
			});

		// 初始化超级模式计时器
		timer_super_mode.set_wait_time(10000); // 10秒持续时间
		timer_super_mode.set_one_shot(1);
		timer_super_mode.set_callback([&]() {
			deactivate_super_mode();
		});
	}
	~Player() = default;
	virtual void on_update(int delta)
	{
		int direction = is_right_key_down - is_left_key_down;
		if (direction != 0)
		{   if(!is_attacking_ex)
			is_facing_right = direction > 0;
			current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
			float distance = direction * run_velocity * delta;
			on_run(distance);
		}
		else
		{
			current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
			timer_run_effect_generation.pause();
		}
		if (is_attacking_ex)
			current_animation = is_facing_right ? &animation_attack_ex_right : &animation_attack_ex_left;
		if (hp <= 0)
			current_animation = last_hurt_direction.x < 0 ? &animation_die_left : &animation_die_right;
		current_animation->on_update(delta);
		animation_jump_effect.on_update(delta);
		animation_land_effect.on_update(delta);
		timer_attack_cd.on_update(delta);
		timer_invulnerable_blink.on_update(delta);
		timer_invulnerable.on_update(delta);
		timer_run_effect_generation.on_update(delta);
		timer_cursor_visibility.on_update(delta);
		// 更新超级模式计时器
		if (is_super_mode) {
			timer_super_mode.on_update(delta);
		}
		if (hp <= 0)
			timer_die_effect_generation.on_update(delta);
		particle_list.erase(remove_if(particle_list.begin(), particle_list.end(), [](const Particle& particle)
			{
				return !particle.check_valid();
			}),
			particle_list.end());
		for (Particle& particle : particle_list)
			particle.on_update(delta);
		if (is_showing_sketch_frame)
			sketch_image(current_animation->get_frame(), &img_sketch);
		move_and_collide(delta);
	}
	virtual void on_draw(const Camera& camera)
	{
		if (is_jump_effect_visible)
		{
			IMAGE* frame = animation_jump_effect.get_frame();
			if (is_super_mode) {
				putimage_alpha_scale(camera, (int)position_jump_effect.x, (int)position_jump_effect.y, frame, 1.5f);
			} else {
			animation_jump_effect.on_draw(camera, (int)position_jump_effect.x, (int)position_jump_effect.y);
			}
		}
		if (is_land_effect_visible)
		{
			IMAGE* frame = animation_land_effect.get_frame();
			if (is_super_mode) {
				putimage_alpha_scale(camera, (int)position_land_effect.x, (int)position_land_effect.y, frame, 1.5f);
			} else {
			animation_land_effect.on_draw(camera, (int)position_land_effect.x, (int)position_land_effect.y);
			}
		}
		for (const Particle& particle : particle_list)
			particle.on_draw(camera);
		// 绘制超级模式特效
		if (is_super_mode) {
			setlinecolor(RGB(255, 215, 0));
			setlinestyle(PS_SOLID, 3);
			ellipse(
				(int)(camera.get_position().x + position.x - 10),
				(int)(camera.get_position().y + position.y - 10),
				(int)(camera.get_position().x + position.x + size.x + 10),
				(int)(camera.get_position().y + position.y + size.y + 10)
			);
			// 拉伸绘制主动画帧
			IMAGE* frame = current_animation->get_frame();
			putimage_alpha_scale(camera, (int)position.x, (int)position.y, frame, 1.5f);
		} else {
		current_animation->on_draw(camera, (int)position.x, (int)position.y);
		}
		if (is_debug)
		{
			setlinecolor(RGB(0, 125, 255));
			rectangle((int)position.x, (int)position.y, (int)(position.x + size.x), (int)(position.y + size.y));
		}
		if (is_cursor_visible)
		{
			switch (id)
			{
			case PlayerID::P1:
				putimage_alpha(camera, (int)(position.x + (size.x - img_1P_cursor.getwidth()) / 2),
					(int)(position.y - img_1P_cursor.getheight()), &img_1P_cursor);
				break;
			case PlayerID::P2:
				putimage_alpha(camera, (int)(position.x + (size.x - img_2P_cursor.getwidth()) / 2),
					(int)(position.y - img_2P_cursor.getheight()), &img_2P_cursor);
				break;
			}
		}
		// 显示超级模式剩余时间
		if (is_super_mode) {
			int remainingTime = (int)(timer_super_mode.get_time_left() / 1000); // 转换为秒
			TCHAR timeText[20];
			swprintf_s(timeText, 20, _T("超级模式: %ds"), remainingTime);
			settextcolor(RGB(255, 215, 0));
			settextstyle(20, 0, _T("IPix"));
			outtextxy(
				(int)(camera.get_position().x + position.x + (size.x - textwidth(timeText)) / 2),
				(int)(camera.get_position().y + position.y - 25),
				timeText
			);
		}
		// 主动画帧部分
		if (hp > 0 && is_invulnerable && is_showing_sketch_frame) {
			float scale = is_super_mode ? 1.5f : 1.0f;
			putimage_alpha_scale(camera, (int)position.x, (int)position.y, &img_sketch, scale);
		} else if (is_super_mode) {
			IMAGE* frame = current_animation->get_frame();
			putimage_alpha_scale(camera, (int)position.x, (int)position.y, frame, 1.5f);
		} else {
			current_animation->on_draw(camera, (int)position.x, (int)position.y);
		}
	}
	virtual void on_input(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
				case 0x41:
					is_left_key_down = 1;
					break;
				case 0x44:
					is_right_key_down = 1;
					break;
				case 0x57://閳ユ辅閳?
					on_jump();
					break;
				case 0x46://F
					if (can_attack)
					{
						on_attack();
						can_attack = 0;
						timer_attack_cd.restart();
					}
					break;
				case 0x47://G
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				case 0x4A: // J - 激活超级模式
					if (id == PlayerID::P1) {
						int can_activate = (!is_super_mode && !super_mode_used && hp < 50);
						activate_super_mode();
						if (can_activate) {
							mciSendString(_T("play ji from 0"), NULL, 0, NULL);
						}
					}
					break;
				}

				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT:
					is_left_key_down = 1;
					break;
				case VK_RIGHT:
					is_right_key_down = 1;
					break;
				case VK_UP:
					on_jump();
					break;
				case VK_OEM_PERIOD://.
					if (can_attack)
					{
						on_attack();
						can_attack = 0;
						timer_attack_cd.restart();

					}
					break;
				case VK_OEM_2://"/"
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			}
			break;


			
		case WM_KEYUP:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
				case 0x41:
					is_left_key_down = 0;
					break;
				case 0x44:
					is_right_key_down = 0;
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT:
					is_left_key_down = 0;
					break;
				case VK_RIGHT:
					is_right_key_down = 0;
					break;

				}

				break;
			}
			break;
		}


	}
	void set_id(PlayerID id)
	{
		this->id = id;
	}
	void set_position(float x, float y)
	{
		position.x = x;
		position.y = y;
	}
	virtual void on_run(float distance)
	{
		if (is_attacking_ex)
		{
			return;
	     }
		position.x += distance;
		timer_run_effect_generation.resume();
	}
	virtual void on_jump()
	{     
		if (velocity.y != 0||is_attacking_ex)
			return;
		velocity.y += jump_velocity;
		is_jump_effect_visible = 1;
		animation_jump_effect.reset();
		IMAGE* effect_frame = animation_jump_effect.get_frame();
		position_jump_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_jump_effect.y = position.y + size.y - effect_frame->getheight();
	}
	virtual void on_attack()//閺咁噣鈧碍鏁鹃崙?
	{

	}
	virtual void on_attack_ex()
	{

	}
	const Vector2& get_position()const
	{
		return position;
	}
	const Vector2& get_size()const
	{
		return size;
	}
	const Vector2& get_velocity() const
	{
		return velocity;
	}
	void make_invulnerable()
	{
		is_invulnerable = 1;
		timer_invulnerable.restart();
	}
	int get_hp()const
	{
		return hp;
	}
	int get_mp()const
	{
		return mp;
	}
	void on_land()
	{
		is_land_effect_visible = 1;
		animation_land_effect.reset();
		IMAGE* effect_frame = animation_land_effect.get_frame();
		position_land_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_land_effect.y = position.y + size.y - effect_frame->getheight();
	}
	void set_hp(int val)
	{
		hp = val;
	}

	// 激活超级模式
	void activate_super_mode() {
		if (is_super_mode || super_mode_used || hp >= 50) return; // 只能用一次且血量<50
		is_super_mode = true;
		super_mode_used = true;
		damage_multiplier = 2.0f; // 伤害翻倍
		damage_reduction = 0.5f;  // 受到伤害减半
		// 备份当前大小并放大
		original_size = size;
		size.x *= 1.5f;
		size.y *= 1.5f;
		// 调整位置以避免通过平台
		position.y -= 0.5f * original_size.y;
		timer_super_mode.restart();
	}
	
	// 停用超级模式
	void deactivate_super_mode() {
		if (!is_super_mode) return;
		is_super_mode = false;
		damage_multiplier = 1.0f;
		damage_reduction = 1.0f;
		// 恢复原始大小
		position.y += 0.5f * original_size.y;
		size = original_size;
	}
	
	// 获取伤害倍率
	float get_damage_multiplier() const {
		return damage_multiplier;
	}
	
	// 检查是否处于超级模式
	bool is_in_super_mode() const {
		return is_super_mode;
	}

	// 添加一个用于判断角色是否面向左侧的方法
	bool is_facing_left() const {
		return !is_facing_right;
	}
};
