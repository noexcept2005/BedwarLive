#pragma once

#define BED_DEF_MAX_HP 50.0

bool AntiBlastBlock(BlockType bt)
{
	return (bt == Obsidian || bt == Glass);
}

#define BED_WATER_SPEEDRATE 0.5
#define WATERED_DIGGING_SPEED_RATE 0.2
#define WATERED_DIGGING_DAMAGE_RATE 0.5		//对床本体的伤害

class Bed
{
public:
	TeamID team;
	map<BlockType, size_t> defense;

	double hp{ BED_DEF_MAX_HP };
	double max_hp{ BED_DEF_MAX_HP };

	clock_t lastHurt{ 0L };

	inline bool IsActive() const
	{
		return hp > 0.0;
	}
	inline bool HasDefense() const
	{
		return !defense.empty();
	}
	inline bool HasActualDefense() const
	{	//方块数量大于等于6个时才有效
		auto bcnt = CountDefenseBlockCount();
		return bcnt >= 6;
	}
	inline bool HasWater()
	{
		return defense[Water] > 0;
	}
	size_t CountDefenseBlockCount() const
	{	//床防御方块总数
		size_t res{ 0U };
		for (const auto& pr : defense)
		{
			if (pr.first == Water)
				continue;	//水不能算！！！！
			res += pr.second;
		}
			
		return res;
	}
	inline double GetHpRate() const
	{
		return Clamp(hp / max_hp, 0.0, 1.0);
	}

#define BLOCK_DUG_PROB_COEFF 4.0

	bool AttackDefense(DamageSource ds, bool explosion)
	{	//返回 true 表示防御成功，返回 false 表示还需要对床本体造成伤害
		//对床的防御进行破坏
		bool watered = HasWater();
		if (explosion)	//爆炸
		{
			if (watered)		//放水
				return true;	//防爆

			for (BlockType bid = Wool; bid < Water; bid = BlockType(int(bid) + 1))
			{
				if (AntiBlastBlock(bid)		//防爆
					&& FractionProb(8, Clamp<size_t>(defense[bid], 0, 8))
											//防爆方块数量越多，防爆概率越高，
											//该层大于等于8个时一定抗炸
					)	
					return true;

				if (defense[bid] <= 0)
					continue;

				//计算每层尽力爆破的方块数量
				size_t cnt = size_t(ds.damage / block_hps[bid]);
				if (defense[bid] >= min(4, cnt))
				{	//方块够，但是每层最多炸掉4个
					defense[bid] -= min(4, cnt);
					Sound(block_sounds_broken[bid]);
					ds.damage -= block_hps[bid] * min(4, cnt);
					if (ds.damage <= 0.0)
						return true;
				}
				else {
					//这一层方块炸完了
					ds.damage -= block_hps[bid] * defense[bid];
					defense[bid] = 0;
				}
			}

			//爆炸伤害没有全部被方块吸收，只能让床受伤
			return false;

		}
		else {	//非爆炸	(挖掘)
			for (BlockType bid = Wool; bid < Water; bid = BlockType(int(bid) + 1))
			{
				if (defense[bid] > 4)
				{	//每层大于4个方块开始才有效，8个方块时必有效
					if (FractionProb(						//分数概率

							Clamp((defense[bid] - 4.0) / 4.0, 0.0, 1.0)
							* block_hps[bid]
						,		//↑分母  ↓分子
							Clamp<int>(ds.damage * BLOCK_DUG_PROB_COEFF * 
								(watered? 1 : WATERED_DIGGING_SPEED_RATE),
										1, 255)
						)	//如果含水，挖掘速度变成一半
						)
					{	//按概率损伤
						//进入到 if 表示方块被挖掘
						if (defense[bid] > 0)
							defense[bid]--;
						
					}
					
					Sound(block_sounds_broken[bid]);	//音效
					//防御成功
					return true;	//每次只能挖一个方块
				}
			}
		}
		//没有一层方块是大于4个的时候防御无效
		//防御失败
		return false;
	}
#define BED_DAMAGE_COLOR LIGHTRED
	void OnHurt(DamageSource ds, bool explosion = false)
	{
		if (HasActualDefense())
		{	//有防御
			if (AttackDefense(ds, explosion))
				return;
		}
		else if (HasWater())
		{
			ds.damage *= WATERED_DIGGING_DAMAGE_RATE;
		}

		// 损伤床
		// 伤害指示器
		AddTextTip(GenerateNormalRandom(point_beds[team].x, 5.0), point_beds[team].y - 50, sprintf2("%.0f", ceil(ds.damage)), BED_DAMAGE_COLOR, DAMAGE_SHOW_DURATION);
		hp -= ds.damage;
		Sound(SndWood);

		if (hp <= 0.0)
		{
			hp = 0.0;
			OnDestroy(ds);	//床已被破坏
		}
		lastHurt = clock();
	}
	void OnDestroy(DamageSource ds)
	{
		Sound(SndBedDestruct);
		chat.AddChat(team_names[team] + "床已被摧毁！");
		if (!ds.owner_username.empty())
		{
			IncUserStat(ds.owner_username, "bed_destructor_cnt");
			AddScore(ds.owner_username, ScoreItem::DestroyBed);
		}
		OnBedDestroy(this->team, ds.owner_username);
	}
};

#define ADD_DEFENSE_MIN_DISTANCE (uix * 0.2)
double DistanceToBed(TeamID team, double x1, double y1)
{
	return Distance(x1, y1, point_beds[team].x, point_beds[team].y);
}