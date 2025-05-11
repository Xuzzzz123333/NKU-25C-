#pragma once
#include"vector2.h"
#include"timer.h"
class Camera
{
	Vector2 position;
	Timer timer_shake;
	bool is_shaking = false;//闁哄嫷鍨伴幆渚€骞庨弽褍袟
	float shaking_strength = 0;//闁硅埖鐗曟慨鈺侇嚕閸濆嫬顔?
public:
	Camera()
	{
		timer_shake.set_one_shot(true);
		timer_shake.set_callback([&]()
			{
				is_shaking = false;//闁稿绮嶉娑㈠箮閺嵮冃?
				reset();
			});
	}
	~Camera() = default;
	const Vector2& get_position()const
	{
		return position;
	}
	void reset()
	{
		position.x = 0;
		position.y = 0;
	}
	void on_update(int delta)
	{
		timer_shake.on_update(delta);
		if (is_shaking)
		{
			position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
			position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;//闁革箑妫楅懜浼存⒕韫囨梹绨氶柛鎺戞缁?
		}
	}
	void shake(float strength, int duration)
	{
		is_shaking = true;
		shaking_strength = strength;
		timer_shake.set_wait_time(duration);
		timer_shake.restart();
	}
};
