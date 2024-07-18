#pragma once

bool IsGameOver();

inline bool AdminName(string_view name)
{
	return sequ(str(name), MY_NAME) || sequ(str(name), MY_NAME2);
}

map<TeamID, string> team_names
{
	pair{RedTeam, "红队"s},
	pair{BlueTeam, "蓝队"s},
};
map<TeamID, string> team_names2
{
	{RedTeam, "红之队"},
	{BlueTeam, "蓝之队"},
};

inline TeamID OppoTeam(TeamID t1)
{
	return t1 == RedTeam ? BlueTeam : RedTeam;
}
TeamID ParseTeamID(string_view name, bool* pres = nullptr)
{
	for (auto const& pr : team_names)
		if (str(name) == pr.second)
		{
			if (pres)
				*pres = true;
			return pr.first;
		}
	if (pres)
		*pres = false;
	return RedTeam;
}

#include "userstat.h"
#include "buff.h"

void CalcParabolaSpeed(double dist, double& v_p, double& v_h);

class Bed;
class Team;

enum PlayerAction
{
	Defending,	//停止
	Bridging,	//搭路
	Moving,		//移动
	Digging,	//挖掘
};

enum BridgingMethod
{
	Sneaking,			//蹲搭
	NinjaBridging,		//蹲起搭
	Moonwalk,			//月步
	GodBridging,		//神桥
};

//达成该搭路方式满熟练度的搭路次数
//如果熟练度不够，将会有概率在搭路时失误掉进虚空
const vector<size_t> bridging_skilled_cnt
{
	10U,
	24U,
	32U,
	64U,
};

//%
#define BRIDGE_MISTAKE_PROB_MIN		0
#define BRIDGE_MISTAKE_PROB_MAX		6

int GetBridgingMistakeProb(BridgingMethod method, string_view pname)
{	//搭路失误概率	每秒一次判定
	size_t cnt{ 0U };
	if (user_stats.find(str(pname)) != user_stats.end())
	{
		cnt = user_stats[str(pname)].stat[bridging_cnt_keys[method]];
	}

	size_t maxvalue = bridging_skilled_cnt[method];

	//搭路越多，失误概率越低，且概率降低速度越来越低
	return Lerp(BRIDGE_MISTAKE_PROB_MAX, BRIDGE_MISTAKE_PROB_MIN,
		EaseOutCubic(Clamp(cnt / double(maxvalue), 0.0, 1.0))
	);
}

bool CanPlayerBridgeThisWay(string_view pname, BridgingMethod method)
{	//某玩家是否可以使用这种搭路方式
	if (user_stats.find(str(pname)) == user_stats.end())
	{	//新人
		user_stats.insert(make_pair(pname, UserStat()));
		user_stats[str(pname)].username = pname;
		return method == Sneaking;	//老老实实蹲搭
	}

	if (method == NinjaBridging)	//蹲起搭
		return (user_stats[str(pname)].stat["sneaking_cnt"] >= NINJA_AVAILABLE_BASE_CNT);
	if (method == Moonwalk)			//月步
		return (user_stats[str(pname)].stat["ninjabridging_cnt"] >= MOONWALK_AVAILABLE_BASE_CNT);
	if (method == GodBridging)		//神桥
		return (user_stats[str(pname)].stat["moonwalk_cnt"] >= GODBRIDGE_AVAILABLE_BASE_CNT);
	return false;
}


#include "entity.h"

class Player
{
public:
	string name;
	int variant{ 0 };					//外貌ID
	TeamID team;
	PickaxeID pickaxe{ BareHand };		//镐子
	SwordID   sword{ WoodenSword };		//剑（武器）
	ArmorID	  armor{ NoArmor };			//护甲

	double hp{ 20.0 };
	double max_hp{ 20.0 };

	Player()
	{
		name = "Player";
		team = RedTeam;
		RandomVariant();
	}
	Player(string_view _name, TeamID _team) : name(_name.data()), team(_team)
	{
		RandomVariant();
	}
	bool UpgradePickaxe(long& team_exp)
	{	//升级镐子
		if (IsGameOver())
		{
			chat.AddChat(str(name) + " 目前无法使用升级镐子，因为游戏已经结束了", GetStringColor(name));
			return false;
		}

		if (pickaxe == DiamondPickaxe)
		{
			chat.AddChat(name + "镐子已经升级到满级钻石镐！", team);
			return false;
		}
		if (team_exp < pickaxe_prices[pickaxe + 1])
		{
			chat.AddChat("经验不足，无法升级镐子！", team);
			return false;
		}
		pickaxe = PickaxeID(pickaxe + 1);
		team_exp -= pickaxe_prices[pickaxe];
		ClampA(team_exp, 0L, LONG_MAX);

		auto ptr = GetPlayerEntity(name);
		if (ptr)
		{	//更新镐子
			ptr->pick = pickaxe;
		}

		return true;
	}
	//bool UpgradeSword(long& team_exp)
	//{	//升级武器
	//	if (sword == NetheriteSword)
	//	{
	//		chat.AddChat(name + "武器已经升级到满级下界合金剑！", team);
	//		return false;
	//	}
	//	if (team_exp < sword_prices[pickaxe + 1])
	//	{
	//		chat.AddChat("经验不足，无法升级武器！", team);
	//		return false;
	//	}
	//	sword = SwordID(sword + 1);
	//	team_exp -= sword_prices[sword];
	//	ClampA(team_exp, 0L, LONG_MAX);

	//	auto ptr = GetPlayerEntity(name);
	//	if (ptr)
	//	{	//更新武器
	//		ptr->sword = sword;
	//	}

	//	return true;
	//}
	void Spawn()
	{
		constexpr int pspawn_min = 50;
		constexpr int pspawn_max = 200;

		double x{ 0.0 }, y{ 0.0 };

		if (team == RedTeam)
			x = RandomRangeDouble(pspawn_min, pspawn_max);
		else
			x = RandomRangeDouble(uix - pspawn_max, uix - pspawn_min);

		y = point_bridge_left_left[1].y;

		SpawnPlayerEntity(name, team, variant, x, y);

		auto ptr = GetPlayerEntity(name);
		if (ptr)
		{
			ptr->armor = armor;				//装备永久
			if (ptr->pick > BareHand)		//镐子降级
				ptr->pick = PickaxeID(pickaxe - 1);
			ptr->sword = sword;				//武器
		}
	}
	void RandomVariant()
	{	//随机外貌
		variant = RandomRange(0, PL_VARIANT_CNT, true, false);
	}
	void OnBridgeDone(BridgingMethod bm);
};

enum BlockType {
	NoBlock		= 0,
	Wool		= 1,
	Plank		= 2,
	Endstone	= 3,
	Glass		= 4,
	Obsidian    = 5,
	Water		= 6,
};

const vector<string> block_names
{
	"空气",
	"羊毛",
	"木板",
	"末地石",
	"玻璃",
	"黑曜石",
	"水",
};

const vector<SoundID> block_sounds
{
	SndStone,
	SndWool,
	SndWood,
	SndStone,
	SndStone,
	SndStone,
	SndWater,
};
const vector<SoundID> block_sounds_broken
{
	SndStone,
	SndWool,
	SndWood,
	SndStone,
	SndGlass,	//玻璃破碎
	SndStone,
	SndWater,
};

const vector<long> block_prices
{
	0,
	5,
	40,
	30,
	50,
	1500,
	400,
};

const vector<int> block_amount
{
	0, 4, 8, 8, 4, 4, 1,
};

const vector<int> block_hps
{
	0, 5, 25, 30, 15, 800, 100,
};

const vector<string> bridging_names
{
	"蹲搭",
	"蹲起搭",
	"月步",
	"神桥",
};

BridgingMethod ParseBridgingMethod(string_view s)
{
	for (int i = 0; i < 4; ++i)
		if (sequ(bridging_names[i], str(s)))
			return BridgingMethod(i);
	return Sneaking;
}
const vector<long> bridging_prices
{
	0L,
	5L,
	15L,
	20L,
};
const vector<long> bridging_durations
{
	25000L,
	15000L,
	7000L,
	5000L,
};

enum TeamUpgradeID {
	Protection,
	Sharpness,
	Hasteness,
	Furnace,
};

const vector<string> roman_levels
{
	"O", "I", "II", "III", "IV", "V",
};

const vector<string> team_upgrade_names
{
	"锋利附魔",
	"护甲强化",
	"疯狂矿工",
	"经验熔炉",
};

const vector<vector<long>> team_upgrade_prices
{
	{5, 8, 16},
	{5, 10, 20, 30},
	{4, 6},
	{4, 8, 12, 16},
};

const vector<Defense> protection_defense_additions
{	//团队升级“护甲强化” 各保护等级下提供的防御值
	0, 2, 3, 4, 6,
};

enum ItemID
{
	NoItem				= 0,
	FireballItem		= 1,
	TNT					= 2,
	GoldenApple			= 3,
	HealPotion			= 4,
	InvPotion			= 5,
	SpeedPotion			= 6,
	JumpPotion			= 7,
	StrengthPotion		= 8,
	EnderPearlItem		= 9,
	Sponge				= 10,
	KnockbackStick		= 11,
};

const vector<string> item_names
{
	"空物品",
	"烈焰弹",
	"TNT",
	"金苹果",
	"治疗药水",
	"隐身药水",
	"速度药水",
	"跳跃药水",
	"力量药水",
	"末影珍珠",
	"海绵",
	"击退棒",
};
const vector<int> item_amounts
{
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
};
const vector<long> item_prices
{
	0L,
	40L,
	200L,
	50L,
	100L,
	500L,
	100L,
	100L,
	300L,
	400L,
	50L,
	100L,
};

map<ItemID, BuffID> potion_buffs
{
	{HealPotion, InstantHeal},
	{InvPotion, Invisibility},
	{SpeedPotion, Speed},
	{JumpPotion, JumpBoost},
	{StrengthPotion, Strength},
};

map<ItemID, clock_t> potion_durations
{
	{HealPotion, 1000L},
	{InvPotion, 30000L},
	{SpeedPotion, 120000L},
	{JumpPotion, 30000L},
	{StrengthPotion, 60000L},
};

#define IRON_CD	   1000L
#define IRON_EXP_AMOUNT 1L
#define GOLD_CD	   5000L
#define GOLD_EXP_AMOUNT 5L
#define DIAMOND_CD 15000L
#define EMERALD_CD 30000L
#define EMERALD_EXP_AMOUNT 100L

void OnBedDestroy(TeamID team, string_view who);

bool WillFall(shared_ptr<Entity>& pet, FallReason reason, string_view pname);

#include "bed.h"
#include "team.h"

struct BedwarGame
{
	map<TeamID, Team> teams;
	map<string, Player> players;
	vector<shared_ptr<Entity>> entities;

	vector<string> random_team_players;

	bool started{ false };
	bool over{ false };
	clock_t start{ 0L };
	clock_t end{ 0L };

	//绿宝石
	long emerald_cnt{ 0L };
	clock_t lastEmerald{ 0L };

	//钻石
	long diamond_cnt[2]{ 0L };
	clock_t lastDiamond[2]{ 0L };

	//一局游戏中的统计
	struct {

		map<string, size_t> player_scores;
		string mvp_name{ "" };

		size_t chat_cnt{ 0U };
		size_t diamond_collected_cnt{ 0U };	//收集钻石总数
		size_t item_purchase_cnt{ 0U };		//物品购买次数
		size_t block_purchase_cnt{ 0U };	//方块购买次数
		size_t mob_purchase_cnt{ 0U };		//怪物购买次数

		//用于庆祝动画
		size_t tnt_cnt{ 0L };
		size_t remain_tnt{ 0L };
		clock_t last_tnt{ 0L };

	}stats;

	void InitStats()
	{
		stats.chat_cnt = 0U;
		stats.diamond_collected_cnt = 0U;
		stats.item_purchase_cnt = 0U;	
		stats.block_purchase_cnt = 0U;
		stats.mob_purchase_cnt = 0U;	
		stats.player_scores.clear();
	}
}bw;

bool IsGameOver()
{
	return bw.over;
}

bool OnIncompleteBridge(Place place)
{
	if (OnRedBridge(place))
		return !bw.teams[RedTeam].bridges[place - RedBridgeA].FullBlocks();
	if (OnBlueBridge(place))
		return !bw.teams[BlueTeam].bridges[place - BlueBridgeA].FullBlocks();
	return false;
}
void AddScore(string_view who, ScoreItem reason)
{	//获得分数
	bw.stats.player_scores[str(who)] += score_item_values.at(int(reason));

	//记录到档案
	GetUserStat(who, "score") += score_item_values.at(int(reason));
}

Defense GetTeamDefense(TeamID team)
{
	return protection_defense_additions[bw.teams[team].team_upgrade_levels[Protection]];
}
clock_t GetGameDuration()
{
	if (bw.over)
		return bw.end - bw.start;
	return (clock() - bw.start);
}

bool VoidPos(Place place, double x, double y)
{
	if (place == OtherPlace)
	{
		chat.AddChat("处于其他地点！", GOLD);
		return true;
	}

	if (OnBridge(place))
	{
		//检查桥的缺口
		if (OnRedBridge(place))
		{
			return bw.teams[RedTeam].bridges[place - RedBridgeA].HereEmpty(
				GetNowBridgePosRate(x, RIGHT)
			);
		}
		else {
			return bw.teams[BlueTeam].bridges[place - BlueBridgeA].HereEmpty(
				GetNowBridgePosRate(x, LEFT)
			);
		}
	}
	return false;
}

void SpawnPlayerEntity(string_view name, TeamID team, int variant, double x, double y)
{
	PlayerEntity pe;
	pe.id = PlayerAvatar;
	pe.name = str(name);
	pe.team = team;
	pe.variant = variant;
	pe.x = x;
	pe.y = y;
	if (pe.team == RedTeam)
		pe.theta = 0.0;
	else
		pe.theta = PI;
	pe.active = true;

	bw.entities.push_back(make_shared<PlayerEntity>(pe));
}
int SelectAvailableBridge(TeamID team)
{
	static int _map[] = { 1, 2, 0 };
#ifdef BRIDGE_NO_CHOOSE
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
	{
		if (bw.teams[team].bridges[_map[i]].CanPass())
			return _map[i];
	}
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
	{
		if (bw.teams[team].bridges[_map[i]].HasBlock())
			return _map[i];
	}

#else	//严格模式，必须能走通才上桥
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
	{
		if (bw.teams[team].bridges[_map[i]].CanPass())
			return _map[i];
	}
#endif

	return -1;

}
PlayerEntity* GetPlayerEntity(string_view name)
{
	for (auto& pet : bw.entities)
		if (pet->id == PlayerAvatar 
			&& sequ(pet->name, str(name)))
			return dynamic_cast<PlayerEntity*>(pet.get());
	return nullptr;
}
bool NearbyExistEnemy(TeamID team, double x, double y, double r)
{
	for (const auto& pet : bw.entities)
		if (pet->team != team
			&& Distance(x, y, pet->x, pet->y) <= r)
			return true;
	return false;
}
void CreateProjectile(EntityID eid, TeamID team, string_view owner_username, double v, double v_h, double x, double y, double h, double theta, double proj_damage)
{	//创建弹射物
	Projectile proj;
	proj.id = eid;
	proj.owner_username = str(owner_username);
	proj.x = x;
	proj.y = y;
	proj.h = 20.0;
	proj.throw_point.x = x;
	proj.throw_point.y = y;
	/*proj.range = proj_ranges[eid];*/
	proj.theta = theta;
	proj.team = team;

	proj.InitData();

	proj.proj_damage = proj_damage;
	//重置弹射物初速度
	proj.v.Reset(theta, v);
	if (proj.v.IsZero())
		proj.move = false;	//这样才能静止
	else
		proj.move = true;
	proj.v_h = v_h;

	proj.Awake();

	bw.entities.push_back(make_shared<Projectile>(proj));
}

void PlayerEntity::UpdatePickaxe()
{
	pick = bw.players[name].pickaxe;
}
double PlayerEntity::GetDigCDRate() const
{	// 与团队疯狂矿工等级有关
	return hasteness_dig_cd_rates[bw.teams[team].team_upgrade_levels[Hasteness]];
}
double PlayerEntity::GetDamageAddition() const
{	// 与团队锋利附魔等级有关
	return sharpness_damage_addition[bw.teams[team].team_upgrade_levels[Sharpness]]

		//加上力量buff
		+ buffs.HasBuff(Strength) ? BUFF_STRENGTH_DAMAGE_ADDITION : 0.0;
}
void PlayerEntity::TryToDigOppoBed()
{
	if (!active)
		return;

	if (TimeToDig())
	{
		Bed& bed = GetOppoBed();
		if (!bed.IsActive())
		{	//对方床已被破坏
			ResumeMoving();
			return;
		}
		DamageSource ds;
		ds.damage = GetDigDamage();
		ds.owner_username = owner_username;
		ds.eid = id;
		ds.team = team;
		bed.OnHurt(ds);

		lastDig = clock();
	}
}

void PlayerEntity::UpdateActiveFacing()
{
	if (ForceStop())
	{	//强制停下
		if (!Still())
			Stop();
		return;
	}

	if (!IsBridging())
	{
		//面向敌人/敌床
		Entity* pet = GetNearestEnemy();
		int x_obed = bw.teams[OppoTeam(team)].bed.max_hp;
		int x_oe = pet ? pet->x : -10000.0;
		int d_obed = abs(x_obed - x);
		int d_oe = abs(x - x_oe);

		//鬼畜现象的来源
		if (d_oe < d_obed)
		{
			if (d_oe >
				100	//提高这个值可以减轻鬼畜现象
				)
			{
				/*if (x > x_oe && RightTheta(theta))
					TurnToLeft();
				else if (x < x_oe && LeftTheta(theta))
					TurnToRight();*/

				if (ForceGood(x_oe > x ? RIGHT : LEFT))
					TurnToEntity(pet);
			}
			else if (d_oe < 40)
			{	//离这个敌怪太近了
				if (ForceGood(x_oe > x ? RIGHT : LEFT))
					Stop();	//停下就行了
			}
		}
		else if ((team == RedTeam && LeftTheta(theta)
			|| team == BlueTeam && RightTheta(theta))

			&& d_obed > 200)
		{	//面向敌床
			if (ForceGood(x_obed > x ? RIGHT : LEFT))
				TurnToOppoBed();
		}

		if (d_oe >= 40 && Still())
		{	//不能一直停着
			ResumeMoving();
		}
	}
}

void MeleeEntity::TryToDigOppoBed()
{
	if (!active || !TimeToContact())
		return;

	Bed& bed = GetOppoBed();
	if (!bed.IsActive())
	{	//对方床已被破坏
		ResumeMoving();
		return;
	}
	DamageSource ds;
	ds.damage = melee_damage;
	ds.owner_username = owner_username;
	ds.eid = id;
	ds.team = team;
	if (entity_explosion_radii.find(id) != entity_explosion_radii.end()
		)
	{
		if (Distance(x, y, point_beds[bed.team].x, point_beds[bed.team].y)
			<= entity_explosion_radii[id]	//在范围内再爆炸
			)
		{
			MakeExplosion(x, y, entity_explosion_radii[id], id, owner_username,
				ds.damage);		//爆炸伤害取代接触伤害
			active = false;
		}
	}
	else
	{
		bed.OnHurt(ds);		//接触伤害
	}

	lastContact = clock();
	AfterContact();
}
void PlayerEntity::TryToAttack()
{
	if (!this || !active)
		return;

	DamageSource ds;
	ds.eid = id;
	ds.owner_username = name;
	ds.damage = GetMeleeDamage();
	ds.team = team;

	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || pet->team == team 
			|| pet->GetType() == ProjectileType)
			continue;

		if (CanShoot() && TimeToShoot()
			&& Distance(x, y, pet->x, pet->y) <= ET_RANGER_VIEW)
		{	//远程
			ShootTarget(pet.get());
			return;
		}

		if (CanMelee() && TimeToMelee() 
			&& Distance(x, y, pet->x, pet->y) <= ET_MELEE_RANGE
			&& abs(h - pet->h) <= ET_MELEE_RANGE_H)
		{	//近战
			
			double _theta = atan2(pet->x - x, y - pet->y);

			pet->OnHurt(ds);
			pet->SufferImpulse(ds.damage, _theta, 0.0);

			//在桥上有概率被弄下去
			if (OnBridge(pet->place)
				&& WillFall(pet, MeleeKnockback, pet->name)
				)
			{
				pet->OnVoidFall();
			}

			lastMelee = clock();
			AfterMelee();

			break;
		}
	}
}
void MeleeEntity::TryToAttack()
{
	if (!active || !TimeToContact())
		return;

	DamageSource ds;
	ds.eid = id;
	ds.owner_username = owner_username;
	ds.damage = melee_damage + GetDamageAddition();
	ds.team = team;

	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || pet->GetType() == ProjectileType)
			continue;

		auto dist = Distance(x, y, pet->x, pet->y);

		if (pet->team != team
			&& dist <= ET_MELEE_RANGE
			&& abs(h-pet->h) <= ET_MELEE_RANGE_H)
		{	//近战
			if (entity_explosion_radii.find(id) != entity_explosion_radii.end()
				)
			{
				if (max(dist, abs(h - pet->h))
					<= entity_explosion_radii[id]	//在范围内再爆炸
					)
				{
					MakeExplosion(x, y, entity_explosion_radii[id], id, owner_username,
						ds.damage);		//爆炸伤害取代接触伤害
				}
				else
					continue;
			}
			else
			{
				double _theta = atan2(pet->x - x, y - pet->y);

				pet->OnHurt(ds);
				pet->SufferImpulse(ds.damage, _theta, 0.0);

				//在桥上有概率被弄下去
				if (OnBridge(pet->place)
					&& WillFall(pet, MeleeKnockback, pet->name)
					)
				{
					pet->OnVoidFall();
				}
			}
			lastContact = clock();
			AfterContact();
			break;
		}
	}
}
void RangerEntity::TryToDigOppoBed()
{
	if (!active || !TimeToShoot())
		return;
	if (!GetOppoBed().IsActive())
	{	//对方床已被破坏
		ResumeMoving();
		return;
	}

	if (Distance(x, y, point_beds[GetOppo()].x, point_beds[GetOppo()].y)
		<= ET_RANGER_VIEW)
	{	//远程
		ShootThere(point_beds[GetOppo()].x, point_beds[GetOppo()].y);
		return;
	}
}
void RangerEntity::TryToAttack()
{
	if (!active || !TimeToShoot())
		return;

	//for (auto& pet : bw.entities)
	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || pet->team == team 
			|| pet->GetType() == ProjectileType)
			continue;

		if (Distance(x, y, pet->x, pet->y) <= ET_RANGER_VIEW)
		{
			ShootTarget(pet.get());
			return;
		}
	}
}
void MeleeRangerEntity::TryToDigOppoBed()
{
	MeleeEntity::TryToDigOppoBed();
	RangerEntity::TryToDigOppoBed();
}
void MeleeRangerEntity::TryToAttack()
{
	if (!Entity::active)
		return;

	DamageSource ds;
	ds.eid = id;
	ds.owner_username = MeleeEntity::owner_username;
	ds.damage = melee_damage + GetDamageAddition();
	ds.team = team;

	if (TimeToShoot())
	{
		for (int i = 0; i < bw.entities.size(); ++i)
		{
			auto pet = bw.entities.at(i);
			if (pet && pet->team != team && pet->GetType() != ProjectileType
				&& Distance(x, y, pet->x, pet->y) <= ET_RANGER_VIEW)
			{
				ShootTarget(pet.get());
				return;
			}
		}
	}
	if (TimeToContact())
	{
		for (int i = 0; i < bw.entities.size(); ++i)
		{
			auto pet = bw.entities.at(i);

			auto dist = Distance(x, y, pet->x, pet->y);

			if (entity_explosion_radii.find(id) != entity_explosion_radii.end()
				)
			{
				if (max(dist, abs(h - pet->h))
					<= entity_explosion_radii[id]	//在范围内再爆炸
					)
					MakeExplosion(x, y, entity_explosion_radii[id], id, MeleeEntity::owner_username,
						ds.damage);		//爆炸伤害取代接触伤害
				else
					continue;
			}
			else if (pet->team != team
				&& dist <= ET_MELEE_RANGE
				&& abs(h - pet->h) <= ET_MELEE_RANGE_H)
			{

				double _theta = atan2(pet->x - x, y - pet->y);

				pet->OnHurt(ds);
				pet->SufferImpulse(ds.damage, _theta, 0.0);

				//在桥上有概率被弄下去
				if (OnBridge(pet->place)
					&& WillFall(pet, MeleeKnockback, pet->name)
					)
				{
					pet->OnVoidFall();
				}

				lastContact = clock();
				AfterContact();
				break;
			}
		}
	}
}
void Projectile::TryToAttack()
{
	if (!active)
		return;

	DamageSource ds;
	ds.eid = id;
	ds.owner_username = owner_username;
	ds.damage = proj_damage + GetDamageAddition();
	ds.team = team;

	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || !pet->active
			|| pet->GetType() == ProjectileType)	//弹射物不对弹射物造成伤害
			continue;

		auto dist = Distance(x, y, pet->x, pet->y);

		if (pet->team != team
			&& dist <= ET_PROJ_CONTACT_R
			&& abs(h - pet->h) <= ET_PROJ_CONTACT_R_H)
		{
			if (entity_explosion_radii.find(id) != entity_explosion_radii.end()
				)
			{
				if (max(dist, abs(h - pet->h))
					<= entity_explosion_radii[id]	//在范围内再爆炸
					)
					MakeExplosion(x, y, entity_explosion_radii[id], id, owner_username,
						ds.damage);		//爆炸伤害取代接触伤害
				else
					continue;
			}
			else
			{
				double _theta = atan2(pet->x - x, y - pet->y);
				double _theta_h = atan2(pet->h - 0.0, dist);
				pet->OnHurt(ds);	//弹射物正常的接触伤害
				pet->SufferImpulse(ds.damage, _theta, _theta_h);

				//在桥上有概率被弄下去
				if (OnBridge(pet->place)
					&& WillFall(pet, ProjKnockback, pet->name)
					)
				{
					pet->OnVoidFall();
				}
				OnRangeOver();		//弹射物被销毁，应该调用这个
			}
			
			active = false;
			break;
		}
	}
}

template <typename TEntity, typename T2 /*= TEntity*/>
//requires...
shared_ptr<Entity> SpawnEntity(EntityID eid, TeamID team, string_view whose, double x, double y, double theta, double v, double h)
{	//生成实体
	TEntity that;
	that.id = eid;
	that.team = team;
	that.x = x;
	that.y = y;
	that.h = h;
	that.T2::owner_username = str(whose);
	that.theta = theta;
	that.v.Reset(theta, v);
	that.ResumeMoving();

	that.InitData();
	that.Awake();

	bw.entities.push_back(make_shared<TEntity>(that));
	return bw.entities.back();
}

bool Entity::CanDigBed_External(const Bed& target_bed) const
{
	return target_bed.IsActive();
	//return !target_bed.HasActualDefense();
}

double Entity::GetSpeedRate() const
{
	double rate{ SPEEDRATE };
	if (NearOppoBed() && bw.teams[OppoTeam(team)].bed.HasWater())
	{
		rate *= BED_WATER_SPEEDRATE;
	}
	if (buffs.HasBuff(Speed))	//迅捷
		rate *= BUFF_SPEED_RATE;
	return rate;
}
double Entity::GetJumpSpeedRate() const
{
	double rate{ 1.0 };
	if (buffs.HasBuff(JumpBoost))
		rate *= BUFF_JUMP_BOOST_RATE;
	return rate;
}

Bed& Entity::GetOppoBed()
{
	return bw.teams[GetOppo()].bed;
}
Entity* Entity::GetNearestEnemy() const
{
	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || pet->team == team 
			|| !pet->active
			|| pet->GetType() == ProjectileType)
			continue;

		return pet.get();
	}
	return nullptr;
}
void Entity::UpdateActiveFacing()
{
	if (!IsBridging())
	{
		//面向敌人/敌床
		Entity* pet = GetNearestEnemy();
		int x_obed = bw.teams[OppoTeam(team)].bed.max_hp;
		int x_oe = pet ? pet->x : -10000.0;
		int d_obed = abs(x_obed - x);
		int d_oe = abs(x - x_oe);

		//鬼畜现象的来源
		if (d_oe < d_obed)
		{
			if (d_oe >
				100	//提高这个值可以减轻鬼畜现象
				)
			{
				/*if (x > x_oe && RightTheta(theta))
					TurnToLeft();
				else if (x < x_oe && LeftTheta(theta))
					TurnToRight();*/

				TurnToEntity(pet);
			}
			else if (d_oe < 40)
			{
				Stop();	//停下就行了
			}
		}
		else if ((team == RedTeam && LeftTheta(theta)
			|| team == BlueTeam && RightTheta(theta))

			&& d_obed > 200)
		{	//面向敌床
			TurnToOppoBed();
		}

		if (d_oe >= 40 && Still())
		{	//不能一直停着
			ResumeMoving();
		}
	}
}
void Entity::UpdateRoute()
{	//更新运动路线方案
	if (GetType() == ProjectileType)
		return;

	UpdateActiveFacing();

	DIR dir = v.x > 0.0 ? RIGHT : LEFT;
	if (AtCenterIsland(x, y))
	{	//中间
		y += (point_center.y - y) / 2.0;
	}
	else if (AtHome(place) && !AtBridgeLeftLeft(x, y) && !AtBridgeRightRight(x, y))
	{	//家里
		y += (point_beds[RedTeam].y - y) / 4.0;
	}
	else if (OnBridge(place))
	{	//在桥上
		bool yes{ false };
		if (OnRedBridge(place))
		{
			yes = (dir == LEFT && x >= uix / 2 || dir == RIGHT && x <= uix / 2);
		}
		else {
			yes = (dir == RIGHT && x <= uix * .75 || dir == LEFT && x >= uix * .75);
		}

		if (yes)
		{	//向桥中央走
			if (target_bridge_index == -2)
				;
			else if (target_bridge_index == -1)
			{	//没有定好哪座桥
				/*if (!IsBridging())
					OnVoidFall();*/
				target_bridge_index = -2;
			}
			else {
				target_bridge_index = -2;
			}

			int index = OnRedBridge(place) ? place - RedBridgeA : place - BlueBridgeA;
			y += (BRIDGE_TOP + BRIDGE_GAP * index - 40 - y) / 2.0;
		}
	}
	else if (AtCenterIslandLeft(x, y))
	{
		if (dir == RIGHT)
		{
			y += (point_centerisland_left.y - y) / 2.0;
		}
		else {
			if (target_bridge_index >= 0)
				y += (BRIDGE_TOP + BRIDGE_GAP * target_bridge_index - 40 - y) / 2.0;
		}
	}
	else if (AtCenterIslandRight(x, y))
	{
		if (dir == LEFT)
		{
			y += (point_centerisland_right.y - y) / 2.0;
		}
		else {
			if (target_bridge_index >= 0)
				y += (BRIDGE_TOP + BRIDGE_GAP * target_bridge_index - 40 - y) / 2.0;
		}
	}
	else if (AtBridgeLeftLeft(x, y))
	{
		if (dir == LEFT)
		{
			y += (point_beds[team].y - y) / 2.0;
		}
		else {
			if (target_bridge_index >= 0)
				y += (BRIDGE_TOP + BRIDGE_GAP * target_bridge_index - y) / 2.0;
		}
	}
	else if (AtBridgeRightRight(x, y))
	{
		if (dir == RIGHT)
		{
			y += (point_beds[team].y - y) / 2.0;
		}
		else {
			if (target_bridge_index >= 0)
				y += (BRIDGE_TOP + BRIDGE_GAP * target_bridge_index - y) / 2.0;
		}
	}

	if (AtHome(place) && !Still()

		&& (x < 100.0 && LeftTheta(v.Theta()) 
		 || x > uix - 100.0 && RightTheta(v.Theta()))

		&& EdgeBackEntityType(GetType()))

	{	//屏幕边缘
		
		TurnAround();
		ForceCommandReset();

		if (GetType() == PlayerType())
		{
			AddTextTip(x, y - 10, "边缘掉头", PINK, 1000L);
			if (AtOwnHome())
			{
				Stop();
			}
		}else
		{
			AddTextTip(x, y - 10, "掉头", PINK, 1000L);
		}
	}
	else if (!IsBridging()
		&& GetIQ() >= Normal
		&& (WannaOnBridge(RedTeam, dir, x, y)
		|| WannaOnBridge(BlueTeam, dir, x, y))
		)
	{	//准备上桥
		//AddTextTip(x, y - 10, "准备上桥", team);

		bool cut{ false };
		TeamID which = x < uix / 2 ? RedTeam : BlueTeam;
		int that;

		bool flag{ false };
		if (target_bridge_index < 0)
		{
			that = SelectAvailableBridge(which);
			target_bridge_index = that;
			flag = true;
		}
		else {
			that = target_bridge_index;
		}

		if (that == -1 && flag)
			cut = true;
		else if (that < 0 || that >= TEAM_BRIDGE_MAX_CNT)
		{	//异常情况
			AddTextTip(x, y-10, "that=" + str(that), ORANGE);
			target_bridge_index = SelectAvailableBridge(which);
			cut = true;
		}
		else if (RedTeam == which)
		{
			cut = !bw.teams[RedTeam].bridges[that].CanPass();
		} 
		else if (BlueTeam == which){
			cut = !bw.teams[BlueTeam].bridges[that].CanPass();
		}

		IQLevel iq = GetIQ();

		if (cut)
		{
			if (flag && h < 5.0 && !VeryForceMoving())	//<!> 没桥时触发
			{
				if (!Still() && (iq == Normal || AtHome(place)))
				{
					AddTextTip(x, y - 10, place_names[place] + "停下", PINK, 1000L);
					if (GetType() == PlayerType)
					{
						Stop();	//停下
					}
				}
				else if (iq == Smart) {
					//chat.AddChat(place_names[place] + " 死路折返", CYAN);
					AddTextTip(x, y - 10, place_names[place] + " 死路折返", PINK, 1000L);
					TurnAround();
				}
			}
		}
		else if (iq != Smart && Still() && !ForceStop())
		{
			ResumeMoving();	//连通 继续走
		}
	}
}
void Entity::CheckVoidFall()
{	//检查是否进入虚空
	if (void_fall)
		return;
	if (h > 20.0)
		return;
	if (h > -10.0 && GetType() == ProjectileType)
		return;

	if (VoidPos(place, x, y) && !IsBridging())
		void_fall = true;

	if (/*place == OtherPlace 
		||*/
		h < -10.0 
		//&& GetType() != ProjectileType
		)
	{
		//chat.AddChat("高度太低了", team);
		void_fall = true;
	}

	//else if (OnRedBridge(place))
	//{
	//	double rate = GetNowBridgePosRate(x);
	//	//if (rate < bw.teams[RedTeam].bridges[place - RedBridgeA].GetCompletion() / 100.0)
	//	if (bw.teams[RedTeam].bridges[place - RedBridgeA].HereEmpty(rate))
	//	{
	//		chat.AddChat("从红桥掉下去", RedTeam);
	//		void_fall = true;
	//	}
	//}
	//else if (OnBlueBridge(place))
	//{
	//	double rate = GetNowBridgePosRate(x);
	//	/*if (rate < bw.teams[BlueTeam].bridges[place - BlueBridgeA].completion / 100.0)*/
	//	if (bw.teams[RedTeam].bridges[place - BlueBridgeA].HereEmpty(rate))
	//	{
	//		chat.AddChat("从蓝桥掉下去", BlueTeam);
	//		void_fall = true;
	//	}
	//}

	if (void_fall)
	{
		OnVoidFall();
	}
}
void Projectile::OnRangeOver()
{
	active = false;
	if (!VoidPos(place, x, y))
	{
		if (id == EnderPearl)
		{	//传送玩家到此处
			auto ptr = GetPlayerEntity(owner_username);
			if (ptr && ptr->active)
			{
				ptr->TeleportTo(x, y);
				ptr->LockMoveFor(300);
				ptr->ResumeMoving();
				return;
			}
		}
		else if (id == Fireball)
		{	//火球爆炸
			MakeExplosion(x, y, entity_explosion_radii[Fireball], Fireball, owner_username,
				entity_melee_damages[Fireball]);
		}
	}
}


void Team::UpdateBridging()
{
	int index = 0;
	for (auto& b : bridges)
	{
		if (b.building)
		{
			//if (!b.CanPass())
			if (!b.FullBlocks())
			{
				double rate = b.GetBridgingRate();
				/*
				if (fequ(rate, 1.0))
				{
					b.EndBuild();
					b.can_pass = true;
					bw.players[b.builder].OnBridgeDone(b.method);
				}
				else*/
				{	//bridging
					/*b.completion =
						max(b.completion, 99 * rate);*/

					auto ptr = GetPlayerEntity(b.builder);
					if (!ptr)
					{
						b.InterruptBuild();
						return;
					}

					b.HereSetBlock((b.build_dir == RIGHT ? rate : 1.0 - rate), TeamWool(ptr->team));

					double px{ 0.0 };
					int left_base{ 0 }, right_base{ 0 };

					if (team == RedTeam)
					{
						left_base = BRIDGE_LEFT_LEFT;
						right_base = CENTERISLAND_LEFT;
					}
					else if (team == BlueTeam)
					{
						left_base = CENTERISLAND_RIGHT;
						right_base = BRIDGE_RIGHT_RIGHT;
					}

					if (b.build_dir == RIGHT)
						px = left_base + BRIDGE_BLOCK_PX * BRIDGE_BLOCK_CNT * rate;
					else
						px = right_base - BRIDGE_BLOCK_PX * BRIDGE_BLOCK_CNT * rate;

					double py = BRIDGE_TOP + index * BRIDGE_GAP - 50;

					//设置玩家位置和朝向
					ptr->SetPos(px, py, b.build_dir == LEFT ? 0.0 : PI);

				}

				//随机出现搭路失误事件
				b.UpdateMistakeOccurrence();
			}
			else {
				b.EndBuild();
				bw.players[b.builder].OnBridgeDone(b.method);
			}
		}
		++index;
	}
}
size_t Team::CountAlivePlayers() const
{	//队伍中活着的玩家数量
	if (player_names.empty())
		return 0U;

	size_t cnt{ 0U };
	PlayerEntity* ptr{ nullptr };
	for (auto const& pname : player_names)
		if (nullptr != (ptr = GetPlayerEntity(pname)) 
			&& ptr->active)
			++cnt;
	return cnt;
}

bool WillFall(shared_ptr<Entity>& pet, FallReason reason, string_view pname)
{	//是否会被击退下桥梁
	double rate = 1.0;
	switch (reason)
	{
	case MeleeKnockback:
		rate = 0.1; break;
	case ProjKnockback:
		rate = 0.3; break;
	case ExplosionKnockback:
		rate = 0.6; break;
	}

	rate *= UserFallRateCoeff(pname, reason);
	rate *= (1.0 - pet->GetKnockbackResistance());

	return Percent(100 * rate);
}

void MakeExplosion(double x, double y, double r, EntityID eid, string_view username, double damage)
{	//制造瞬间爆炸				高度只能是0
	AddTextTip(x, y, "BOOM!", GOLD, 3000L);

	DamageSource ds;
	ds.eid = eid;
	ds.owner_username = username;
	ds.team = bw.players[str(username)].team;

	//对周围实体造成伤害
	for (int i = 0; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || !pet->active || pet->GetType() == ProjectileType
		)
			continue;

		auto dist = Distance(x, y, pet->x, pet->y);
		if (dist <= r && abs(0.0 - pet->h) <= r * 2.0)
		{
			//离得越近伤害越高 非线性
			auto rat = dist / r;
			double _theta = atan2(y - pet->y, pet->x - x);
			double _theta_h = atan2(pet->h - 0.0, dist);
			ds.damage = Lerp(damage, 0.0, EaseInExpo(rat));

			if (pet->team != bw.players[str(username)].team)
				pet->OnHurt(ds);	//敌军会受到伤害

			//击退力
			pet->SufferImpulse(ds.damage, _theta, _theta_h);

			//在桥上有概率被弄下去
			if (OnBridge(pet->place) 
				&& pet->h <= 20.0	//高度不够，更容易被炸下去
				&& WillFall(pet, ExplosionKnockback, pet->name)	//60% 被炸下去
				)
			{
				pet->OnVoidFall();
			}
		}
	}

	//对周围敌方床（防御）造成破坏
	TeamID teams[]{ RedTeam, BlueTeam };
	for (auto t : teams)
	{
		if (t == bw.players[str(username)].team	//忽略己方床
			|| !bw.teams[t].bed.IsActive()		//和已摧毁的床
			)
			continue;	

		if (Distance(x, y, point_beds[t].x, point_beds[t].y) <= r)
		{
			//chat.AddChat("TNT 炸床", ORANGE);
			bw.teams[t].bed.OnHurt(ds, true);
		}
	}

	Sound(SndExplode);
}

void DrawBridgeText(int x, int y)
{
	static string bridging_texts{ "" };
	if (bridging_texts.empty())
		for (int i = 0; i < bridging_names.size(); ++i)
		{
			bridging_texts += bridging_names[i] + "\t-" + str(bridging_prices[i]) + "经验\n";
		}

	//if (NeedBridge())
	{
		constexpr int fs = 25;
		setfont(fs, 0, DEF_FONTNAME);
		setcolor(MAGENTA);
		xyprintf_stroke(x, y, "搭桥操作 扣搭路方式名称 每个队伍最多三座桥");
		setwcolor(ORANGE);
		rectprintf_shadow(x, y + fs * 1.2, 400, 500,
			bridging_texts.c_str()
		);
	}
}

bool AddDefense(TeamID team, string_view pname, BlockType btype)
{	//增强床防御
	if (IsGameOver())
	{
		chat.AddChat(str(pname) + " 目前无法增强床防御，因为游戏已经结束了", GetStringColor(pname));
		return false;
	}

	auto ptr = GetPlayerEntity(pname);
	if (!ptr)
	{
		chat.AddChat(str(pname) + "未复活，无法进行床防御", team);
		return false;
	}
	//if (ptr->place != (ptr->team == RedTeam ? RedHome : BlueHome))
	if (DistanceToBed(team, ptr->x, ptr->y) > ADD_DEFENSE_MIN_DISTANCE)
	{
		AddTextTip(ptr->x, ptr->y - 15, "离床太远！", team);
		return false;
	}
	if (bw.teams[team].exp < block_prices[btype])
	{
		AddTextTip(team == RedTeam ? 250 : 1800,
			290, "经验不足", YELLOW, 2000L);
		//chat.AddChat("买不起" + block_names[btype] + "!", team);
		return false;
	}
	if (btype == Water
		&& bw.teams[team].bed.defense.find(Water) != bw.teams[team].bed.defense.end()
		&& bw.teams[team].bed.defense[Water] > 0)
	{
		AddTextTip(team == RedTeam ? 350 : 1610,
			280, "当前床已经布置水", ROYALBLUE, 3000L);
		return false;
	}
	if (bw.teams[team].bed.defense.find(btype) == bw.teams[team].bed.defense.end())
		bw.teams[team].bed.defense.insert(make_pair(btype, 0));
	bw.teams[team].bed.defense[btype] += block_amount[btype];
	bw.teams[team].exp -= block_prices[btype];
	chat.AddChat(team_names[team] + "购买" + block_names[btype] + "x" + str(block_amount[btype]),
		WHITE);
	bw.stats.block_purchase_cnt++;
	Sound(block_sounds[btype]);
	return true;
}
DIR GetBridgeExtendDir(TeamID team, int index, double rate2mid)
{	//分析桥梁应该延伸的方向
	auto& blocks = bw.teams[team].bridges[index].blocks;
	int beg = Clamp<int>(rate2mid * BRIDGE_BLOCK_CNT, 0, BRIDGE_BLOCK_CNT - 1);

	if (beg == 0)
		return team == RedTeam ? RIGHT : LEFT;
	if (beg == BRIDGE_BLOCK_CNT - 1)
		return team == BlueTeam ? RIGHT : LEFT;

	int i = beg;
	bool left_cut{false}, right_cut{false};
	while (i >= 0)
	{
		if (blocks[i] == Air)
		{
			left_cut = true;
			break;
		}
		--i;
	}

	i = beg;
	while (i <= BRIDGE_BLOCK_CNT - 1)
	{
		if (blocks[i] == Air)
		{
			right_cut = true;
			break;
		}
		++i;
	}

	if (left_cut && !right_cut)
		return LEFT;
	if (!left_cut && right_cut)
		return RIGHT;
	return Choice({ LEFT, RIGHT });
}

bool ChooseBridge(string_view name, BridgingMethod bm, int index = -1)
{	//准备搭路
	if (IsGameOver())
	{
		chat.AddChat(str(name) + " 目前无法搭路，因为游戏已经结束了", GetStringColor(name));
		return false;
	}

	if (bw.teams[bw.players[str(name)].team].exp < bridging_prices[bm])
	{
		chat.AddChat(str(name) + " 搭路失败，因为经验不足", GetStringColor(name));
		return false;
	}

	if (index < 0 || index >= TEAM_BRIDGE_MAX_CNT)
	{
		return false;
	}
	auto ptr = GetPlayerEntity(name);

	if (ptr)
	{
		if (!PlaceToBridge(ptr->place, ptr->x, ptr->y))
		{
			chat.AddChat(str(name) + "不在合适的位置，无法搭路！", bw.players[str(name)].team);
			AddTextTip(ptr->x, ptr->y - 15, "太远，无法搭路！", YELLOW, 3000L);
			return false;
		}
	}
	else
	{
		chat.AddChat(str(name) + "已死，无法搭路！", bw.players[str(name)].team);
		return false;
	}
	if (ptr->action == Bridging)
	{
		chat.AddChat(str(name) + " 已经在搭路了！", bw.players[str(name)].team);
		return false;
	}

	DIR2 dir;
	TeamID _team;
	if (ptr->place == CenterIsland)
	{
		if (RightTheta(ptr->theta))
		{
			dir = RIGHT;
			_team = BlueTeam;
		}
		else {
			dir = LEFT;
			_team = RedTeam;
		}
	}
	else if (ptr->place == RedHome)
	{
		dir = RIGHT;
		_team = RedTeam;
	}
	else if (ptr->place == BlueHome)
	{
		dir = LEFT;
		_team = BlueTeam;
	}
	else if (OnRedBridge(ptr->place))
	{
		dir = GetBridgeExtendDir(RedTeam, ptr->place - RedBridgeA, GetNowBridgePosRate(ptr->x, RIGHT));
		_team = RedTeam;
	}
	else if (OnBlueBridge(ptr->place))
	{
		dir = GetBridgeExtendDir(BlueTeam, ptr->place - BlueBridgeA, GetNowBridgePosRate(ptr->x, LEFT));
		_team = BlueTeam;
	}
	else {
		AddTextTip(ptr->x, ptr->y - 15, "没有到达可以搭桥的地点！", PINK, 2000L);
		return false;
	}

	if (bw.teams[_team].bridges[index].FullBlocks())
	{
		AddTextTip(ptr->x, ptr->y - 15, "这座桥已经很完整了！", PINK, 2000L);
		return false;
	}
	if (bw.teams[_team].FullBridge())
	{
		AddTextTip(_team == RedTeam ? 430 : 1550,
			370, "该侧桥梁数已满", MAGENTA, 2000L);
		//chat.AddChat(team_names[team] + "不需要搭桥！", team);
		return false;
	}
	/*if (bw.teams[team].IsBridging())
	{
		AddTextTip(team == RedTeam ? 430 : 1550,
			360, "搭桥不能同时进行！", ORANGE, 4000L);
		return false;
	}*/
	
	/*bw.teams[team].building_bridge = index;
	bw.teams[team].building_player = name;
	bw.teams[team].bridging_method = bm;
	bw.teams[team].lastBeginBridging = clock();*/

	ptr->action = Bridging;
	ptr->void_fall = false;
	ptr->theta = dir == RIGHT ? 0.0 : PI;

	bw.teams[_team].bridges[index].BeginBuild(dir, bm, name);
	bw.teams[_team].exp -= bridging_prices[bm];

	AddTextTip(/*team == RedTeam ? 530 : 1450,*/
		ptr->x,
		ptr->y - 15, "使用" + bridging_names[bm] + "搭桥！", PINK, 2000L);
	//chat.AddChat(team_names[team] + "使用" + bridging_names[bm] + "搭路！", WHITE);
	Sound(SndOrb);
	return true;
}
bool PurchaseArmor(string_view name, ArmorID armor)
{	//购买护甲
	if (IsGameOver())
	{
		chat.AddChat(str(name) + " 目前无法购买护甲，因为游戏已经结束了", GetStringColor(name));
		return false;
	}
	auto ptr = GetPlayerEntity(name);
	if (bw.players[str(name)].armor >= armor)
	{
		if (ptr)
			AddTextTip(ptr->x, ptr->y - 10, "无需购买该护甲", PINK, 1000L);
		chat.AddChat(str(name) + " 已有相同或更强护甲！", bw.players[str(name)].team);
		return false;
	}

	bw.players[str(name)].armor = armor;
	if (ptr)
		ptr->armor = armor;

	if (ptr)
		AddTextTip(ptr->x, ptr->y - 10, "护甲：" + armor_names[armor], ROYALBLUE, 1500L);
	else
		chat.AddChat(str(name) + " 护甲已升级为" + armor_names[armor], bw.players[str(name)].team);
	Sound(SndOrb);
	return true;
}
bool PurchaseSword(string_view name, SwordID sword)
{	//购买武器
	auto ptr = GetPlayerEntity(name);
	if (bw.players[str(name)].sword >= sword)
	{
		if (ptr)
			AddTextTip(ptr->x, ptr->y - 10, "无需购买"+sword_names[sword], PINK, 1000L);
		chat.AddChat(str(name) + " 已有相同或更强武器！", bw.players[str(name)].team);
		return false;
	}

	bw.players[str(name)].sword = sword;
	if (ptr)
		ptr->sword = sword;

	if (ptr)
		AddTextTip(ptr->x, ptr->y - 10, "武器：" + sword_names[sword], ROYALBLUE, 1500L);
	else
		chat.AddChat(str(name) + " 护甲已升级为" + sword_names[sword], bw.players[str(name)].team);
	Sound(SndOrb);
	return true;
}
bool TeamUpgrade(TeamID team, TeamUpgradeID tuid)
{	//团队升级
	if (IsGameOver())
	{
		chat.AddChat(team_names[team] + " 目前无法团队升级，因为游戏已经结束了", team);
		return false;
	}

	if (bw.teams[team].team_upgrade_levels.at(tuid) >=
		team_upgrade_prices.at(tuid).size())
	{
		AddTextTip(team == RedTeam ? 240 : 1630,
			300, team_upgrade_names.at(tuid) + "已经满级", RED, 2000L);
		//chat.AddChat(team_names[team] + "的" + team_upgrade_names.at(tuid) + "已经满级了！", team);
		return false;
	}
	if (bw.teams[team].diamonds <
		team_upgrade_prices.at(tuid).at(bw.teams[team].team_upgrade_levels.at(tuid)))
	{
		AddTextTip(team == RedTeam ? 200 : 1600,
			370, "钻石不足，无法升级", RED, 2000L);
		//chat.AddChat(team_names[team] + "钻石不足，无法升级" + team_upgrade_names.at(tuid), team);
		return false;
	}
	bw.teams[team].diamonds -= team_upgrade_prices.at(tuid).at(bw.teams[team].team_upgrade_levels.at(tuid));
	bw.teams[team].team_upgrade_levels.at(tuid)++;
	Sound(SndOrb);
	AddTextTip(team == RedTeam ? 260 : 1640,
		300, "团队升级!", ORANGE, 3000L);
	chat.AddChat(team_names[team] + "进行团队升级：" + team_upgrade_names.at(tuid), team);
	return true;
}
bool SummonArmy(TeamID team, string_view pname, EntityID eid, int bridge_index)
{	//召唤怪物
	if (IsGameOver())
	{
		chat.AddChat(team_names[team] + " 目前无法召唤军队，因为游戏已经结束了", team);
		return false;
	}
	if (bw.teams[team].exp < entity_prices[eid])
	{
		AddTextTip(team == RedTeam ? 200 : 1600,
			350, "经验不足", ORANGE, 2000L);
		
		return false;
	}
	if (bridge_index < 0)
		bridge_index = 1;

	double v = entity_speeds.at(eid) * 6.0;
	//选择桥梁
	double y = point_bridge_left_left[bridge_index].y;

#define ENTITY_SUBCLASS_CASE_MACRO(EntityType, ActualEntityType) \
		case EntityType:\
			{					\
				SpawnEntity<ActualEntityType>(eid, team, pname, \
					team == RedTeam ? BRIDGE_LEFT_LEFT - 150 : BRIDGE_RIGHT_RIGHT + 150, \
					y,		\
					team == RedTeam ? 0.0 : PI, \
					v);				\
				break;	\
			}

#define ENTITY_SUBCLASS_CASE_MACRO_2(EntityType, ActualEntityType, ActualEntityType2) \
		case EntityType:\
			{					\
				SpawnEntity<ActualEntityType, ActualEntityType2>(eid, team, pname, \
					team == RedTeam ? BRIDGE_LEFT_LEFT - 150 : BRIDGE_RIGHT_RIGHT + 150, \
					y,		\
					team == RedTeam ? 0.0 : PI, \
					v);				\
				break;	\
			}

	switch (entity_types.at(eid))
	{
		ENTITY_SUBCLASS_CASE_MACRO(PlayerType, PlayerEntity)

		ENTITY_SUBCLASS_CASE_MACRO(MeleeEntityType, MeleeEntity)

		ENTITY_SUBCLASS_CASE_MACRO(RangerEntityType, RangerEntity)

		ENTITY_SUBCLASS_CASE_MACRO_2(MeleeRangerEntityType, MeleeRangerEntity, RangerEntity)

		ENTITY_SUBCLASS_CASE_MACRO(ProjectileType, Projectile)

	default:
		{
			chat.AddChat("无效的军队实体类型！", team);
			return false;
		}
	}
	bw.stats.mob_purchase_cnt++;
	return true;
}
int GetAlphaIndex(string_view s)
{	//UTF-8
	if (s == "A" || s == "a")
		return 0;
	if (s == "B" || s == "b")
		return 1;
	if (s == "C" || s == "c")
		return 2;
	return -1;
}
int ParseBridgeIndex(TeamID team, string_view s, string_view prefix, bool create = false)
{	//返回 -1 表示操作失败

	string diff = strxhead(str(s), prefix.length());
	//chat.AddChat("diff=\"" + diff + "\"", team);

	if (isalpha(diff[0]))
	{
		auto index = GetAlphaIndex(diff);/*diff[0] - isupper(diff[0]) ? 'A' : 'a';*/
		chat.AddChat("选择的桥梁：" + str(char('A' + index)), team);
		return index;
	}
	else if (!diff.empty()){
		//无效的桥梁编号
		chat.AddChat("无效的桥梁编号：\"" + diff + "\"，正确格式为字母，请重新输入", RED);
		return -1;
	}
	//chat.AddChat("RandomBridge", team);

	if (create)
	{
		static int _map[]{ 1, 2, 0 };
		for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		{
			if (! bw.teams[team].bridges[_map[i]].CanPass()
				&& ! bw.teams[team].bridges[_map[i]].building)
			{
				return _map[i];
			}
		}
		chat.AddChat("桥梁已满，无法操作！", ORANGE);
		return -1;
	}
	else
	{	//选择已存在的桥梁
		vector<int> availables;
		for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		{
			if (bw.teams[team].bridges[i].CanPass())	//完整的桥梁
			{
				availables.push_back(i);
			}
		}
		if (availables.empty())
		{
			chat.AddChat("没有桥梁，无法操作！", YELLOW);
			return -1;
		}

		if (s.length() == prefix.length())	//不指定桥梁编号
			return Choice(availables);		//则随机桥梁
	}

	return -1;
}

size_t DiamondCollectMaxCount()
{	//每次可以收集多少钻石
	//return RandomRange(1, 3, true, true);
	return Choice({ 1, 1, 2, 2, 2, 3, 3 });
}

void CollectDiamond(TeamID team, int index)
{	//收集钻石
	if (IsGameOver())
	{
		chat.AddChat(team_names[team] + " 目前无法捡钻石，因为游戏已经结束了", team);
		return;
	}
	size_t cnt = min(DiamondCollectMaxCount(), bw.diamond_cnt[index]);
	if (cnt <= 0)
		return;
	AddTextTip(team == RedTeam ? 220 : 1690,
		370, "钻石 +"+str(cnt), AQUA, 3000L);
	bw.teams[team].diamonds += cnt;
	bw.diamond_cnt[index] -= cnt;
	bw.stats.diamond_collected_cnt += cnt;
	Sound(SndItem);
	//chat.AddChat(team_names[team] + "成功收集一颗钻石!", team);
}
void CollectEmeralds(TeamID team)
{	//收集绿宝石
	if (bw.emerald_cnt <= 0)
		return;

	AddTextTip(team == RedTeam ? 220 : 1690,
		350, "中心岛经验 +" + str(100L * bw.emerald_cnt), LIGHTGREEN, 3000L);

	//一次全部捡完
	bw.teams[team].exp += 100L * bw.emerald_cnt;
	bw.emerald_cnt = 0;
	
	Sound(SndItem);
}
bool PurchaseItem(TeamID team, ItemID item)
{	//购买物品
	if (IsGameOver())
	{
		chat.AddChat(team_names[team] + " 目前无法购买物品，因为游戏已经结束了", team);
		return false;
	}
	if (bw.teams[team].exp < item_prices.at(item))
	{
		chat.AddChat("经验不足，无法购买" + item_names.at(item) + "!", team);
		return false;
	}
	chat.AddChat(team_names[team] + "购买" + item_names.at(item), team);
	bw.teams[team].items[item] += item_amounts[item];
	bw.teams[team].exp -= item_prices.at(item);
	bw.stats.item_purchase_cnt ++;
	Sound(SndItem);
	return true;
}

const vector<double> enderpearl_throw_vhs
{
	150.0,
	50.0,
	10.0,
};

#define USEITEM_THROW_STRONG 0
#define USEITEM_THROW_MEDIUM 1
#define USEITEM_THROW_WEAK 2
#define USEITEM_THROW_HOME 3
#define USEITEM_THROW_OPPO 4

#define USEITEM_TNT_JUMP_LEFT 2
#define USEITEM_TNT_JUMP_RIGHT 3
#define USEITEM_TNT_BLOW_BED 4

#define TNT_V_H 5.0
#define TNT_BLOW_BED_DIST_MAX 200.0
#define TNT_JUMP_RADIUS 20.0
#define TNT_JUMP_PLAYER_V_H 80.0

#define USEITEM_FIREBALL_JUMP 2
#define USEITEM_FIREBALL_JUMP_BACK 3

#define FIREBALL_JUMP_RADIUS 15.0
#define FIREBALL_JUMP_PLAYER_V_H 30.0
#define FIREBALL_JUMP_FB_V_H -30.0

bool UseItem(TeamID team, string_view pname, ItemID iid, int param = 1)
{	//使用物品
	if (iid == 0)
		return false;

	if (IsGameOver())
	{
		chat.AddChat(str(pname) + " 目前无法使用物品，因为游戏已经结束了", GetStringColor(pname));
		return false;
	}

	auto ptr = GetPlayerEntity(pname);
	if (!ptr)
		return false;

	if (bw.teams[team].items[iid] <= 0)
	{	//还没有该物品
		if (!PurchaseItem(team, iid))	//当场买
			return false;
	}

	if (iid == FireballItem)
	{	//火球
		double _x{ ptr->x }, _y{ ptr->y };
		double _v_h = 0.0;
		double _v = entity_speeds.at(Fireball);
		if (param != 1)
		{
			_v *= 0.05;
			_v_h = FIREBALL_JUMP_FB_V_H;
			_x = ptr->x + (param == USEITEM_FIREBALL_JUMP ? 1.0 : -1.0) * FIREBALL_JUMP_RADIUS;
			ptr->Jump(FIREBALL_JUMP_PLAYER_V_H);
		}
		CreateProjectile(Fireball, team, pname, _v, _v_h, 
			_x, _y, ptr->h + 20.0,
			param != 1 ? 
				OppoRadian(ptr->theta) : ptr->theta,
			entity_melee_damages.at(Fireball));
		//如果不行就强行给玩家一个力
	}
	else if (iid == EnderPearlItem)
	{	//末影珍珠
		double v_h = 0.0;
		double v_p = entity_speeds.at(EnderPearl);
		if (param < USEITEM_THROW_HOME)
		{
			v_h = enderpearl_throw_vhs.at(param);
			CreateProjectile(EnderPearl, team, pname, v_p, v_h, ptr->x, ptr->y, ptr->h + 20.0, ptr->theta,
				0.0);
		}
		else {
			double dist = ptr->team == RedTeam ? (ptr->x - point_beds[RedTeam].x) : (point_beds[BlueTeam].x - ptr->x);
			CalcParabolaSpeed(dist, v_p, v_h);
			CreateProjectile(EnderPearl, team, pname, v_p, v_h, ptr->x, ptr->y, ptr->h+20.0,
				(ptr->team == RedTeam ? (param == USEITEM_THROW_HOME ? PI : 0.0) 
									  : (param == USEITEM_THROW_HOME ? 0.0 : PI)),
				0.0);
		}
		IncUserStat(pname, "ender_pearl_cnt");
		//丢末影珍珠音效缺失
	}
	else if (iid == TNT)
	{	//TNT
		double _theta{ ptr->theta };
		double _x{ ptr->x }, _y{ ptr->y };
		double _v{ entity_speeds.at(PrimedTNT) };
		double _v_h{ TNT_V_H };

		if (param == USEITEM_TNT_BLOW_BED)
		{
			TeamID oppo = team == RedTeam ? BlueTeam : RedTeam;
			if (Distance(ptr->x, ptr->y, point_beds[oppo].x, point_beds[oppo].y)
				> TNT_BLOW_BED_DIST_MAX)
			{
				AddTextTip(ptr->x, ptr->y - 10,
					"离敌方床太远！", YELLOW, 2000L);
				return false;
			}
			//精准投放
			_x = point_beds[oppo].x;
			_y = point_beds[oppo].y;
			_v = 0.0;
			_v_h = 4.0;
			_theta = oppo == BlueTeam ? 0.0 : PI;
			AddTextTip(ptr->x, ptr->y - 10,
				"炸弹已安置", ORANGE, 2000L);
		}
		else if (param == USEITEM_TNT_JUMP_LEFT || param == USEITEM_TNT_JUMP_RIGHT)
		{	//TNT 跳
			_v = 0.0;
			_v_h = 2.0;
			double p_jump_v_h = TNT_JUMP_PLAYER_V_H;
			if (param == USEITEM_TNT_JUMP_LEFT)
				_x = ptr->x + TNT_JUMP_RADIUS;
			else
				_x = ptr->x - TNT_JUMP_RADIUS;

			//TNT跳要领：放置TNT，快要爆炸的时候起跳
			ptr->Stop();
			ptr->LockMoveFor(3000L);
			ptr->Jump(p_jump_v_h);
		}
		CreateProjectile(PrimedTNT, team, pname, _v, _v_h,
			_x, _y, ptr->h + 20.0, _theta,
			entity_melee_damages.at(PrimedTNT));
	}
	else if (iid == Sponge)
	{	//海绵
		//TeamID oppo = team == RedTeam ? BlueTeam : RedTeam;
		double dist_1 = Distance(ptr->x, ptr->y, point_beds[RedTeam].x, point_beds[RedTeam].y);
		double dist_2 = Distance(ptr->x, ptr->y, point_beds[BlueTeam].x, point_beds[BlueTeam].y);
		if (dist_1 > TNT_BLOW_BED_DIST_MAX
				&& dist_2 > TNT_BLOW_BED_DIST_MAX)
		{
			AddTextTip(ptr->x, ptr->y - 10,
				"离床太远！", YELLOW, 2000L);
			return false;
		}
		Bed& bed = bw.teams[dist_1 > dist_2 ? BlueTeam : RedTeam].bed;
		if (!bed.HasWater())
		{
			AddTextTip(ptr->x, ptr->y - 10,
				"该床没有放水！", YELLOW, 2000L);
			return false;
		}
		//自己床也可以
		bed.defense[Water] = 0;
	}
#define GOLDENAPPLE_HEAL_MAXHP 4.0
	else if (iid == GoldenApple)
	{	//金苹果
		if (ptr->max_hp <= entity_hps[ptr->id])		//黄血不能叠加！
			ptr->Heal(0.0, GOLDENAPPLE_HEAL_MAXHP);
		ptr->buffs.AddBuff(Regeneration, 5000L);	//五秒的生命恢复
		Sound(SndEat);
	}
	else if (potion_durations.find(iid) != potion_durations.end())
	{	//药水类
		ptr->buffs.AddBuff(potion_buffs[iid], potion_durations[iid]);
		Sound(SndDrink);
	}
#define KBSTICK_RANGE	55.0

	else if (iid == KnockbackStick)
	{	//击退棒需要输入弹幕使用
		auto ptr2 = ptr->GetNearestEnemy();
		if (!ptr2)
		{
			AddTextTip(ptr->x, ptr->y - 10, "没有敌人", GetStringColor(ptr->name), 1500L);
			return false;
		}
		if (Distance(ptr2->x, ptr2->y, ptr->x, ptr->y) > KBSTICK_RANGE)
		{
			AddTextTip(ptr->x, ptr->y - 10, "离敌人太远了", GetStringColor(ptr->name), 1500L);
			return false;
		}

		//1 点伤害
		DamageSource ds;
		ds.damage = 1.0;
		ds.eid = PlayerAvatar;
		ds.owner_username = ptr->name;
		ds.team = ptr->team;

		//计算受击角度
		double _theta = atan2(ptr->y - ptr2->y, ptr2->x - ptr->x);
		double _theta_h = atan2(ptr2->h - ptr->h, Distance(ptr->x, ptr->y, ptr2->x, ptr2->y));

		ptr2->OnHurt(ds);
		ptr2->SufferImpulse(80.0, _theta, _theta_h);

		Sound(SndPlayerAttack);
		AddTextTip(ptr->x, ptr->y - 10, "使用击退棒!", GetStringColor(ptr->name), 2000L);
		return true;	//不消耗击退棒
	}
	bw.teams[team].LoseItem(iid, 1);
	AddTextTip(ptr->x, ptr->y - 10, "使用" + item_names[iid], ORANGE);
	return true;
}

map<BridgingMethod, string> bridging_statmap
{
	{Sneaking, "sneaking_cnt"},
	{NinjaBridging, "ninjabridging_cnt"},
	{Moonwalk, "moonwalk_cnt"},
	{GodBridging, "godbridging_cnt"},
};

void Player::OnBridgeDone(BridgingMethod bm)
{
	Sound(SndPling);
	chat.AddChat(name + "已完成搭路！", bw.players[name].team);

	auto ptr = GetPlayerEntity(name);
	if (ptr)
	{
		ptr->TurnAround();
		//ptr->theta = ptr->v.Theta();	//go forward
		ptr->ResumeMoving();
		/*ptr->UpdateSpeed();*/
		ptr->action = Moving;
	}

	IncUserStat(name, bridging_statmap[bm]);
	AddScore(name, ScoreItem::Bridge);
}

bool ExistPlayer(string_view name)
{
	return bw.players.find(str(name)) != bw.players.end();
}
void AddPlayer(string_view name, TeamID team)
{
	Player p(name, team);
	if (bw.started)
		p.Spawn();
	bw.players.insert(make_pair(name, p));
	bw.teams[team].player_names.push_back(name.data());
	chat.AddChat("玩家 \"" + str(name) + "\" 加入了 " + team_names[team] + "!", team);
	AddTextTip(team == RedTeam ? 450 : 1500,
		700, "[+] " + str(name), team, 5000L);
	Sound(SndEnter);
}
void RemovePlayer(string_view name)
{
	auto ptr = GetPlayerEntity(name);

	AddTextTip(bw.players[string(name)].team == RedTeam ? 350 : 1600,
		690, "[-] " + str(name), WHITE, 2000L);
	chat.AddChat("玩家 \"" + str(name) + "\" 退出了 " + team_names[bw.players[string(name)].team] + "!", bw.players[string(name)].team);
	
	if (ptr && ptr->action == Bridging)
	{
		chat.AddChat("中断" + team_names[bw.players[string(name)].team] + "的搭桥操作", RED);
		//bw.teams[bw.players[string(name)].team].bridges[bw.teams[bw.players[string(name)].team].building_bridge];
		bw.teams[bw.players[string(name)].team].InterruptBridging(name);
		//Interrupt Bridging
	}
	
	bw.players.erase(str(name));
	for (auto& tm : bw.teams)
	{
		tm.second.player_names.erase(
			remove_if(tm.second.player_names.begin(), tm.second.player_names.end(),
				[=](const string& pname) {return pname == name; }),
			tm.second.player_names.end()
		);
	}
	bw.entities.erase(
		remove_if(bw.entities.begin(), bw.entities.end(),
			[=](const auto& pet) {return pet->name == name; }),
		bw.entities.end()
	);
	Sound(SndLeave);
}

#include "helpinfo.h"

void InitGame()
{
	cout << "InitGame...\n";
	bw.teams.insert(make_pair(RedTeam, Team(RedTeam)));
	bw.teams.insert(make_pair(BlueTeam, Team(BlueTeam)));
	bw.random_team_players.clear();
	bw.started = false;
	bw.over = false;
	bw.InitStats();
}
void SpawnPlayers()
{
	for (auto const& pr : bw.teams)
	{
		for (const auto& pname : pr.second.player_names)
		{
			bw.players[pname].Spawn();
		}
	}
}
void StartGame()
{
	SpawnPlayers();
	bw.started = true;
	bw.over = false;
	bw.start = clock();

	globalstats["game_cnt"]++;

	chat.AddChat("游戏开始！", GREEN);
	CenterTitleEx("游戏开始", YELLOW, "保护你的床", LIGHTRED);
}
void DisposeGame()
{
	cout << "DisposeGame...\n";
	bw.started = false;
	bw.players.clear();
	bw.teams.clear();
	bw.entities.clear();
	bw.over = true;
	bw.end = clock();
	chat.Clear();
	chat.AddChat("游戏关闭！", BLACK);
}

#include "chat_runner.h"

#define GAMEOVER_TNT_CD 300.0

void UpdateGameOverCelebration()
{
	if (!bw.over)
		return;

	if (bw.stats.remain_tnt > 0U 
		&& clock() - bw.stats.last_tnt >= GAMEOVER_TNT_CD)
	{
		auto i = (bw.stats.tnt_cnt - bw.stats.remain_tnt);
		SpawnEntity<Projectile>(PrimedTNT, Choice({RedTeam, BlueTeam}), ""sv,
			100 + (uix / bw.stats.tnt_cnt) * i, point_center.y, PI, 0.0,
			i * 50.0);

		--bw.stats.remain_tnt;
		bw.stats.last_tnt = clock();
	}
}

void GameOver(TeamID winner)
{
	if (bw.over)
		return;

	bw.over = true;
	bw.end = clock();

	bw.stats.remain_tnt = bw.stats.tnt_cnt = Clamp(20 - bw.entities.size(), 3U, 18U);
	bw.stats.last_tnt = clock();

	chat.AddChat(team_names[winner] + "胜利！", winner);

	//中央标题
	CenterTitleEx(team_names[winner] + "胜利！", winner, "用时 " + TimeString(GetGameDuration() / 1000), CYAN);

	//头顶显示
	for (auto const& pname : bw.teams[winner].player_names)
	{
		AddScore(pname, ScoreItem::Win);
		IncUserStat(pname, "win_cnt");

		auto ptr = GetPlayerEntity(pname);
		if (ptr)
		{
			AddTextTip(ptr->x, ptr->y - 10, "胜利！", GetStringColor(pname), 5000L);
		}
	}

	//保存统计数据
	SaveGlobalStats();
	SaveUserStats();

	Sound(SndLevelup);
}
void OnBedDestroy(TeamID team, string_view who)
{	//床毁
	if (IsGameOver())
		return;

	CenterTitleEx(team_names2[team], team, "床已被破坏，破坏者：" + str(who), GOLD);
	if (bw.teams[team].CountAlivePlayers() <= 0)
	{	//人亡
		GameOver(team == RedTeam ? BlueTeam : RedTeam);
	}
}
void OnPlayerDeath(string_view pname)
{	//人亡
	if (!bw.teams[bw.players[str(pname)].team].bed.IsActive()
		&& bw.teams[bw.players[str(pname)].team].CountAlivePlayers() <= 0)
	{	//床毁
		GameOver(bw.players[str(pname)].team == RedTeam ? BlueTeam : RedTeam);
	}
}
void CheckWin()
{
	if (bw.over)
		return;

	static TeamID _teams[]{ RedTeam, BlueTeam };
	for (TeamID t : _teams)
	{
		if (bw.teams[t].CountAlivePlayers() <= 0 
			&& !bw.teams[t].bed.IsActive())
		{
			GameOver(OppoTeam(t));
			return;
		}
	}
}
#define GAMEOVER_ANIMATION_DURATION 2000L

void DrawGameOver()
{
	if (!bw.over)
		return;
	clock_t end2 = bw.end + 4000L;
	//模糊+变暗
	double rat = Clamp((clock() - end2) / double(GAMEOVER_ANIMATION_DURATION), 0.0, 1.0);
	imagefilter_blurring(nullptr, Lerp(0x01, 0x50, rat), Lerp(0xFE, 0x55, rat));

	int _y = Lerp(uiy / 2 - g.center_title_fs / 2.0, uiy * 0.2, EaseOutCubic(rat));
	setfont(g.center_title_fs, 0, DEF_FONTNAME);
	setcolor(g.center_title_color);
	midprinty_shadow(g.center_title, _y);
	setfont(g.center_subtitle_fs, 0, DEF_FONTNAME);
	setcolor(g.center_subtitle_color);
	midprinty_shadow(g.center_subtitle, _y + g.center_title_fs + 10);

	constexpr int delta_x = 280;
	constexpr int name_fs = 26;
	constexpr int name_v_add = 65;
	constexpr int name_top = 500;
	static int col_midxes[]{ uix / 2 - delta_x, uix / 2 + delta_x };

	double arat = GetFadeinoutRate(GAMEOVER_ANIMATION_DURATION, g.center_title_duration, clock() - g.center_title_start);
	TeamID teams[]{RedTeam, BlueTeam};
	int x = 0;
	for (auto t : teams)
	{	//打印队员
		int y = 0;
		setfont(40, 0, DEF_FONTNAME);
		setcolor(StepColor(BLACK, t, rat));

		if (!bw.teams[t].player_names.empty())
		{
			midprinty_shadow(team_names[t]+" ("+str(bw.teams[t].player_names.size())+" 人)", name_top - 50, col_midxes[x]);
			setfont(name_fs, 0, DEF_FONTNAME);
			for (auto const& pname : bw.teams[t].player_names)
			{	//玩家昵称
				setwcolor(EGECOLORA(t, int(255 * rat)));
				midprinty_shadow(pname, name_top + name_v_add * y, col_midxes[x]);
				if (bw.stats.player_scores.find(pname) != bw.stats.player_scores.end())
				{	//玩家分数
					setwcolor(EGECOLORA(GetStringColor(pname), int(255 * rat)));
					midprinty_shadow(str(bw.stats.player_scores[pname])+" 分", name_top + name_v_add * y + name_fs, col_midxes[x]);
				}
				++y;
			}
		}
		else {
			midprinty_shadow(team_names[t], name_top - 50, col_midxes[x]);
			//setwcolor(StepColor(BLACK, InvertedColor(t), rat));
			setwcolor(EGECOLORA(InvertedColor(t), int(255 * rat)));
			midprinty_shadow("-/-", name_top + 100, col_midxes[x]);
		}
		++x;
	}

	static int btm_top = uiy * 0.85;

	setfont(25, 0, DEF_FONTNAME);
	//setcolor(StepColor(BLACK, WHITE, rat));
	setcolor(EGECOLORA(WHITE, int(255 * rat)));
	//midprinty_shadow("弹幕数量：" + str(bw.stats.chat_cnt), btm_top);
	rectprintf_shadow(Lerp<int>(-200, uix * 0.45, EaseOutCubic(rat)), uiy * 0.6,
		500, 600,
		"这是第 #%zu 场比赛；\n"
		"弹幕数量：%zu\n"
		"收集钻石总数：%zu\n"
		"物品购买次数：%zu\n"
		"防御购买次数：%zu\n"
		"军队购买次数：%zu\n"
		,
		globalstats["game_cnt"],
		bw.stats.chat_cnt,
		bw.stats.diamond_collected_cnt,
		bw.stats.item_purchase_cnt,
		bw.stats.block_purchase_cnt,
		bw.stats.mob_purchase_cnt
	);

	setcolor(RainbowColor());
	midprinty_shadow("请输入\"下一把\"或\"继续\"", Lerp<double>(uiy, uiy * 0.9, EaseOutCubic(rat)));
}

#define EMERALD_MAX_DROP 5
#define DIAMOND_MAX_DROP 5

void GameUpdate()
{	//游戏更新

	CheckWin();
	UpdateGameOverCelebration();

	static clock_t lastIron = clock();
	static clock_t lastGold = clock();

	//家里的资源点刷新
	bw.teams[RedTeam].UpdateResources();
	bw.teams[BlueTeam].UpdateResources();

	//中心岛绿宝石点刷新
	if (clock() - bw.lastEmerald >= EMERALD_CD
		&& bw.emerald_cnt < EMERALD_MAX_DROP)
	{
		bw.emerald_cnt++;
		bw.lastEmerald = clock();
	}

	//两边岛屿钻石点刷新
	for (int i = 0; i < 2; ++i)
		if (clock() - bw.lastDiamond[i] >= DIAMOND_CD
			&& bw.diamond_cnt[i] < DIAMOND_MAX_DROP)
		{
			Sound(SndPlop);
			bw.diamond_cnt[i]++;
			bw.lastDiamond[i] = clock();
		}

	//更新桥梁
	bw.teams[RedTeam].UpdateBridging();
	bw.teams[BlueTeam].UpdateBridging();

	//for (auto& pet : bw.entities)
	for (int i = 0 ; i < bw.entities.size(); ++i)
	{
		auto pet = bw.entities.at(i);
		if (!pet || !pet->active)
			continue;

		pet->Update();
		//全员攻击

		if (pet->AtOppoHome() 
			&& pet->NearOppoBed() 
			//如果靠近敌方床，很难做到不去挖
			&& pet->CanDigBed(pet->GetOppoBed()))
		{	//挖床	
			
			//AddTextTip(pet->x, pet->y - 10, "开始挖床！", ORANGE, 1000L);
			pet->SetAction(Digging);
			pet->TurnToOppoBed();
			//pet->Stop();
			pet->TryToDigOppoBed();
		}
		else 
		{	//攻击
			/*if (pet->v.IsZero())
				pet->ResumeMoving();*/
			pet->TryToAttack();
		}

		if (pet->GetType() == PlayerType
			&& AtCenter(pet->x, pet->y))
		{
			CollectEmeralds(pet->team);
		}
	}

	//失效的文字提示 删除之
	text_tips.erase(
		remove_if(text_tips.begin(), text_tips.end(),
			[=](auto& tip) { return !tip.IsActive(); }),
		text_tips.end()
	);

	//死去的实体 删除之
	bw.entities.erase(
		remove_if(bw.entities.begin(), bw.entities.end(),
			[=](auto& pet) { return !pet->active; }),
		bw.entities.end()
	);
}
void GameControl()
{	//辅助控制

	if (K(VK_ESCAPE))
	{
		Sound(SndLeave);
		cout << "按 Esc 退出游戏\n";
		g.playing = false;
	}
	else if (bw.over && K(VK_SPACE))
	{
		Sound(SndLeave);
		cout << "按 空格 进入下一把游戏\n";
		g.retry = true;
		g.playing = false;
	}
}

#define COMMAND_FILENAME L"commands.txt"
#define COMMAND_SEP		 '`'
void GameInputCheck()
{
	if (!ExistFile(COMMAND_FILENAME))
		return;

	vector<string> lines = ReadFileLines(COMMAND_FILENAME);

	for (auto const& line : lines)
	{
		vector<string> cut = CutLine(line, COMMAND_SEP);
		if (cut.size() < 2)
			continue;
		cout << cut[0] << "   " << cut[1] << '\n';
		RunChatCommand(cut[0], cut[1]);
	}

	//Clear
	fstream fout(COMMAND_FILENAME, ios::out);
	fout.close();
}
