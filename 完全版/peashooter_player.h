#pragma once
#include"player.h"
#include"atlas.h"
#include<iostream>
#include"pea_bullet.h"
using namespace std;
extern Atlas atlas_peashooter_idle_left;
extern Atlas atlas_peashooter_idle_right;
extern Atlas atlas_peashooter_run_left;
extern Atlas atlas_peashooter_run_right;
extern Atlas atlas_peashooter_attack_ex_right;
extern Atlas atlas_peashooter_attack_ex_left;
extern vector<Bullet*>bullet_list;
extern Atlas atlas_peashooter_die_left;
extern Atlas atlas_peashooter_die_right;
extern Camera main_camera;
class PeashooterPlayer :public Player
{
	Timer timer_attack_ex;
	Timer timer_spawn_pea_ex;
	const float speed_pea = 0.75f;
	const float speed_pea_ex = 1.5f;
	const int attack_ex_duration = 2500;
	void spawn_pea_bullet(float speed)
	{
		PeaBullet* bullet = new PeaBullet();
		Vector2 bullet_position, bullet_velocity;
		const Vector2& bullet_size = bullet->get_size();
		bullet_position.x = is_facing_right ? position.x + size.x - bullet_size.x / 2 : position.x - bullet_size.x / 2;
		bullet_position.y = position.y;

		// 获取当前的AI难度 (g_ai_difficulty 在 ai_controller.h 中声明为 extern)
		// 0=EASY, 1=NORMAL, 2=HARD, 3=HELL
		extern int g_ai_difficulty;

		if (id == PlayerID::P1 && is_in_super_mode()) {
			// 抛物线发射
			float current_bullet_vel_x = 0.25f;
			float current_bullet_vel_y = -0.5f;

			// 地狱难度下，P1超级模式的豌豆子弹水平和垂直速度均降低
			if (g_ai_difficulty == 3) { // HELL difficulty
				current_bullet_vel_x *= 0.7f; 
				current_bullet_vel_y *= 0.7f; // 垂直速度是负的，乘以0.7会使其绝对值减小，即向上抛得更低/慢
			}

			bullet_velocity.x = is_facing_right ? current_bullet_vel_x : -current_bullet_vel_x;
			bullet_velocity.y = current_bullet_vel_y;
			bullet->is_parabola = true;
		} else {
			// 直线发射 (AI或P1普通模式，速度保持不变)
		bullet_velocity.x = is_facing_right ? speed : -speed;
		bullet_velocity.y = 0;
		}
		bullet->set_position(bullet_position.x, bullet_position.y);
		bullet->set_velocity(bullet_velocity.x, bullet_velocity.y);
		bullet->set_collide_target(id==PlayerID::P1?PlayerID::P2:PlayerID::P1);
		bullet->set_callback([&]() {mp += 25; });
		// 如果处于超级模式，应用伤害倍率
		if (is_in_super_mode()) {
			bullet->apply_damage_multiplier(get_damage_multiplier());
		}
		bullet_list.push_back(bullet);
     }
public:
	PeashooterPlayer(bool facing_right=1):Player(facing_right)
	{
		animation_idle_left.set_atlas(&atlas_peashooter_idle_left);
		animation_idle_right.set_atlas(&atlas_peashooter_idle_right);
		animation_run_left.set_atlas(&atlas_peashooter_run_left);
		animation_run_right.set_atlas(&atlas_peashooter_run_right);
		animation_attack_ex_left.set_atlas(&atlas_peashooter_attack_ex_left);
		animation_attack_ex_right.set_atlas(&atlas_peashooter_attack_ex_right);
		animation_die_left.set_atlas(&atlas_peashooter_die_left);
		animation_die_right.set_atlas(&atlas_peashooter_die_right);

		animation_idle_left.set_interval(75);//鐢囨？闂?
		animation_idle_right.set_interval(75);
		animation_run_left.set_interval(75);
		animation_run_right.set_interval(75);
		animation_attack_ex_right.set_interval(75);
		animation_attack_ex_left.set_interval(75);
		animation_die_left.set_interval(150);
		animation_die_right.set_interval(150);
		animation_die_left.set_loop(0);
		animation_die_right.set_loop(0);
		size.x = 96; size.y = 96;
		timer_attack_ex.set_wait_time(attack_ex_duration);
		timer_attack_ex.set_one_shot(true);
		timer_attack_ex.set_callback([&]() {
			is_attacking_ex = 0;
			});
		timer_spawn_pea_ex.set_wait_time(100);
		timer_spawn_pea_ex.set_callback([&]() {
			spawn_pea_bullet(speed_pea_ex);
			});
		attack_cd = 100;
	}
	~PeashooterPlayer() = default;
	void on_update(int delta)
	{
		Player::on_update(delta);
		if (is_attacking_ex)
		{
			main_camera.shake(5, 100);
			timer_attack_ex.on_update(delta);
			timer_spawn_pea_ex.on_update(delta);
		}
	}
	void on_attack()
	{
		spawn_pea_bullet(speed_pea);
		switch (rand() % 2)
		{
		case 0:
			mciSendString(_T("play pea_shoot_1 from 0"), NULL, 0, NULL);
			break;
		case 1:
			mciSendString(_T("play pea_shoot_2 from 0"), NULL, 0, NULL);
			break;
		}
	}
	void on_attack_ex()
	{
		is_attacking_ex = 1;
		timer_attack_ex.restart();
		is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();
		mciSendString(_T("play pea_shoot_ex from 0"), NULL, 0, NULL);
	}
};
