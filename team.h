#pragma once

vector<float> furnace_cd_rates
{
	1.0f,
	0.9f,
	0.8f,
	0.7f,
};

enum BridgeBlockType {
	Air				= 0,
	RedWool			= 1,
	BlueWool		= 2,
};

map<TeamID, BridgeBlockType> team_wool
{
	{RedTeam, RedWool},
	{BlueTeam, BlueWool},
};

map<BridgeBlockType, string> bridge_block_tile_imgnames
{
	{RedWool, "tile_red_wool.png"},
	{BlueWool, "tile_blue_wool.png"},
};

inline BridgeBlockType TeamWool(TeamID team)
{
	return team_wool[team];
}

class Bridge
{
public:
	BridgeBlockType blocks[BRIDGE_BLOCK_CNT]{ Air };
	//int left_completion{ 0 };	//0-100
	//int right_completion{ 0 };	//0-100
	DIR2 build_dir{ RIGHT };			//最后一次延伸方向

	bool building{ false };
	BridgingMethod method{ NinjaBridging };
	string builder{ "" };

	mutable bool can_pass{ false };
	clock_t lastBuildBegin{ 0L };
	mutable clock_t lastUpdateCanPass{ 0L };

	double GetBridgingRate() const
	{
		return Clamp((clock() - lastBuildBegin) / double(bridging_durations[method]), 0.0, 1.0);
	}
	int GetCompletion() const
	{
		return BRIDGE_BLOCK_CNT - count(begin(blocks), end(blocks), Air);
	}
	bool HasBlock() const
	{
		for (int i = 0; i < BRIDGE_BLOCK_CNT; ++i)
			if (blocks[i] != Air)
				return true;
		return false;
	}
	bool FullBlocks() const
	{
		for (int i = 0; i < BRIDGE_BLOCK_CNT; ++i)
			if (blocks[i] == Air)
				return false;
		return true;
	}
	bool HereEmpty(double rate) const
	{
		if (CanPass())
			return false;

		int mid = BRIDGE_BLOCK_CNT * rate;
		int gap = 4;
		int air_cnt{ 0 };
		int beg = Clamp(mid - gap / 2, 0, BRIDGE_BLOCK_CNT - 1);
		for (int i = 0; i + beg < BRIDGE_BLOCK_CNT && i < gap; ++i)
		{
			air_cnt += (blocks[beg+i] == Air) ? 1 : 0;
		}
		/*if (air_cnt >= gap)
		{
			chat.AddChat("rate=" + str(rate) + " beg=" + str(beg) + " air_cnt = " + str(air_cnt), ORANGE);
		}*/
		return air_cnt >= gap;
	}
	void HereSetBlock(double rate, BridgeBlockType bid)
	{
		blocks[size_t(BRIDGE_BLOCK_CNT * rate)] = bid;
	}
	void UpdateCanPass() const
	{
		lastUpdateCanPass = clock();

		int air_cnt{ 0 };
		for (int i = 0; i < BRIDGE_BLOCK_CNT; ++i)
		{
			if (blocks[i] == Air)
			{
				air_cnt++;
				if (air_cnt > 3)
				{	//超过 格空气的间隙
					can_pass = false;
					return;
				}
			}
			else {
				air_cnt = 0;
			}
		}
		can_pass = true;
	}
	bool CanPass() const
	{
		if (clock() - lastUpdateCanPass < 1000)
			return can_pass;

		UpdateCanPass();
		return can_pass;
	}
	void UpdateMistakeOccurrence()
	{
		if (building)
		{
			static clock_t lastUpdateMistake = clock();

			if (clock() - lastUpdateMistake >= 1000L)
			{
				if (Percent(GetBridgingMistakeProb(method, builder)))
				{
					//致命失误！
					auto ptr = GetPlayerEntity(builder);
					if (ptr)
					{
						ptr->ClearAction();
						ptr->x += (LeftTheta(ptr->theta) ? 10.0 : -10.0);	//人工偏移 掉下去就行
					}
					InterruptBuild();
				}
				lastUpdateMistake = clock();
			}
		}
	}
	void ClearBlocks()
	{
		for (auto b : blocks)
			b = Air;
	}
	void Destroy()
	{	//拆除
		ClearBlocks();
		building = false;
		builder = "";
	}
	void BeginBuild(DIR2 dir, BridgingMethod bm, string_view pname)
	{
		build_dir = dir;
		building = true;
		builder = str(pname);
		method = bm;
		lastBuildBegin = clock();
	}
	void EndBuild()
	{
		building = false;
	}
	void InterruptBuild()
	{	//中断搭路
		building = false;
	}
};

class Team
{
public:
	TeamID team{ RedTeam };
	vector<string> player_names;

	Bed bed;

	Bridge bridges[TEAM_BRIDGE_MAX_CNT];
	//int bridges[TEAM_BRIDGE_MAX_CNT]{ 0 };	//桥是否连通
	//BridgingMethod bridging_method{Sneaking};
	//int building_bridge{ 0 };
	//string building_player{ "" };
	//clock_t lastBeginBridging{ 0L };

	vector<int>	team_upgrade_levels;
	vector<short> items;

	long exp{ 0L };
	long diamonds{ 0L };

	clock_t lastIron{ 0L };
	clock_t lastGold{ 0L };

	Team() = default;
	Team(TeamID teamID)
		: team(teamID)
	{
		bed.team = team;
		bed.hp = bed.max_hp;

		team_upgrade_levels.resize(team_upgrade_names.size(), 0);
		items.resize(item_names.size(), 0);
	}
	inline bool CanRespawn() const
	{
		return bed.IsActive();
	}
	void LoseItem(ItemID iid, short cnt = 1)
	{
		items.at(iid) -= cnt;
		if (items.at(iid) < 0)
			items.at(iid) = 0;
	}
	void ObtainItem(ItemID iid, short cnt = 1)
	{
		items.at(iid) += cnt;
	}
	inline clock_t GetIronCD() const
	{
		return IRON_CD * furnace_cd_rates[team_upgrade_levels.at(Furnace)];
	}
	inline clock_t GetGoldCD() const
	{
		return GOLD_CD * furnace_cd_rates[team_upgrade_levels.at(Furnace)];
	}
	size_t CountAlivePlayers() const;
	void InterruptBridging(string_view whose)
	{
		for (auto& b : bridges)
		{
			if (b.builder == whose)
			{
				b.InterruptBuild();
				return;
			}
		}
	}
	void UpdateResources()
	{
		if (clock() - lastIron >= GetIronCD())
		{
			exp += IRON_EXP_AMOUNT;
			lastIron = clock();
		}
		if (clock() - lastGold >= GetGoldCD())
		{
			exp += GOLD_EXP_AMOUNT;
			lastGold = clock();
		}
	}

	void ShowList() const
	{
		chat.AddChat(team_names[team] + "成员有 " + str(player_names.size()) + " 人：" + str(player_names),
			team);
	}


	bool NeedBridge() const
	{
		int sum = 0;
		for (auto b : bridges)
			sum += int(b.CanPass());

		return sum == 0;
	}
	bool FullBridge() const
	{
		for (auto b : bridges)
			if (!b.CanPass())
				return false;
		return true;
	}
	bool IsBridging() const
	{
		for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		{
			/*if (bridges[i].builder != ""
				&& bridges[i].lastBuildBegin > 0L
				&& clock() - bridges[i].lastBuildBegin 
					<= 1000L + bridging_durations[bridges[i].method]
				)
				return true;*/
			if (bridges[i].building)
				return true; 
		}
		return false;
		/*return
		building_bridge >= 0
			&& !building_player.empty()
			&& lastBeginBridging > 0L 
			&& (clock() - lastBeginBridging <= 1000L + bridging_durations[bridging_method]);*/
	}
	void UpdateBridging();
	void DrawBedBlocks(int x, int y) const
	{
		constexpr int fs = 30;

		string defs{ "" };
		int hp{ 0 };
		for (auto& pr : bed.defense)
		{
			if (pr.second > 0)
			{
				defs += block_names[pr.first] + " x " + str(pr.second) + '\n';
				hp += block_hps[pr.first] * pr.second;
			}
		}
		if (defs.empty())
			defs = "无任何防护";
		setfont(fs, 0, DEF_FONTNAME);
		setcolor(team);
		if (!defs.empty())
		{
			rectprintf_shadow(x, y, 400, 500,
				"%s床防御\n\n%s\n防御值：%d", team_names[team].c_str(),
				defs.c_str(), hp);
		}
		else {
			rectprintf_shadow(x, y, 400, 500,
				"%s床防御\n\n%s", team_names[team].c_str(),
				defs.c_str());
		}
	}
};


void CalcParabolaSpeed(double dist, double& v_p, double& v_h)
{	//计算抛物线垂直方向速度
	constexpr double coeff_1 = 0.03;
	double t = dist / v_p;
	v_h = t * VOID_FALL_G / 2.0 * coeff_1;
}
