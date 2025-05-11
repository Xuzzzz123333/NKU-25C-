#pragma once
#include"bullet.h"
#include"camera.h"
#include"animation.h"
extern IMAGE img_pea;
extern Atlas atlas_pea_break;
class PeaBullet:public Bullet
{
	Animation animation_break;//鐠炲矁鐪寸€涙劕鑴婇惍瀵割暢閸斻劎鏁?
public:
	bool is_parabola = false;
	const float gravity = 1e-3f;
	PeaBullet()
	{
		size.x = 64; size.y = 64;
		damage = 10;
		animation_break.set_atlas(&atlas_pea_break);
		animation_break.set_interval(100);
		animation_break.set_loop(false);//瀵邦亞骞嗛悩鑸碘偓?
		animation_break.set_callback([&]() {can_remove = true;  });
	}
	~PeaBullet() = default;
	void on_collide()
	{
		Bullet::on_collide();
		switch (rand() % 3)
		{
		case 0:
			mciSendString(_T("play pea_break_1 from 0"), NULL, 0, NULL);
			break;
		case 1:
			mciSendString(_T("play pea_break_2 from 0"), NULL, 0, NULL);
			break;
		case 2:
			mciSendString(_T("play pea_break_3 from 0"), NULL, 0, NULL);
			break;
		}
	}
	void on_update(int delta)
	{
		if (is_parabola) {
			velocity.y += gravity * delta;
		}
		position += velocity * (float)delta;
		if (!valid)
			animation_break.on_update(delta);
		if (check_if_exceeds_screen())
			can_remove = 1;
	}
	void on_draw(const Camera& camera)const
	{
		if (valid)
			putimage_alpha(camera, (int)position.x, (int)position.y, &img_pea);
		else
			animation_break.on_draw(camera, (int)position.x, (int)position.y);
		Bullet::on_draw(camera);
	}
	void apply_damage_multiplier(float multiplier) {
		damage = static_cast<int>(damage * multiplier);
	}
};
