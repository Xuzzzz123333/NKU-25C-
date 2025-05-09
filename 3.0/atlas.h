#pragma once
#include<vector>
#include"graphics.h"
using namespace std;
class Atlas
{
public:
	Atlas() = default;
	~Atlas() = default;
	void load_from_file(LPCTSTR path_template, int num)//閸旂姾娴囬崶鎯у剼閸掓銆?
	{
		img_list.clear();
		img_list.resize(num);
		TCHAR path_file[256];
		for (int i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path_template, i + 1);
			loadimage(&img_list[i], path_file);
		}
	}
	void clear()//濞撳懐鈹栧鎻掑鏉炵晫娈戦崶鍓у鐎电钖?
	{
		img_list.clear();
	}
	int get_size()
	{
		return(int)img_list.size();
	}
	IMAGE* get_image(int idx)
	{
		if (idx < 0 || idx >= img_list.size())
		{
			return nullptr;
		}
		return &img_list[idx];
	}
	void add_image(const IMAGE& img)
	{
		img_list.push_back(img);
	}
private:
  vector<IMAGE>img_list;
};
