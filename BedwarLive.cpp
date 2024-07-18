//UTF-8
//C++20
#ifdef __GNUC__
#define GCC		//-lwinmm
#endif

//#define QUICK_MODE
//#define BRIDGE_NO_CHOOSE

#define CURRENT_VERSION "v0.2"
#define SPEEDRATE 6.0
#define WORMWAKE_ROOM_ID 31196635

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <random>
#include <chrono>
#include <io.h>
#include "graphics.h"
#ifndef GCC
#pragma comment(lib, "graphics.lib")
#pragma comment(lib, "winmm.lib")
#endif
#define CJZAPI __stdcall
using namespace std;

#define MY_NAME "Wormwaker"
#define MY_NAME2 "Worrnwaker"

#define DEF_FONTNAME "Minecraft AE Pixel"

int uix{ 1920 };
int uiy{ 1080 };
double frac{ 1.0 };

#include "basic.h"
#include "cplayer.h"

struct Globals
{
	HWND hwnd{ nullptr };
	bool loaded{ false };
	bool retry{ false };
	bool playing{ false };

	clock_t timedown_s{ 10000L };
	vector<string> rank_score;	//分数总榜

	constexpr static int center_title_fs = 120;
	string center_title{ "" };
	color_t center_title_color{ GOLD };
	clock_t center_title_duration{ 5000L };
	clock_t center_title_start{ 0L };
	PIMAGE center_title_pimg{ nullptr };

	constexpr static int center_subtitle_fs = 60;
	string center_subtitle{ "" };
	color_t center_subtitle_color{ GOLD };
	clock_t center_subtitle_duration{ 5000L };
	clock_t center_subtitle_start{ 0L };
	PIMAGE center_subtitle_pimg{ nullptr };

	const string image_dir{ "images\\" };
	const string font_dir{ "fonts\\" };
	const string audio_dir{ "audio\\" };

	const vector<string> used_images
	{
		"title_0.jpg",
		"title_1.jpg",
		"home_1.jpg",
		"win.jpg",
		"view.png",
		"background_0.png",
		"CenterIsland.png",
		"RedHome.png",
		"BlueHome.png",
		"DiamondSpot.png",
		"Diamond.png",
		"Emerald.png",
		"RedWool.png",
		"BlueWool.png",
		"Endstone.png",
		"Plank.png",
		"RedGlass.png",
		"BlueGlass.png",
		"Obsidian.png",
		"Water.png",
		"RedBed.png",
		"BlueBed.png",
		"tile_red_wool.png",
		"tile_blue_wool.png",
		"player_0.png",
		"player_1.png",
		"player_2.png",
		"player_3.png",
		"player_4.png",
		"player_5.png",
		"entity_1.png",
		"entity_1_1.png",
		"entity_1_2.png",
		"entity_1_3.png",
		"entity_1_4.png",
		"entity_2.png",
		"entity_3.png",
		"entity_3b.png",
		"entity_4.png",
		"entity_4b.png",
		"entity_5.png",
		"entity_6.png",
		"entity_7.png",
		"entity_8.png",
		"entity_9.png",
		"entity_10.png",
		"entity_11.png",
		"entity_12.png",
		"entity_12b.png",
		"weapon_1.png",
		"weapon_2.png",
		"weapon_3.png",
		"weapon_4.png",
		"weapon_5.png",
		"pickaxe_1.png",
		"pickaxe_2.png",
		"pickaxe_3.png",
		"pickaxe_4.png",
		"pickaxe_5.png",
		"leather_armor.png",
		"chainmail_armor.png",
		"iron_armor.png",
		"diamond_armor.png",
		"netherite_armor.png",
		"leather_armor_b.png",
		"chainmail_armor_b.png",
		"iron_armor_b.png",
		"diamond_armor_b.png",
		"netherite_armor_b.png",
	};

	vector<string> lounge_bg_names
	{
		"title_0.jpg",
		"title_1.jpg",
		"win.jpg",
		"view.png",
		"home_1.jpg"
	};
}g;

void DrawVersion()
{
	setfont(25, 0, DEF_FONTNAME);
	if (clock() / 1000 % 60 == 0)
		setcolor(LIGHTRED);
	else
		setcolor(WHITE);
	xyprintf_shadow(10, uiy - 35, CURRENT_VERSION " - Wormwake Games");
}
void CenterTitle(string_view text, color_t color)
{
	g.center_title = str(text);
	g.center_title_color = color;
	g.center_title_start = clock();
	if (g.center_title_pimg)
		delimage(g.center_title_pimg);
	g.center_title_pimg = newimage(text.size() * g.center_title_fs / 2, g.center_title_fs);
	setbkcolor(EGERGBA(1, 1, 1, 0), g.center_title_pimg);
	cleardevice(g.center_title_pimg);
	setfont(g.center_title_fs, 0, DEF_FONTNAME, g.center_title_pimg);
	setcolor(color, g.center_title_pimg);
	outtextxy(0, 0, text.data(), g.center_title_pimg);
}
void CenterSubtitle(string_view text, color_t color)
{
	g.center_subtitle = str(text);
	g.center_subtitle_color = color;
	g.center_subtitle_start = clock();
	if (g.center_subtitle_pimg)
		delimage(g.center_subtitle_pimg);
	g.center_subtitle_pimg = newimage(text.size() * g.center_subtitle_fs / 2, g.center_subtitle_fs);
	setbkcolor(EGERGBA(1, 1, 1, 0), g.center_subtitle_pimg);
	cleardevice(g.center_subtitle_pimg);
	setfont(g.center_subtitle_fs, 0, DEF_FONTNAME, g.center_subtitle_pimg);
	setcolor(color, g.center_subtitle_pimg);
	outtextxy(0, 0, text.data(), g.center_subtitle_pimg);
}
void CenterTitleEx(string_view title, color_t color1, string_view subtitle, color_t color2)
{
	CenterTitle(title, color1);
	CenterSubtitle(subtitle, color2);
}

#include "sound.h"
#include "texttip.h"
#include "image.h"
#include "chat.h"

#define TEAM_BRIDGE_MAX_CNT 3
#define BRIDGE_BLOCK_CNT 18
#define BRIDGE_BLOCK_PX	 20
#define BRIDGE_TOP 460
#define BRIDGE_GAP 70
#define CENTERISLAND_LEFT	 (uix/2 - 250)
#define CENTERISLAND_RIGHT	 (uix/2 + 250)
#define BRIDGE_LEFT_LEFT	 (CENTERISLAND_LEFT - BRIDGE_BLOCK_CNT * BRIDGE_BLOCK_PX)	//350
#define BRIDGE_RIGHT_RIGHT	 (CENTERISLAND_RIGHT + BRIDGE_BLOCK_CNT * BRIDGE_BLOCK_PX)//1550

#define BED_TOP 460
#define BED_HP_TOP 510
#define RED_BED_LEFT 210
#define BLUE_BED_LEFT 1600
#define DIAMOND_SPOT_1_TOP 210
#define DIAMOND_SPOT_2_TOP (DIAMOND_SPOT_1_TOP + 430)
#define CENTERISLAND_TOP	(DIAMOND_SPOT_1_TOP + 140)
#define CENTERISLAND_Y		(DIAMOND_SPOT_1_TOP + 305)

enum TeamID : color_t
{
	RedTeam = RED,
	BlueTeam = BLUE,
};

POINT point_bridge_left_left[TEAM_BRIDGE_MAX_CNT];
POINT point_bridge_right_right[TEAM_BRIDGE_MAX_CNT];
POINT point_centerisland_left;
POINT point_centerisland_right;
POINT point_center;
map<TeamID,POINT> point_beds;

void InitUserStatsKeys();
void ReadUserStats();
void ReadGlobalStats();

void Load()
{
	if (g.loaded)
		return;
	cout << "Loading...\n";

	g.loaded = true;

	InitUserStatsKeys();
	ReadUserStats();
	ReadGlobalStats();

	for (auto const& imgname : g.used_images)
		imgMgr.Load(imgname);

	// 提前翻转好图片，空间换时间
	for (auto const& imgname : g.used_images)
	{
		if (imgname.length() <= 7
			|| strhead(imgname, 7) != "entity_"
			&& strhead(imgname, 7) != "player_"
			&& strhead(imgname, 7) != "pickaxe"
			&& strhead(imgname, 7) != "weapon_"
			&& imgname.find("_armor") == string::npos
			)
			continue;
		//水平翻转图片
		PIMAGE pimg0 = imgMgr.images[imgname];
		PIMAGE pimg = newimage(getwidth(pimg0), getheight(pimg0));
		const color_t* buffer0 = getbuffer(pimg0);
		color_t* buffer = getbuffer(pimg);
		for (int y = 0; y < getheight(pimg0); ++y)
			for (int x = 0; x < getwidth(pimg0); ++x)
			{
				buffer[y * getwidth(pimg0) + x]
					= buffer0[(y + 1) * getwidth(pimg0) - x - 1];
			}
		imgMgr.Add(pimg, strxtail(imgname, 4) + "left.png");
	}

	point_beds.insert(make_pair(RedTeam, POINT{ RED_BED_LEFT, BED_TOP }));
	point_beds.insert(make_pair(BlueTeam, POINT{ BLUE_BED_LEFT, BED_TOP }));
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
	{
		point_bridge_left_left[i].x = BRIDGE_LEFT_LEFT;
		point_bridge_left_left[i].y = BRIDGE_TOP + BRIDGE_GAP * i - 50;

		point_bridge_right_right[i].x = BRIDGE_RIGHT_RIGHT;
		point_bridge_right_right[i].y = BRIDGE_TOP + BRIDGE_GAP * i - 50;
	}
	point_centerisland_left.x = CENTERISLAND_LEFT;
	point_centerisland_left.y = CENTERISLAND_Y;

	point_centerisland_right.x = CENTERISLAND_RIGHT;
	point_centerisland_right.y = CENTERISLAND_Y;

	point_center.x = uix / 2;
	point_center.y = CENTERISLAND_Y;
}

void SaveUserStats();
void SaveGlobalStats();

void Unload()
{
	if (!g.loaded)
		return;
	cout << "Unloading...\n";

	imgMgr.Dispose();

	if (g.center_title_pimg)
		delimage(g.center_title_pimg);
	if (g.center_subtitle_pimg)
		delimage(g.center_subtitle_pimg);

	SaveUserStats();
	SaveGlobalStats();

	g.loaded = false;
}
//////////////////////////////////////////////////////
#include "globalstat.h"
#include "bedwar.h"

inline void DrawBackground(const char* name, int alpha = -1)
{
	/*putimage(uix / 2 - getwidth(imgMgr.images[name]) / 2,
			 uiy / 2 - getheight(imgMgr.images[name]) / 2,
			 imgMgr.images[name]);*/
	int w = getwidth(imgMgr.images[name]);
	int h = getheight(imgMgr.images[name]);
	double scale{ 1.0 };

	//Uniform
	if (uix / double(w) > uiy / double(h))
	{
		scale = uiy / double(h);
	}
	else {
		scale = uix / double(w);
	}

	//putimage(uix / 2 - w / 2 * scale,
	//	uiy / 2 - h / 2 * scale, w * scale, h * scale, imgMgr.images[name],
	//	0, 0, w, h);
	putimage_rotatezoom(nullptr, imgMgr.images[name],
		uix / 2 - w / 2 * scale,
		uiy / 2 - h / 2 * scale, 0.0f, 0.0f, 0.0f,
		scale,
		false,
		alpha);
}
void DrawChat()
{
	chat.Display();
}
void DrawTeamUpgradeTexts(int x, int y, TeamID team)
{
	constexpr int fs = 24;

	string team_upgrades;
	for (int i = 0; i < team_upgrade_names.size(); ++i)
	{
		if (bw.teams[team].team_upgrade_levels.at(i) >= team_upgrade_prices.at(i).size())
		{
			team_upgrades += team_upgrade_names.at(i) + "\t已满级\t("
				+ roman_levels.at(bw.teams[team].team_upgrade_levels.at(i)) + ")\n";
		}
		else {
			team_upgrades += team_upgrade_names.at(i);
			if (bw.teams[team].team_upgrade_levels.at(i) > 0)
			{
				team_upgrades += roman_levels.at(bw.teams[team].team_upgrade_levels.at(i)+1);
			}
			team_upgrades += "\t-"
				+ str(team_upgrade_prices.at(i).at(bw.teams[team].team_upgrade_levels.at(i)))
				+ "钻石\t";
			team_upgrades += '\n';
		}
		
	}

	setfont(fs, 0, DEF_FONTNAME);
	setcolor(GREEN);
	xyprintf_stroke(x, y, "团队升级 扣字选择升级");
	setwcolor(ROYALBLUE);
	rectprintf_shadow(x, y + fs, 400, 400,
		team_upgrades.c_str());
}
void DrawBlockPrices()
{
	constexpr int fs = 28;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(GOLD);
	xyprintf_stroke(700, 710, "床防御方块价目表，扣方块名称购买");
	setcolor(BLACK);

	static string block_price_text{ "" };
	if (block_price_text.empty())
	{
		for (int b = 1; b < block_names.size(); ++b)
		{
			block_price_text += block_names.at(b) + "x";
			block_price_text += str(block_amount.at(b)) + "\t-";
			block_price_text += str(block_prices.at(b)) + "经验\n";
		}
	}
	rectprintf(700, 750 + fs, 500, 500, block_price_text.c_str());
}

void DrawItemPrices(int x, int y)
{
	string items{ "" };
	for (int i = 0; i < item_names.size(); ++i)
	{
		items += item_names.at(i);
		if (item_amounts.at(i) > 1)
			items += "x" + str(item_amounts.at(i));
		items += "\t-" + str(item_prices.at(i)) + "经验\n";
	}

	constexpr int fs = 24;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(ORANGE);
	xyprintf_stroke(x, y, "物品商店 扣\"购买+物品名称\" 使用物品则是直接扣物品名称");
	setcolor(PINK);
	rectprintf_shadow(x, y + fs, 400, 500,
		items.c_str());

}
void DrawPickaxePrices(int x, int y)
{

	string pickaxes{};
	int i = 0;
	for (const auto& name : pickaxe_names)
	{
		if (i == 0)
		{
			++i;
			continue;
		}
		
		pickaxes += name + "\t-";
		pickaxes += str(pickaxe_prices[i]) + "经验\n";

		++i;
	}
	constexpr int fs = 24;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(ORANGE);
	xyprintf_stroke(x, y, "镐子商店 请直接输入\"升级镐子\"");

	setwcolor(EGERGB(127, 255, 170));
	rectprintf_shadow(x, y + fs, 400, 500,
		pickaxes.c_str());
}
void DrawSwordPrices(int x, int y)
{
	string swords{};
	int i = 0;
	for (const auto& name : sword_names)
	{
		if (i == 0)
		{
			++i;
			continue;
		}
			
		swords += name + "\t-";
		swords += str(sword_prices[i]) + "经验\n";

		++i;
	}
	constexpr int fs = 24;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(ORANGE);
	xyprintf_stroke(x, y, "武器商店 扣名称购买");
	setwcolor(EGERGB(240, 230, 140));
	rectprintf_shadow(x, y + fs, 400, 500,
		swords.c_str());
}
void DrawArmorPrices(int x, int y)
{
	string armors{};
	int i = 0;
	for (const auto& name : armor_names)
	{
		if (i == 0)
		{
			++i;
			continue;
		}

		armors += name + "\t-";
		armors += str(armor_prices[i]) + "经验\n";

		++i;
	}
	constexpr int fs = 24;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(MAGENTA);
	xyprintf_stroke(x, y, "护甲商店 扣名字购买永久护甲");
	setwcolor(EGERGB(240, 230, 140));
	rectprintf_shadow(x, y + fs, 400, 500,
		armors.c_str());
}
void DrawArmyPrices(int x, int y)
{
	constexpr int fs = 22;

	setfont(fs, 0, DEF_FONTNAME);
	setwcolor(GOLD);
	xyprintf_stroke(x, y, "军队商店 扣名称购买");

	string army_text{""};

	int e = 0;
	for (const auto& name : entity_names)
	{
		if (e == 0 || entity_types[e] == ProjectileType)
		{
			++e;
			continue;
		}

		army_text += name;
		army_text += "\t-" + str(entity_prices[e]) + "经验\n";

		++e;
	}

	setcolor(EGERGB(216, 191, 216));
	rectprintf_shadow(x, y + fs, 400, 400,
		army_text.c_str());
}
void DrawTeamItems(int x, int y, TeamID team)
{
	string s{ "" };
	bool empty{ true };
	for (int i = 0; i < bw.teams[team].items.size(); ++i)
		if (bw.teams[team].items.at(i))
		{
			empty = false;
			s += item_names.at(i) + "x" + str(bw.teams[team].items.at(i)) + '\n';
		}

	constexpr int fs = 24;
	setfont(fs, 0, DEF_FONTNAME);

	if (!empty)
	{
		setcolor(RainbowColor());
		xyprintf_shadow(x, y, "团队物品");
	}
	
	setcolor(team);
	rectprintf_shadow(x, y + fs, 400, 400, s.c_str());
}
void DrawStaticTexts()
{
	constexpr int text_fs = 20;
	static const char* text_fontname = "Minecraft AE Pixel";

	setfont(text_fs, 0, text_fontname);
	setcolor(RainbowColor());
	xyprintf_shadow(50, 10, "Bedwar Interactive Live");
	setcolor(WHITE);
	xyprintf_shadow(50, 30, "Wormwaker's Bilibili Live");

	/*constexpr int fs = 28;
	static const int below_top = uiy * 0.85;
	constexpr int below_w = 400;
	setfont(fs, 0, DEF_FONTNAME);
	setcolor(RainbowColor());
	rectprintf_shadow(50, below_top, below_w, uiy - below_top - 30,
		"扣\"加入+队名\": 加入队伍，红队或蓝队\n"
		"扣\"回头\": 使人物回头\n"
		"扣\"向左\"或\"向右\"：改变人物朝向\n"
		"扣\"回家\": 往家走\n"
		"扣\"停住\"：停止走路\n"
		"扣\"起步\"：继续走路\n"
		"扣\"退出\": 退出队伍\n"

		);*/
	constexpr int help_fs = 18;
	setfont(help_fs, 0, text_fontname);
	setcolor(RainbowColor());
	rectprintf_shadow(350, 10, 1500, 300, "扣\"加入+队名\": 加入队伍，红队或蓝队 | "
		"扣\"回头\": 使人物回头 | "
		"扣\"向左\"或\"向右\"：改变人物朝向 | "
		"扣\"去红家\"或\"去蓝家\"：改变人物朝向 | "
		"扣\"回家\": 往家走 | "
		"扣\"停住\"：停止走路\n"
		"扣\"起步\"：继续走路 | "
		"扣\"退出\": 退出队伍 | "
		"扣\"帮助+关键词\"查看相关帮助 "
	);

	constexpr int tupg_top = 130;
	DrawTeamUpgradeTexts(10, tupg_top, RedTeam);
	DrawTeamUpgradeTexts(1640, tupg_top, BlueTeam);

	DrawBlockPrices();

	constexpr int inven_top = 380;
	DrawTeamItems(10, inven_top, RedTeam);
	DrawTeamItems(1640, inven_top, BlueTeam);
	
	constexpr int lui_left = 40;
	constexpr int rui_left = 1700;

	constexpr int diamond_fs = 40;
	setfont(diamond_fs, 0, "微软雅黑");
	setcolor(AQUA);
	xyprintf_stroke(lui_left, 330, "钻石: %ld", bw.teams[RedTeam].diamonds);
	xyprintf_stroke(rui_left, 330, "钻石: %ld", bw.teams[BlueTeam].diamonds);

	constexpr int exp_fs = 50;
	setcolor(GOLD);
	setfont(exp_fs, 0, "微软雅黑");
	xyprintf_stroke(lui_left, 280, "经验：%ld", bw.teams[RedTeam].exp);
	xyprintf_stroke(rui_left, 280, "经验：%ld", bw.teams[BlueTeam].exp);

	bw.teams[RedTeam].DrawBedBlocks(100, 640);
	bw.teams[BlueTeam].DrawBedBlocks(1600, 640);

	//bw.teams[RedTeam].DrawBridgeText(330, 300);
	//bw.teams[BlueTeam].DrawBridgeText(1250, 300);
	DrawBridgeText(300, 100);
	DrawItemPrices(1100, 50);
	DrawPickaxePrices(1380, 75);
	DrawSwordPrices(20, 800);
	DrawArmorPrices(650, 800);
	DrawArmyPrices(380, 800);
}
#define DEF_BED_BLOCK_PX 50
#define RED_BED_DEFENSE_X_OFFSET 0
#define BLUE_BED_DEFENSE_X_OFFSET -55
#define DRAW_BED_DEFENSE(BlockName)		\
		if (bw.teams[team].bed.defense[BlockName] > 0)\
		{\
			for (int i = 0; i < min(8, bw.teams[team].bed.defense[BlockName]); ++i)\
			{	\
				putimage_withalpha(nullptr, imgMgr.images[#BlockName ".png"], \
					x + (team == RedTeam ? RED_BED_DEFENSE_X_OFFSET : BLUE_BED_DEFENSE_X_OFFSET) + (team == RedTeam ? 1 : -1 ) * def_pos_x[i], y + def_pos_y[i],\
					DEF_BED_BLOCK_PX, DEF_BED_BLOCK_PX,	\
					0, 0, getwidth(imgMgr.images[#BlockName ".png"]), getheight(imgMgr.images[#BlockName ".png"]));	\
			}\
		}
#define DRAW_BED_DEFENSE_TEAM(BlockName) \
		if (bw.teams[team].bed.defense[BlockName] > 0)\
		{\
			for (int i = 0; i < min(8, bw.teams[team].bed.defense[BlockName]); ++i)\
			{\
				putimage_withalpha(nullptr, imgMgr.images[(team == RedTeam ? "Red"s : "Blue"s) + #BlockName ".png"],\
					x + (team == RedTeam ? RED_BED_DEFENSE_X_OFFSET : BLUE_BED_DEFENSE_X_OFFSET) + (team == RedTeam ? 1 : -1 ) * def_pos_x[i], y + def_pos_y[i],\
					DEF_BED_BLOCK_PX, DEF_BED_BLOCK_PX,\
					0, 0, getwidth(imgMgr.images[(team == RedTeam ? "Red"s : "Blue"s) + #BlockName ".png"]), getheight(imgMgr.images[(team == RedTeam ? "Red"s : "Blue"s) + #BlockName ".png"]));\
			}\
		}
void DrawBedDefense(int x, int y, TeamID team)
{
	int def_pos_x[]
	{-65, -70, -47, -20,		2, 2, -48, -25};
	int def_pos_y[]
	{-30, 20, 33, -20, 			0, 30, -18, -4};

	DRAW_BED_DEFENSE(Obsidian)
	DRAW_BED_DEFENSE(Endstone)
	DRAW_BED_DEFENSE(Plank)
	DRAW_BED_DEFENSE_TEAM(Glass)
	DRAW_BED_DEFENSE_TEAM(Wool)
	if (bw.teams[team].bed.defense[Water] > 0)
	{
		putimage_withalpha(nullptr, imgMgr.images["Water.png"],
			x + (team == RedTeam ? RED_BED_DEFENSE_X_OFFSET : BLUE_BED_DEFENSE_X_OFFSET) - 40,
			y);
	}
}
void DrawBedHp(int x, int y, TeamID team)
{
	constexpr int bar_w = 110;
	constexpr int bar_h = 9;
	constexpr color_t bar_outline_color = BLACK;
	constexpr color_t bar_color = HPBAR_COLOR;
	constexpr color_t bar_color2 = HPBAR_COLOR2;
	constexpr color_t bar_color_lost = BLACK;

	double rate = bw.teams[team].bed.GetHpRate();

	setfillcolor(bar_color);
	bar(x - bar_w / 2, y, x - bar_w / 2 + rate * bar_w, y + bar_h);
	setfillcolor(bar_color2);
	bar(x - bar_w / 2, y + bar_h * 0.6, x - bar_w / 2 + rate * bar_w, y + bar_h);
	setfillcolor(bar_color_lost);
	bar(x - bar_w / 2 + rate * bar_w, y, x - bar_w / 2 + rate * bar_w + (1.0 - rate) * bar_w, y + bar_h);
	setcolor(bar_outline_color);
	rectangle(x - bar_w / 2, y, x + bar_w / 2, y + bar_h);
}

void DrawIslands()
{
	constexpr int bed_top = BED_TOP;
	constexpr int island_top = DIAMOND_SPOT_1_TOP;

	putimage_withalpha(nullptr, imgMgr.images["DiamondSpot.png"],
		uix / 2 - 0.6 * getwidth(imgMgr.images["DiamondSpot.png"]) / 2, island_top,
		0.6 * getwidth(imgMgr.images["DiamondSpot.png"]), 0.6 * getheight(imgMgr.images["DiamondSpot.png"]),
		0, 0, getwidth(imgMgr.images["DiamondSpot.png"]), getheight(imgMgr.images["DiamondSpot.png"]));

	putimage_withalpha(nullptr, imgMgr.images["DiamondSpot.png"],
		uix / 2 - getwidth(imgMgr.images["DiamondSpot.png"]) / 2, 
		DIAMOND_SPOT_2_TOP);

	static int diamond_w = getwidth(imgMgr.images["Diamond.png"]);

	for (int j = 0; j < 2; ++j)
	for (int i = 0; i < bw.diamond_cnt[j]; ++i)
	{
		int x = uix / 2 - (bw.diamond_cnt[j] - i) / 2 * diamond_w * 0.3;
		putimage_withalpha(nullptr, imgMgr.images["Diamond.png"],
			x,
			//island_top + 20 + 460 * j
			(j == 0 ? DIAMOND_SPOT_1_TOP : DIAMOND_SPOT_2_TOP) + 20
		);
	}

	putimage_withalpha(nullptr, imgMgr.images["CenterIsland.png"],
		uix / 2 - getwidth(imgMgr.images["CenterIsland.png"]) / 2, 
		CENTERISLAND_TOP);

	static int emerald_w = getwidth(imgMgr.images["Emerald.png"]);

	for (int i = 0; i < bw.emerald_cnt; ++i)
	{
		int x = uix / 2 - (bw.emerald_cnt - i) / 2 * emerald_w * 0.6;
		putimage_withalpha(nullptr, imgMgr.images["Emerald.png"],
			x, 
			CENTERISLAND_Y);
	}

	putimage_withalpha(nullptr, imgMgr.images["RedHome.png"],
		-150, bed_top - 220);
	putimage_withalpha(nullptr, imgMgr.images["BlueHome.png"],
		uix - getwidth(imgMgr.images["BlueHome.png"]) + 150, bed_top - 220);

	if (bw.teams[RedTeam].bed.IsActive())
	{
		putimage_withalpha(nullptr, imgMgr.images["RedBed.png"],
			point_beds[RedTeam].x - 40, bed_top - 40);

		DrawBedDefense(point_beds[RedTeam].x, bed_top, RedTeam);
		DrawBedHp(point_beds[RedTeam].x, BED_HP_TOP, RedTeam);
	}

	if (bw.teams[BlueTeam].bed.IsActive())
	{
		putimage_withalpha(nullptr, imgMgr.images["BlueBed.png"],
			point_beds[BlueTeam].x - 40, bed_top - 40);
		DrawBedDefense(point_beds[BlueTeam].x, bed_top, BlueTeam);
		DrawBedHp(point_beds[BlueTeam].x, BED_HP_TOP, BlueTeam);
	}
}

void DrawBridges()
{

	for (auto const& team : bw.teams)
	{
		for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		{
			//if (team.second.bridges[i].completion)
			{
				/*xyprintf_shadow(team.first==RedTeam?280:1500, 300,
					"桥 %c: %d", 'A'+i, team.second.bridges[i]);*/
				auto left_base = team.first == RedTeam ? BRIDGE_LEFT_LEFT : CENTERISLAND_RIGHT;
				/*auto right_base = team.first == RedTeam ? CENTERISLAND_LEFT : BRIDGE_RIGHT_RIGHT;*/
				
				for (int j = 0; j < BRIDGE_BLOCK_CNT; ++j)
				{
					if (team.second.bridges[i].blocks[j] == Air)
						continue;
					PCIMAGE pimg = imgMgr.images[bridge_block_tile_imgnames[team.second.bridges[i].blocks[j]]];
					if (!pimg)	continue;

					putimage(left_base + j * getwidth(pimg),
						BRIDGE_TOP + i * BRIDGE_GAP,
						pimg);
				}
				

				/*if (team.second.bridges[i].build_dir == RIGHT)
					for (int j = 0; j < BRIDGE_BLOCK_CNT * team.second.bridges[i].completion / 100.0; ++j)
					{
						putimage(left_base + j * getwidth(pimg), BRIDGE_TOP + i * BRIDGE_GAP,
							pimg);
					}
				else if (team.second.bridges[i].build_dir == LEFT)
					for (int j = BRIDGE_BLOCK_CNT * team.second.bridges[i].completion / 100.0; j >= 0; --j)
					{
						putimage(right_base - j * getwidth(pimg), BRIDGE_TOP + i * BRIDGE_GAP,
							pimg);
					}*/
			}
		}
	}
}

void DrawMap()
{
	DrawIslands();
	DrawBridges();
}
void DrawEntities()
{
	for (auto& pet : bw.entities)
		pet->Draw();
}
void DrawBridgeCanPass()
{
	constexpr int y = 320;
	constexpr int gap = 25;

	setfont(25, 0, DEF_FONTNAME);
	auto that = [&](TeamID team)
	{
		setcolor(team);

		int x = team == RedTeam ? 300 : 1500;
		
		bool can[TEAM_BRIDGE_MAX_CNT]{ false };
		for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		{
			can[i] = bw.teams[team].bridges[i].CanPass();
			xyprintf_shadow(x, y + i * gap, "%d", can[i]);
		}
	};

	that(RedTeam);
	that(BlueTeam);
}
#define CTITLE_FADE_TIME 1000L
void DrawCenterTitles()
{
	if (bw.over)
		return;

	if (g.center_title_pimg 
		&& clock() - g.center_title_start < g.center_title_duration)
	{
		double rat = GetFadeinoutRate(CTITLE_FADE_TIME, g.center_title_duration, clock() - g.center_title_start);
		/*setfont(g.center_title_fs, 0, DEF_FONTNAME);
		setcolor(EGECOLORA(g.center_title_color, int(255 * rat)));
		midprinty_shadow(g.center_title, uiy / 2 - g.center_title_fs / 2 - 30);*/
		putimage_alphablend(nullptr, g.center_title_pimg, uix / 2 - getwidth(g.center_title_pimg) / 2,
			uiy / 2 - g.center_title_fs / 2 - 30, 255 * rat);
	}
	if (g.center_subtitle_pimg
		&& clock() - g.center_subtitle_start < g.center_subtitle_duration)
	{
		double rat = GetFadeinoutRate(CTITLE_FADE_TIME, g.center_subtitle_duration, clock() - g.center_subtitle_start);
		/*setfont(g.center_subtitle_fs, 0, DEF_FONTNAME);
		setcolor(EGECOLORA(g.center_subtitle_color, int(255 * rat)));
		midprinty_shadow(g.center_subtitle, uiy / 2 + g.center_title_fs / 2 - 10);*/
		putimage_alphablend(nullptr, g.center_subtitle_pimg, uix / 2 - getwidth(g.center_subtitle_pimg) / 2,
			uiy / 2 - g.center_subtitle_fs / 2 - 30, 255 * rat);
	}
}
void DrawScene()
{

	cls();

	DrawBackground("background_0.png");
	DrawMap();
	DrawEntities();

	DrawStaticTexts();
	DrawBridgeCanPass();
	DrawTextTips();
	DrawChat();
	DrawCenterTitles();
	DrawGameOver();

	//setfont(24, 0, DEF_FONTNAME);
	//setcolor(RainbowColor());
	//midprinty_shadow(str(frac), 80);

	delay_fps(60);
}
#define DRAW_CD	 0L
#define UPDATE_CD (1000 / 60.0)
#define INPUT_CD 300L
void InGame()
{
	clock_t lastDraw{ clock() };
	clock_t lastUpdate{ clock() };
	clock_t lastInput{ clock() };

	Sound(SndLevelup);
	g.playing = true;
	while (g.playing)
	{
		if (clock() - lastDraw >= DRAW_CD)
		{
			DrawScene();
			lastDraw = clock();
		}

		if (clock() - lastUpdate >= UPDATE_CD)
		{
			GameUpdate();
			lastUpdate = clock();
		}
		if (clock() - lastInput >= INPUT_CD)
		{
			GameInputCheck();
			lastInput = clock();
		}

		GameControl();
	}
	g.playing = false;
}

void DrawLoungeTimedown()
{
	clock_t sec = g.timedown_s;
	if (sec > 60)
		return;

	setfont(40, 0, DEF_FONTNAME);
	color_t color{ EGERGB(255, 255, 255)};
	if (sec <= 5)
		color = LIGHTRED;
	else if (sec <= 10)
		color = GOLD;
	else if (sec <= 20)
		color = YELLOW;
	
	setcolor(color);
	xyprintf_shadow(200, uiy * 0.8, str(sec).c_str());
}

#define PLAYER_RANK_ITEM_CNT 10U
#define LOUNGE_ANIMATION_DURATION 10000L	//倒计时

void DrawLoungeRank(clock_t start)
{
	static int mid2 = uix * 0.85;
	constexpr int rank_fs = 28;
	constexpr int rank_top = 200;
	constexpr int rank_top2 = 300;
	constexpr int rank_v_add = 70;

	double rat = Clamp((clock() - start) / double(LOUNGE_ANIMATION_DURATION), 0.0, 1.0);

	setfont(rank_fs, 0, DEF_FONTNAME);
	int draw_cnt{ 0 };
	for (int i = 0; i < min(PLAYER_RANK_ITEM_CNT, g.rank_score.size()); ++i)
	{
		string name = g.rank_score.at(i);

		if (user_stats[name].stat["score"] <= 0)
			break;	//0 分不能进入榜单

		int _x = Lerp(uix + 100 * i, mid2, EaseOutCubic(rat));
		++draw_cnt;
		setwcolor(DARKGRAY, 0.8f);
		midprinty_shadow("#" + str(i + 1), rank_top2 + rank_v_add * i - rank_fs * 0.9, _x);
		setcolor(EGECOLORA(GetStringColor(name), int(255 * rat)));
		midprinty_shadow(name + ": " + str(user_stats[name].stat["score"]), rank_top2 + rank_v_add * i, _x);
	}
	if (draw_cnt > 0)
	{
		setfont(40, 0, DEF_FONTNAME);
		setcolor(EGECOLORA(ROYALBLUE, int(255 * rat)));
		midprinty_shadow("总分榜", rank_top, Lerp(uix + 50, mid2, EaseOutCubic(rat)));
	}
	setfont(30, 0, DEF_FONTNAME);
	setcolor(WHITE);
	midprinty_shadow("档案数：" + str(user_stats.size()), uiy * 0.8, mid2);
}
void DrawLounge(string_view bg_name, clock_t start)
{
	double rat = Clamp((clock() - start) / double(LOUNGE_ANIMATION_DURATION), 0.0, 1.0);

	cls();

	DrawBackground(bg_name.data(), 255 * rat);
	imagefilter_blurring(nullptr, Lerp(0x01, 0xFF, rat), 0x50);

	constexpr int title_fs = 75;
	constexpr int text_fs = 30;
	constexpr int intro_fs = 24;
	constexpr int text_v_add = 50;
	constexpr int author_fs = 24;
	constexpr int text_top = 400;
	
	static int mid = uix * 0.65;
	static int intro_top0 = uiy * 0.8;

	setfont(title_fs, 0, DEF_FONTNAME);
	setcolora(StepColor(EGERGB(10, 0, 0), LIGHTRED, rat), rat);
	midprinty_shadow("起床战争 弹幕互动版", 50);
	setfont(author_fs, 0, DEF_FONTNAME);
	setcolora(GREEN, rat);
	midprinty_shadow("@Wormwaker " + str(WORMWAKE_ROOM_ID), 60 + title_fs);

	//绘制提示
	setfont(text_fs, 0, DEF_FONTNAME);
	setwcolora(InvertedColor(RainbowColor()), rat);
	midprinty_shadow("发弹幕加入游戏！", text_top, mid);
	setwcolora2(YELLOW, rat, 1.5f);
	midprinty_shadow("\"加入\"(随机分配)", text_top + text_v_add, mid);
	midprinty_shadow("\"加入红队\"", text_top + text_v_add * 2, mid);
	midprinty_shadow("\"加入蓝队\"", text_top + text_v_add * 3, mid);

	//绘制统计信息
	setfont(40, 0, DEF_FONTNAME);
	setwcolor(EGERGB(255, 255, 255));
	xyprintf_shadow(mid - 30, text_top - 55, ("# "+str(globalstats["game_cnt"]+1)).c_str());


	//绘制游戏介绍
	setfont(intro_fs, 0, DEF_FONTNAME);
	setcolora(RainbowColor(), rat);
	int intro_top = Lerp(uiy, intro_top0, EaseOutCubic(rat));
	midprinty_shadow("基本任务：保护自己的床，摧毁对面的床，并剿灭敌方玩家", intro_top);
	midprinty_shadow("附加内容：可以购买和使用物品、团队升级、派遣怪物帮你作战", intro_top + intro_fs);
	midprinty_shadow("游戏内基本弹幕指令：查看帮助、羊毛、末地石、木板、玻璃（保护床）、蹲搭、蹲起搭（搭桥）、",
		intro_top + intro_fs * 2);
	midprinty_shadow("回头、回家、向左、向右、停下、继续走、购买烈焰弹、烈焰弹（使用）、",
		intro_top + intro_fs * 3);
	midprinty_shadow("升级镐子、捡钻石、护甲强化、锋利附魔、TNT、僵尸、骷髅、蠹虫、苦力怕、卫道士……",
		intro_top + intro_fs * 4);

	//绘制两队队员
	constexpr int team_fs = 28;
	constexpr int team_v_add = 40;
	constexpr int team_left0 = 200;
	constexpr int red_top = 300;
	int blue_top = red_top + 100 + bw.teams[RedTeam].player_names.size() * team_v_add;
	int team_left = Lerp(-10, team_left0, EaseOutCubic(rat));

	setfont(team_fs, 0, DEF_FONTNAME);
	setwcolor(CYAN);
	xyprintf_shadow(team_left, red_top - 100, "两队都有玩家时游戏开始");

	TeamID teams[]{ RedTeam, BlueTeam };
	for (TeamID t : teams)
	{
		setfont(30, 0, DEF_FONTNAME);
		setwcolora(ORANGE, rat, 0.3f);
		xyprintf_shadow(team_left, (t == RedTeam ? red_top : blue_top) - 40, "< %s =================", team_names[t].c_str());
		setcolora(t, rat);
		int i = 0;
		for (auto const& pname : bw.teams[t].player_names)
		{
			xyprintf_shadow(team_left, (t == RedTeam ? red_top : blue_top) + i * team_v_add, pname.c_str());
			++i;
		}
	}
	int random_top = blue_top + 100 + bw.teams[BlueTeam].player_names.size() * team_v_add;
	setfont(30, 0, DEF_FONTNAME);
	setwcolora(MAGENTA, rat, 0.3f);
	xyprintf_shadow(team_left, random_top - 40, "< 随机入队 =================");
	setcolora(PINK, rat);
	int i{ 0 };
	for (auto const& pname : bw.random_team_players)
	{
		xyprintf_shadow(team_left, random_top + i * team_v_add, pname.c_str());
		++i;
	}

	DrawLoungeTimedown();

	//绘制榜单
	DrawLoungeRank(start);
	DrawChat();
	DrawVersion();

	delay_fps(60);
}

bool ShouldLeaveLounge()
{
	return !bw.teams[RedTeam].player_names.empty() && !bw.teams[BlueTeam].player_names.empty();
}
void UpdateLoungeTimedown()
{
	static clock_t lastTick{ 0L };

	bool has = g.timedown_s > 0 && g.timedown_s <= 60;

	if (clock() - lastTick > 1000L && has)
	{
		g.timedown_s--;
		lastTick = clock();
		if (g.timedown_s <= 5)
			Sound(SndPling);
	}
		

	if (!has && ShouldLeaveLounge())
	{
		Sound(SndPling);
#ifdef GAME_RELEASE
		g.timedown_s = 10L;
#else
		g.timedown_s = 10L;
#endif
	}
	else if (has && !ShouldLeaveLounge() ){
		g.timedown_s = 99L;
	}
}
void Lounge()
{	//休息室，用于匹配玩家
	string bg_name = Choice(g.lounge_bg_names);

	UpdateRanks();

	g.timedown_s = 99L;

	clock_t start{ clock() };
	clock_t lastDraw{ clock() + DRAW_CD};
	while (1)
	{
		if (clock() - lastDraw >= DRAW_CD)
		{
			DrawLounge(bg_name, start);
			lastDraw = clock();
		}

		if (g.timedown_s <= 0)
		{	//倒计时结束
			break;
		}
		UpdateLoungeTimedown();

		GameInputCheck();

		if (K(VK_ESCAPE))
		{
			DisposeGame();
			closegraph();
			Unload();
			exit(0);
		}
	}
}

void Start()
{
	cls();

	DrawBackground("title_0.jpg");

	setfont(100, 0, "Broadway");
	setcolor(LIGHTRED);
	midprinty_shadow("Bedwar Live", 400);

	setfont(40, 0, "Microsoft YaHei UI");
	setcolor(PINK);
	midprinty_shadow("@Wormwaker", 520);

	setfont(80, 0, "Consolas");
	setcolor(YELLOW);
	midprinty_shadow("[PRESS ANY KEY TO START]", uiy * 0.8);

	DrawVersion();

	delay_fps(60);

	Sound(SndOrb);

	getch();

	do {
		g.retry = false;

		InitGame();
#ifndef QUICK_MODE
		Lounge();
#endif
		
		StartGame();
		InGame();
		DisposeGame();
	} while (g.retry);
	
}
// 自定义的异常处理函数
LONG WINAPI MyXcptFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	return EXCEPTION_CONTINUE_EXECUTION;	//强行继续
}
int main()
{
	SetProcessDPIAware();
	SetUnhandledExceptionFilter(MyXcptFilter);
	PreventSetUnhandledExceptionFilter(MyXcptFilter);
	//system("chcp 65001");

	setcodepage(EGE_CODEPAGE_UTF8);
	initgraph(uix, uiy, INIT_NOBORDER | INIT_RENDERMANUAL);
	setcaption("BedwarLive 起床战争弹幕互动版 - Wormwaker's Bilibili Live");
	setbkcolor(BLACK);
	setbkmode(TRANSPARENT);
	srand(time(NULL));

	g.hwnd = getHWnd();
	Load();
	

	Start();

	closegraph();
	Unload();

	return 0;
}
