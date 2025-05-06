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

//1P �ı�

IMAGE img_1P;

//2P �ı�

IMAGE img_2P;

//1P ��λ����

IMAGE img_1P_desc;

//2P ��λ����

IMAGE img_2P_desc;

//�������Ĺ��

IMAGE img_gravestone_left;

//�����ҵ�Ĺ��

IMAGE img_gravestone_right;

//ѡ�ǽ�����ʾ��Ϣ�ı�

IMAGE img_selector_tip;

//ѡ�ǽ��汳��ͼ

IMAGE img_selector_background;

//1P ����ѡ��ťĬ��״̬

IMAGE img_1P_selector_btn_idle_left;

//1P ����ѡ��ťĬ��״̬

IMAGE img_1P_selector_btn_idle_right;

//1P ����ѡ��ť����״̬

IMAGE img_1P_selector_btn_down_left;

//1P ����ѡ��ť����״̬

IMAGE img_1P_selector_btn_down_right;

//2P ����ѡ��ťĬ��״̬

IMAGE img_2P_selector_btn_idle_left;

//2P ����ѡ��ťĬ��״̬

IMAGE img_2P_selector_btn_idle_right;

//2P ����ѡ��ť����״̬

IMAGE img_2P_selector_btn_down_left;

//2P ����ѡ��ť����״̬

IMAGE img_2P_selector_btn_down_right;

//ѡ�ǽ��泯������㶹����

IMAGE img_peashooter_selector_background_left;

//ѡ�ǽ��泯���ҵ��㶹����

IMAGE img_peashooter_selector_background_right;

//ѡ�ǽ��泯��������տ�

IMAGE img_sunflower_selector_background_left;

//ѡ�ǽ��泯���ҵ����տ�

IMAGE img_sunflower_selector_background_right;



//���

IMAGE img_sky;

//ɽ��

IMAGE img_hills;

//����ƽ̨

IMAGE img_platform_large;

//С��ƽ̨

IMAGE img_platform_small;



//1P ָʾ���

IMAGE img_1P_cursor;

//2P ָʾ���

IMAGE img_2P_cursor;



// �㶹����ͼ��

//�������Ĭ�϶���

Atlas atlas_peashooter_idle_left;

//�����ҵ�Ĭ�϶���

Atlas atlas_peashooter_idle_right;

//������ı��ܶ���

Atlas atlas_peashooter_run_left;

//�����ҵı��ܶ���

Atlas atlas_peashooter_run_right;

//����������⹥������

Atlas atlas_peashooter_attack_ex_left;

//�����ҵ����⹥������

Atlas atlas_peashooter_attack_ex_right;

//���������������

Atlas atlas_peashooter_die_left;

//�����ҵ���������

Atlas atlas_peashooter_die_right;



//���տ�ͼ��

//�������Ĭ�϶���

Atlas atlas_sunflower_idle_left;

//�����ҵ�Ĭ�϶���

Atlas atlas_sunflower_idle_right;

//������ı��ܶ���

Atlas atlas_sunflower_run_left;

//�����ҵı��ܶ���

Atlas atlas_sunflower_run_right;

//����������⹥������

Atlas atlas_sunflower_attack_ex_left;

//�����ҵ����⹥������

Atlas atlas_sunflower_attack_ex_right;

//���������������

Atlas atlas_sunflower_die_left;

//�����ҵ���������
Atlas atlas_sunflower_die_right;
//�㶹ͼƬ
IMAGE img_pea;
//�㶹���鶯��ͼ��
Atlas atlas_pea_break;
//�չ⶯��ͼ��
Atlas atlas_sun;
//�չⱬը����ͼ��
Atlas atlas_sun_explode;
//�����չ⶯��ͼ��
Atlas atlas_sun_ex;
//�����չⱬը����ͼ��
Atlas atlas_sun_ex_explode;
//"��"�ı�����ͼ��
Atlas atlas_sun_text;
//������Ч����ͼ��
Atlas atlas_run_effect;
//��Ծ��Ч����ͼ��
Atlas atlas_jump_effect;
//�����Ч����ͼ��
Atlas atlas_land_effect;
//1P ��ʤ�ı�ͼƬ
IMAGE img_1P_winner;
//2P ��ʤ�ı�ͼƬ
IMAGE img_2P_winner;
//��ʤ����ı�����ͼƬ
IMAGE img_winner_bar;
//�㶹����ͷ��ͼƬ
IMAGE img_avatar_peashooter;
//���տ�ͷ��ͼƬ
IMAGE img_avatar_sunflower;
IMAGE img_menu_background;
