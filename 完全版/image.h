#pragma once
#include<graphics.h>
#include"util.h"
#include"atlas.h"
#include"scene.h"
#include"menu_scene.h"
#include"game.scene.h"
#include"scene_manager.h"
#include"selector_scene.h"
#pragma comment(lib,"Winmm.lib")


IMAGE img_VS;

//1P 閺傚洦婀?

IMAGE img_1P;

//2P 閺傚洦婀?

IMAGE img_2P;

//1P 闁款喕缍呴幓蹇氬牚

IMAGE img_1P_desc;

//2P 闁款喕缍呴幓蹇氬牚

IMAGE img_2P_desc;

//閺堟繂鎮滃锔炬畱婢ф挾顣?

IMAGE img_gravestone_left;

//閺堟繂鎮滈崣宕囨畱婢ф挾顣?

IMAGE img_gravestone_right;

//闁顫楅悾宀勬桨閹绘劗銇氭穱鈩冧紖閺傚洦婀?

IMAGE img_selector_tip;

//闁顫楅悾宀勬桨閼冲本娅欓崶?

IMAGE img_selector_background;

//1P 閸氭垵涔忛柅澶嬪閹稿鎸虫妯款吇閻樿埖鈧?

IMAGE img_1P_selector_btn_idle_left;

//1P 閸氭垵褰搁柅澶嬪閹稿鎸虫妯款吇閻樿埖鈧?

IMAGE img_1P_selector_btn_idle_right;

//1P 閸氭垵涔忛柅澶嬪閹稿鎸抽幐澶夌瑓閻樿埖鈧?

IMAGE img_1P_selector_btn_down_left;

//1P 閸氭垵褰搁柅澶嬪閹稿鎸抽幐澶夌瑓閻樿埖鈧?

IMAGE img_1P_selector_btn_down_right;

//2P 閸氭垵涔忛柅澶嬪閹稿鎸虫妯款吇閻樿埖鈧?

IMAGE img_2P_selector_btn_idle_left;

//2P 閸氭垵褰搁柅澶嬪閹稿鎸虫妯款吇閻樿埖鈧?

IMAGE img_2P_selector_btn_idle_right;

//2P 閸氭垵涔忛柅澶嬪閹稿鎸抽幐澶夌瑓閻樿埖鈧?

IMAGE img_2P_selector_btn_down_left;

//2P 閸氭垵褰搁柅澶嬪閹稿鎸抽幐澶夌瑓閻樿埖鈧?

IMAGE img_2P_selector_btn_down_right;

//闁顫楅悾宀勬桨閺堟繂鎮滃锔炬畱鐠炲矁鐪寸亸鍕

IMAGE img_peashooter_selector_background_left;

//闁顫楅悾宀勬桨閺堟繂鎮滈崣宕囨畱鐠炲矁鐪寸亸鍕

IMAGE img_peashooter_selector_background_right;

//闁顫楅悾宀勬桨閺堟繂鎮滃锔炬畱姒瑦妫╅拋?

IMAGE img_sunflower_selector_background_left;

//闁顫楅悾宀勬桨閺堟繂鎮滈崣宕囨畱姒瑦妫╅拋?

IMAGE img_sunflower_selector_background_right;



//婢垛晝鈹?

IMAGE img_sky;

//鐏炶精鍓?

IMAGE img_hills;

//婢堆冪€烽獮鍐插酱

IMAGE img_platform_large;

//鐏忓繐鐎烽獮鍐插酱

IMAGE img_platform_small;



//1P 閹稿洨銇氶崗澶嬬垼

IMAGE img_1P_cursor;

//2P 閹稿洨銇氶崗澶嬬垼

IMAGE img_2P_cursor;



// 鐠炲矁鐪寸亸鍕閸ラ箖娉?

//閺堟繂鎮滃锔炬畱姒涙顓婚崝銊ф暰

Atlas atlas_peashooter_idle_left;

//閺堟繂鎮滈崣宕囨畱姒涙顓婚崝銊ф暰

Atlas atlas_peashooter_idle_right;

//閺堟繂鎮滃锔炬畱婵傛棁绐囬崝銊ф暰

Atlas atlas_peashooter_run_left;

//閺堟繂鎮滈崣宕囨畱婵傛棁绐囬崝銊ф暰

Atlas atlas_peashooter_run_right;

//閺堟繂鎮滃锔炬畱閻楄鐣╅弨璇插毊閸斻劎鏁?

Atlas atlas_peashooter_attack_ex_left;

//閺堟繂鎮滈崣宕囨畱閻楄鐣╅弨璇插毊閸斻劎鏁?

Atlas atlas_peashooter_attack_ex_right;

//閺堟繂鎮滃锔炬畱濮濊楠搁崝銊ф暰

Atlas atlas_peashooter_die_left;

//閺堟繂鎮滈崣宕囨畱濮濊楠搁崝銊ф暰

Atlas atlas_peashooter_die_right;



//姒瑦妫╅拋闈涙禈闂?

//閺堟繂鎮滃锔炬畱姒涙顓婚崝銊ф暰

Atlas atlas_sunflower_idle_left;

//閺堟繂鎮滈崣宕囨畱姒涙顓婚崝銊ф暰

Atlas atlas_sunflower_idle_right;

//閺堟繂鎮滃锔炬畱婵傛棁绐囬崝銊ф暰

Atlas atlas_sunflower_run_left;

//閺堟繂鎮滈崣宕囨畱婵傛棁绐囬崝銊ф暰

Atlas atlas_sunflower_run_right;

//閺堟繂鎮滃锔炬畱閻楄鐣╅弨璇插毊閸斻劎鏁?

Atlas atlas_sunflower_attack_ex_left;

//閺堟繂鎮滈崣宕囨畱閻楄鐣╅弨璇插毊閸斻劎鏁?

Atlas atlas_sunflower_attack_ex_right;

//閺堟繂鎮滃锔炬畱濮濊楠搁崝銊ф暰

Atlas atlas_sunflower_die_left;

//閺堟繂鎮滈崣宕囨畱濮濊楠搁崝銊ф暰
Atlas atlas_sunflower_die_right;
//鐠炲矁鐪撮崶鍓у
IMAGE img_pea;
//鐠炲矁鐪撮惍瀵割暢閸斻劎鏁鹃崶楣冩肠
Atlas atlas_pea_break;
//閺冦儱鍘滈崝銊ф暰閸ラ箖娉?
Atlas atlas_sun;
//閺冦儱鍘滈悥鍡欏仮閸斻劎鏁鹃崶楣冩肠
Atlas atlas_sun_explode;
//閻楄鐣╅弮銉ュ帨閸斻劎鏁鹃崶楣冩肠
Atlas atlas_sun_ex;
//閻楄鐣╅弮銉ュ帨閻栧棛鍋㈤崝銊ф暰閸ラ箖娉?
Atlas atlas_sun_ex_explode;
//"閺?閺傚洦婀伴崝銊ф暰閸ラ箖娉?
Atlas atlas_sun_text;
//婵傛棁绐囬悧瑙勬櫏閸斻劎鏁鹃崶楣冩肠
Atlas atlas_run_effect;
//鐠哄疇绌悧瑙勬櫏閸斻劎鏁鹃崶楣冩肠
Atlas atlas_jump_effect;
//閽€钘夋勾閻楄鏅ラ崝銊ф暰閸ラ箖娉?
Atlas atlas_land_effect;
//1P 閼剧柉鍎ㄩ弬鍥ㄦ拱閸ュ墽澧?
IMAGE img_1P_winner;
//2P 閼剧柉鍎ㄩ弬鍥ㄦ拱閸ュ墽澧?
IMAGE img_2P_winner;
//閼剧柉鍎ㄩ悳鈺侇啀閺傚洦婀伴懗灞炬珯閸ュ墽澧?
IMAGE img_winner_bar;
//鐠炲矁鐪寸亸鍕婢舵潙鍎氶崶鍓у
IMAGE img_avatar_peashooter;
//姒瑦妫╅拋闈涖仈閸嶅繐娴橀悧?
IMAGE img_avatar_sunflower;
IMAGE img_menu_background;
