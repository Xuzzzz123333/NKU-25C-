#pragma once
#include<functional>
using namespace std;
class Timer
{
	int pass_time = 0;
	int wait_time = 0;
	bool paused = false;
	bool shotted = false;
	bool one_shot = false;
	function<void()>callback;
public:
	Timer() = default;
	~Timer() = default;
	void restart()
	{
		pass_time = 0;
		shotted = false;
	}
	void set_wait_time(int val)
	{
		wait_time = val;
	}
	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	void on_update(int delta)
	{
		if (paused)
			return;
		pass_time += delta;
		if (pass_time >= wait_time)
		{
			if ((!one_shot || (one_shot && !shotted)) && callback)//计时器不是单次触发或没被触发
				callback();
			shotted = true;
			pass_time = 0;
		}
	}
	void set_callback(function<void()>callback)
	{
		this->callback = callback;
	}
	void pause()
	{
		paused = true;
	}
	void resume()
	{
		paused = false;
	}
};
