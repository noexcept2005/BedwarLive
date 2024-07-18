#pragma once

enum EntityID : USHORT
{
	PlayerAvatar	= 0,
	Zombie			= 1,
	Silverfish		= 2,
	Vindicator		= 3,
	Skeleton		= 4,
	Pillager		= 5,
	Creeper			= 6,
	Blaze			= 7,
	Arrow			= 8,
	SmallFireball	= 9,
	Fireball		= 10,
	EnderPearl		= 11,
	PrimedTNT		= 12,
};

bool NonRotativeProjectile(EntityID eid)
{
	return eid == PrimedTNT;
}
bool NoGravityEntity(EntityID eid)
{
	return eid == Fireball || eid == SmallFireball;
}

Defense GetTeamDefense(TeamID team);
void MakeExplosion(double x, double y, double r, EntityID eid, string_view username, double damage);

enum IQLevel {
	Stupid = 0,		//只会往前走
	Normal = 1,		//碰到虚空会停下
	Smart  = 2,		//碰到虚空会返回
};

const vector<IQLevel> entity_iqs
{
	Normal,	//Smart,
	Normal,
	Stupid,
	Normal,
	Normal,
	Normal,
	Normal,
	Normal,
	Stupid,
	Stupid,
	Stupid,
	Stupid,
	Stupid,
};

map<EntityID, SoundID> entity_hurt_sounds
{
	{PlayerAvatar, SndHit},
	{Zombie, SndHitZombie},
	{Skeleton, SndHitSkeleton},
	{Creeper, SndHitCreeper},
	{Silverfish, SndHitSilverfish},
	{Vindicator, SndHitVindicator},
	{Pillager, SndHitPillager},
	{Blaze, SndHitBlaze},
};

using Knockback = double;

struct DamageSource	//单次伤害信息
{
	EntityID eid;
	TeamID team;
	string owner_username{ "" };
	double damage{ 1.0 };
};

//实体类型
enum EntityType : USHORT
{
	PlayerType				= 0,
	MeleeEntityType			= 1,
	RangerEntityType		= 2,
	MeleeRangerEntityType	= 3,
	ProjectileType			= 4,
};

inline bool EdgeBackEntityType(EntityType etype)
{
	return (etype == PlayerType || etype == MeleeEntityType || etype == RangerEntityType || etype == MeleeRangerEntityType);
}

const vector<string> entity_names
{
	"玩家",
	"僵尸",
	"蠹虫",
	"卫道士",
	"骷髅",
	"掠夺者",
	"苦力怕",
	"烈焰人",
	"箭",
	"小火球",
	"火球",
	"末影珍珠",
	"TNT",
};

const vector<EntityType> entity_types
{
	PlayerType,
	MeleeEntityType,
	MeleeEntityType,
	MeleeEntityType,
	RangerEntityType,
	RangerEntityType,
	MeleeEntityType,	//...
	MeleeRangerEntityType,
	ProjectileType,
	ProjectileType,
	ProjectileType,
	ProjectileType,
	ProjectileType,
};
const vector<double> entity_speeds
{
	7.0,
	4.0,
	6.0,
	6.0,
	4.0,
	4.0,
	4.0,
	3.0,
	130.0,
	90.0,
	100.0,
	350.0,
	10.0,
};
const vector<long> entity_prices
{
	0L,
	5L,
	3L,
	30L,
	10L,
	15L,
	20L,
	25L,
	//弹射物实体不出售 ↓
	999L,
	999L,
	100L,
	999L,
	999L,
};
const vector<double> entity_hps
{
	30.0,
	20.0,
	5.0,
	24.0,
	20.0,
	24.0,
	20.0,
	20.0,
	1.0,
	1.0,
	1.0,
	1.0,
	1.0,
};
//
//const vector<double> entity_damages
//{
//	3.0,	//.
//	3.0,	//.
//	1.0,
//	7.5,	//with axe
//	3.0,	//arrow
//	4.0,	//arrow
//	22.5,	//blow
//	3.5,	//small fireball
//};

map<EntityID, double> entity_melee_damages
{
	{PlayerAvatar, 3.0},
	{Zombie, 3.0},
	{Silverfish, 1.0},
	{Vindicator, 7.5},
	{Creeper, 22.5},
	{Blaze, 4.0},	//melee

	//弹射物伤害由发射者决定
	{Arrow, 1.0},
	{SmallFireball, 1.0},
	{Fireball, 5.0},
	{EnderPearl, 0.0},
	{PrimedTNT, 20.0},
};

map<EntityID, double> entity_ranger_damages
{
	{Skeleton, 3.0},
	{Pillager, 4.0},
	{Blaze, 3.5},
};

enum Place
{
	OtherPlace = 0,
	RedHome  = 1,
	BlueHome = 2,

	RedBridgeA,
	RedBridgeB,
	RedBridgeC,

	BlueBridgeA,
	BlueBridgeB,
	BlueBridgeC,

	CenterIsland,
};

const vector<string> place_names
{
	"其他地点",
	"红队家",
	"蓝队家",

	"红桥A",
	"红桥B",
	"红桥C",

	"蓝桥A",
	"蓝桥B",
	"蓝桥C",

	"中心岛",
};

inline bool AtHome(Place place)
{
	return place == RedHome || place == BlueHome;
}
inline bool OnBridge(Place place)
{
	return place >= RedBridgeA && place <= BlueBridgeC;
}
inline bool OnRedBridge(Place place)
{
	return place >= RedBridgeA && place <= RedBridgeC;
}
inline bool OnBlueBridge(Place place)
{
	return place >= BlueBridgeA && place <= BlueBridgeC;
}

#define PLACE_JUDGE_RADIUS 100.0
#define PLACE_JUDGE_DELTA_Y 200.0

bool VoidPos(Place place, double x, double y);

bool AtCenter(double x, double y)
{
	return abs(x - point_center.x) < PLACE_JUDGE_RADIUS
		&& abs(y - point_center.y) < PLACE_JUDGE_DELTA_Y;
}
bool AtBridgeLeftLeft(double x, double y)
{
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		if (Distance(x, y, point_bridge_left_left[i].x, point_bridge_left_left[i].y)
		< PLACE_JUDGE_RADIUS)
			return true;
	return false;
}
bool AtBridgeRightRight(double x, double y)
{
	for (int i = 0; i < TEAM_BRIDGE_MAX_CNT; ++i)
		if (Distance(x, y, point_bridge_right_right[i].x, point_bridge_right_right[i].y)
			< PLACE_JUDGE_RADIUS)
			return true;
	return false;
}
bool AtCenterIsland(double x, double y)
{
	return (x >= point_centerisland_left.x && x <= point_centerisland_right.x
		&& abs(y - point_centerisland_left.y) < PLACE_JUDGE_DELTA_Y);
}
bool AtCenterIslandLeft(double x, double y)
{
	return abs(x - point_centerisland_left.x) < PLACE_JUDGE_RADIUS
		&& abs(y - point_centerisland_left.y) < PLACE_JUDGE_DELTA_Y;
	/*return Distance(x, y, point_centerisland_left.x, point_centerisland_left.y)
		< PLACE_JUDGE_RADIUS;*/
}
bool AtCenterIslandRight(double x, double y)
{
	/*return Distance(x, y, point_centerisland_right.x, point_centerisland_right.y)
		< PLACE_JUDGE_RADIUS;*/
	return abs(x - point_centerisland_right.x) < PLACE_JUDGE_RADIUS
		&& abs(y - point_centerisland_right.y) < PLACE_JUDGE_DELTA_Y;
}
bool WannaOnBridge(TeamID whichTeamBridge, DIR dir, double x, double y)
{
	if (whichTeamBridge == RedTeam)
		return dir == RIGHT
		&& AtBridgeLeftLeft(x, y)
		|| dir == LEFT
		&& AtCenterIslandLeft(x, y);
	else
		return dir == LEFT
		&& AtBridgeRightRight(x, y)
		|| dir == RIGHT
		&& AtCenterIslandRight(x, y);
}
double GetNowBridgePosRate(double x, DIR dir)
{
	if (x < uix / 2)
	{
		if (dir == RIGHT)
			return Clamp((x - point_bridge_left_left[0].x)
				/ double(point_centerisland_left.x - point_bridge_left_left[0].x),
				0.0, 1.0);
		else
			return 1.0 - Clamp((x - point_bridge_left_left[0].x)
				/ double(point_centerisland_left.x - point_bridge_left_left[0].x),
				0.0, 1.0);
	}
	else {
		if (dir == LEFT)
			return 1.0 -
				Clamp((x - point_centerisland_right.x)
					/ double(point_bridge_right_right[0].x - point_centerisland_right.x),
					0.0, 1.0);
		else
			return Clamp((x - point_centerisland_right.x)
				/ double(point_bridge_right_right[0].x - point_centerisland_right.x),
				0.0, 1.0);
	}
}
bool OnIncompleteBridge(Place place);

bool PlaceToBridge(Place place, double x, double y)
{
	return (AtHome(place)
		|| AtBridgeLeftLeft(x, y) || AtBridgeRightRight(x, y)
		//|| AtCenterIslandLeft(x, y) || AtCenterIslandRight(x, y)
		|| AtCenterIsland(x, y)
		|| OnIncompleteBridge(place)
		);
}

map<EntityID, double> entity_explosion_radii
{
	{Creeper, 100.0},
	{Fireball, 50.0},
	{PrimedTNT, 180.0},
};

enum class ForceCmd		//对玩家的命令
{
	Cancelled	= 0,	//没有强制要求
	Stop,				//停下
	Home,				//回家
	Charge,				//去对面家
};

#define ET_FIGURE_UPDATE_CD 500L
#define ET_DEF_MAX_HP 20.0
#define ET_DEF_DAMAGE 1.0
#define ET_JUMP_SPEED 10.0

#define HPBAR_COLOR		EGERGB(255,55,55)
#define HPBAR_COLOR2	EGERGB(185,20,20)

class Entity
{
public:
	TeamID team{ BlueTeam };
	EntityID id;
	string name{ "*Entity*" };		//B站昵称不允许包含减号和下划线以外的特殊字符
	string owner_username{ "" };

	int variant{ 0 };
	PIMAGE pimg{ nullptr };

	bool active{ true };
	double hp{ ET_DEF_MAX_HP };
	double max_hp{ ET_DEF_MAX_HP };

	int target_bridge_index{ -2 };

	double x;
	double y;
	double h{ 0.0 };

	double theta{ 0.0 };
	
	bool move{ true };
	Vector2D v;
	double v_h{ 0.0 };

	bool void_fall{ false };	//是否掉入虚空
	Place place;	//实时更新

	BuffAttribute buffs;

	clock_t start{ 0L };
	clock_t lastMove{ 0L };
	clock_t lastTeleport{ 0L };
	clock_t lastVoidFall{ 0L };
	clock_t lastFigureUpdate{ 0L };

	Entity()
	{
		active = true;
		start = clock();
		UpdateImage();
	}
	virtual void Awake()
	{
		active = true;
		start = clock();
		lastMove = clock();
	}

	void Heal(double dhp, double dmaxhp = 0.0)
	{
		max_hp += dmaxhp;
		hp += dhp;
		if (hp > max_hp)
			hp = max_hp;
	}
	Entity* GetNearestEnemy() const;
	virtual void ForceCommandReset()
	{

	}
	virtual bool VeryForceMoving() const
	{
		return false;
	}
	virtual bool ForceMoving() const
	{
		return false;
	}
	virtual bool ForceStop() const
	{
		return false;
	}
	inline IQLevel GetIQ() const
	{
		return entity_iqs[id];
	}
	virtual double GetKnockbackResistance() const
	{	//抗击退
		return 0.0;
	}
	virtual Defense GetDefense() const
	{
		return GetTeamDefense(team);
	}
	inline bool AtOwnHome() const
	{
		return (team == RedTeam ? RedHome : BlueHome) == place;
	}
	void TurnToEntity(const Entity* pet2)
	{
		if (!pet2)
			return;

		if (Distance(x, y, pet2->x, pet2->y) > 150.0)
		{	//仅改动X
			if (pet2->x > x)
			{
				v.Reset(0.0, v());
				theta = v.Theta();
			}
			else {
				v.Reset(PI, v());
				theta = v.Theta();
			}
		}
		else {
			//直接面向
			theta = atan2(y - pet2->y, pet2->x - x);
			v.Reset(theta, v());
		}
	}
	virtual void InitData()
	{
		max_hp = hp = entity_hps.at(id);

		if (!pimg)
			UpdateImage();
	}
	inline TeamID GetOppo() const
	{
		return team == RedTeam ? BlueTeam : RedTeam;
	}
#define BED_DIG_RADIUS 250.0
#define BED_DIG_DELTA_Y 400.0
#define BED_DIG_DELTA_H 250.0
	bool NearOppoBed() const
	{	// 在敌方床附近？
		/*return Distance(x, y,
			point_beds[GetOppo()].x, point_beds[GetOppo()].y)
			<=
			BED_DIG_RADIUS;*/
		return abs(x - point_beds[GetOppo()].x) <= BED_DIG_RADIUS
			&& abs(y - point_beds[GetOppo()].y) <= BED_DIG_DELTA_Y
			&& abs(h) <= BED_DIG_DELTA_H;
	}
	virtual bool CanDigBed_Internal() const
	{
		return false;
	}
	virtual bool CanDigBed_External(const Bed& target_bed) const;

	bool CanDigBed(const Bed& target_bed) const
	{
		return CanDigBed_Internal() && CanDigBed_External(target_bed);
	}
	
	virtual void TryToDigOppoBed()
	{

	}
	void TurnToLeft()
	{
		ClearAction();
		v.Reset(PI, v());
		theta = PI;
	}
	void TurnToRight()
	{
		ClearAction();
		v.Reset(0.0, v());
		theta = 0.0;
	}
	void TurnToOppoBed()
	{
		v.Reset(point_beds[GetOppo()].x > x ? 0.0 : PI, v());
		theta = v.Theta();
	}
	virtual void ExtraBehaviours()
	{
			
	}
	void UpdateImage()
	{
		pimg = GetImage();
	}
	void UpdatePlace()
	{
		if (void_fall)
			return;	//保持地点位置不变

		if (x < BRIDGE_LEFT_LEFT)
		{
			place = RedHome;
		}
		else if (x > BRIDGE_RIGHT_RIGHT)
		{
			place = BlueHome;
		}
		else if (x >= point_centerisland_left.x && x <= point_centerisland_right.x)
		{
			place = CenterIsland;
		}
		else if (y >= point_bridge_left_left[0].y - 150
			&& y <= point_bridge_left_left[TEAM_BRIDGE_MAX_CNT-1].y + 150) 
		{
			bool on_left = x < uix / 2;
			int bridge_i = Clamp<int>((y - BRIDGE_TOP + 50) / BRIDGE_GAP, 0, TEAM_BRIDGE_MAX_CNT-1);
			place = Place((on_left ? 0 : 3) + RedBridgeA + bridge_i);
		}
		else {
			place = OtherPlace;
		}
	}
	void TurnToHome()
	{
		//先这么来
		if (team == RedTeam)
			v.Reset(PI, v());
		else
			v.Reset(0.0, v());
		theta = v.Theta();
	}
	void TurnToPoint(const POINT& pt)
	{
		if (pt.x > x)
		{
			v.Reset(0.0, v());
		}
		else {
			v.Reset(PI, v());
		}
	}
	bool TimeToMove() const
	{
		return (GetType() == ProjectileType
			|| (abs(clock() - lastMove) < 500)
			|| h > 10.0);
	}
	void LockMoveFor(clock_t milliseconds)
	{	//对非弹射物有效
		lastMove = clock() + milliseconds;
	}
	EntityType GetType() const
	{
		return entity_types.at(id);
	}
	virtual void ClearAction()
	{
		//just for class PlayerEntity
	}
	virtual void SetAction(PlayerAction act)
	{
		//just for class PlayerEntity
	}
	virtual void TurnAround()
	{	//转身
		ClearAction();
		v.Reset(OppoRadian(v.Theta()), v());
		theta = v.Theta();
		UpdateImage();
	}
	inline bool Still() const
	{	//是否静止
		return (v.IsZero());
	}
	inline bool StillStrict() const
	{	//是否静止
		return (v.IsZero() && fequ(v_h, 0.0) || !move);
	}
	inline void Stop()
	{
		ClearAction();
		if (h <= 5.0)
		{
			v.Clear();
		}
		move = false;
		//AddTextTip(x, y - 10, "停", GetStringColor(owner_username), 500L);
		lastMove = clock();
	}
	void Jump()
	{
		v_h = ET_JUMP_SPEED * GetJumpSpeedRate();
	}
	void Jump(double _v_h)
	{
		v_h = _v_h;
	}
	virtual double GetSpeedRate() const;
	virtual double GetJumpSpeedRate() const;
	void ResumeMoving()
	{
		SetAction(Moving);
		move = true;
		lastMove = clock();
		v.Reset(theta, entity_speeds.at(id) * GetSpeedRate());
		UpdateImage();
	}
	bool AtOppoHome() const
	{
		return team == RedTeam && place == BlueHome
			|| team == BlueTeam && place == RedHome;
	}
	Bed& GetOppoBed();
	virtual void UpdateActiveFacing();
	void UpdateRoute();
	virtual void CheckVoidFall();
	virtual void OnVoidFall()
	{
		void_fall = true;
		if (GetType() != ProjectileType)
			AddTextTip(x, y - 10, "落入虚空", DARKCYAN, 1000L);
		lastVoidFall = clock();
	}
#define VOID_FALL_TIME 700L
#define VOID_FALL_G 5.0
	inline bool VoidFalling() const
	{
		return (void_fall && clock() - lastVoidFall < VOID_FALL_TIME);
	}
	inline bool VoidFallen() const
	{
		return (void_fall && clock() - lastVoidFall >= VOID_FALL_TIME);
	}
	virtual void CheckDeath()
	{
		//掉进虚空
		if (VoidFallen())
		{
			active = false;
		}

		//出屏即死
		if (x < 0 || x >= uix || y < 0 || y >= uiy
			|| h < -100.0 
			|| TooHigh()
			)
		{
			OuttaScreenDeathMessage();
			active = false;
		}
			
	}
	inline bool TooHigh() const
	{
		return h > (GetType() == ProjectileType ? 1500.0 : 5000.0);
	}
	virtual void OuttaScreenDeathMessage()
	{
		if (entity_explosion_radii.find(id) != entity_explosion_radii.end())
		{	//可以爆炸的就爆炸
			MakeExplosion(x, y, entity_explosion_radii[id], id, owner_username,
				entity_melee_damages[id]);
		}
	}
	void UpdatePhysics()
	{
		constexpr double coeff = 0.1;
		constexpr double coeff_h = 0.5;
		if (TimeToMove())
		{
			//根据速度方向和大小进行位移
			x += v.x * coeff;
			y += v.y * coeff;
			lastMove = clock();
		}

		h += v_h * coeff_h;
		/*AddTextTip(x, y - 20, "h=" + str(h), InvertedColor(RainbowColor()), 200L, true);*/

		if (GetType() != ProjectileType && h < 0.0
			&& !VoidPos(place, x, y))
		{	//不允许埋进地下
			h = 0.0;
		}
	}
	void Update()
	{
		if (!active)
			return;

		if (clock() - lastFigureUpdate >= ET_FIGURE_UPDATE_CD)
		{	//定期刷新图片
			pimg = nullptr;
		}

		UpdatePhysics();
		UpdateSpeed();
		 
		CheckDeath();

		UpdateRoute();
		UpdatePlace();
		CheckVoidFall();
		UpdateBuffs();

		ExtraBehaviours();
	}
	void UpdateBuffs()
	{ 
		if (buffs.HasBuff(InstantHeal))
		{	//瞬间治疗 II
			AddTextTip(x, y - 10, "12", LIGHTGREEN, 2000L);
			Heal(12.0);
			buffs.RemoveBuff(InstantHeal);
		}
		if (buffs.HasBuff(Regeneration))
		{	//生命恢复
			if (clock() - buffs.buffs[Regeneration].lastAction >= 1250L)
			{
				AddTextTip(x, y - 10, "1", LIGHTGREEN, 2000L);
				Heal(1.0);
				buffs.buffs[Regeneration].lastAction = clock();
			}
		}
		if (buffs.HasBuff(Poisoned))
		{	//中毒
			if (clock() - buffs.buffs[Poisoned].lastAction >= 1250L)
			{
				DamageSource ds;
				ds.damage = 1.0;
				ds.owner_username = "";
				ds.team = OppoTeam(team);
				OnHurt(ds);
				buffs.buffs[Poisoned].lastAction = clock();
			}
		}
	}
	void UpdateSpeed()
	{
		if (GetType() != ProjectileType && move)
		{	//主观运动
			double ex_v = (GetSpeedRate() * entity_speeds[id]);
			double ex_v_x = ex_v * cos(theta);
			double ex_v_y = -ex_v * sin(theta);

			constexpr double _r = 6.0;

			if (h <= 5.0 && !void_fall)
			{	//地面上
				//迭代，缓缓向预期速度靠近
				if (abs(v.x - ex_v_x) < 5.0)
					v.x = ex_v_x;
				else
					v.x += (ex_v_x - v.x) / _r;

				if (abs(v.y - ex_v_y) < 5.0)
					v.y = ex_v_y;
				else
					v.y += (ex_v_y - v.y) / _r;

			}
		}
		
		if (NoGravityEntity(id))
		{	}
		else if (id == PrimedTNT && h <= 0.0)
			v_h = 0.0;
		else if (h <= 0.2 && !void_fall && GetType() != ProjectileType)
			v_h = 0.0;
		else
		{
			v_h -= VOID_FALL_G;
			/*AddTextTip(x, y - 20, "↓ "+str(v_h), RainbowColor(), 200L, true);*/
		}
	}
	void Hurt(double dhp)	//仅仅是受伤 不显示数字
	{
		hp -= dhp;
		if (max_hp > entity_hps[id])	//模拟的伤害吸收
		{
			max_hp -= dhp;
			if (max_hp < hp)
				max_hp = entity_hps[id];
		}

		HurtSound();

		if (hp <= 0.0)
		{
			DamageSource ds;
			ds.owner_username = "";
			ds.damage = dhp;
			hp = 0.0;
			OnDeath(ds);
		}
	}
#define DAMAGE_SHOW_DURATION 1000L
	virtual void OnHurt(DamageSource ds)
	{
		if (!active 
			|| GetType() == ProjectileType	//弹射物不会受伤
			)
			return;

		DefendDamage(ds.damage, GetDefense());	//扣除防御抵消的伤害

		// 伤害指示器
		AddTextTip(GenerateNormalRandom(x, 3.0), y - 80, sprintf2("%.0f", ceil(ds.damage)), ORANGE, DAMAGE_SHOW_DURATION);
		hp -= ds.damage;

		if (max_hp > entity_hps[id])	//模拟的伤害吸收
		{
			max_hp -= ds.damage;
			if (max_hp < hp)
				max_hp = entity_hps[id];
		}

		if (hp <= 0.0)
		{
			hp = 0.0;
			OnDeath(ds);
		}else
			HurtSound();

		LockMoveFor(300);
	}
	virtual void HurtSound()
	{
		if (entity_hurt_sounds.find(id) != entity_hurt_sounds.end())
			Sound(entity_hurt_sounds[id]);
	}
	static double ImpulseFormula(double damage)
	{	//冲量和伤害关系的公式（强行挂钩）
		return damage * 13.2;
	}
	virtual void SufferImpulse(double damage, double kb_theta, double kb_theta_h)
	{	//接受一个冲量
		double d = ImpulseFormula(damage);
		v.x += cos(kb_theta) * d;
		v.y -= sin(kb_theta) * d;
		//垂直方向击飞
		v_h += sin(kb_theta_h) * d;
	}
	virtual void OnDeath(DamageSource ds)
	{
		active = false;
		if (GetType() != ProjectileType
			&& !ds.owner_username.empty())
		{
			if (!IsGameOver())
			{
				IncUserStat(ds.owner_username, "kill_cnt");	//击杀者刷新统计
				if (GetType() != PlayerType)
					AddScore(ds.owner_username, ScoreItem::KillMob);
			}
		}
	}
	virtual double GetDamageAddition() const
	{
		return 0.0;
	}
	void SetPos(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
	void SetPos(double _x, double _y, double _theta)
	{
		x = _x;
		y = _y;
		theta = _theta;
		UpdateImage();
	}
	void TeleportTo(double _x, double _y)
	{	//传送
		SetPos(_x, _y);
		Sound(SndTeleport);
		AddTextTip(x, y - 10, "传送成功", GetStringColor(name));
		lastTeleport = clock();
	}
	virtual void TryToAttack()
	{

	}
	virtual bool IsBridging() const
	{
		return false;
	}
#define H_DRAW_COEFF 0.5
	virtual void Draw()
	{
		if (buffs.HasBuff(Invisibility))	//隐身 可以输入“位置”查看坐标
			return;

		if (!pimg)
		{
			pimg = GetImage();
			lastFigureUpdate = clock();
		}

		putimage_withalpha(nullptr, pimg, x - getwidth(pimg) / 2,
			y - getheight(pimg) / 2 - H_DRAW_COEFF * h);

		DrawHpBar();
#ifndef GAME_RELEASE
		/*DrawPlace();*/
		//DrawHeight();
#endif
		DrawName();
	}
	virtual void DrawName()
	{
		constexpr int fs = 25;
		setfont(fs, 0, "Calibri");
		setcolor(team);
		midprinty_shadow(owner_username, y - 15 - H_DRAW_COEFF * h, x);
	}
	void DrawHeight()
	{
		setfont(20, 0, "Calibri");
		setcolor(team);
		midprinty_shadow("h=" + str(h), y - 30 - H_DRAW_COEFF * h, x);
	}
	void DrawHpBar() const
	{
		constexpr int bar_w = 90;
		constexpr int bar_h = 8;
		constexpr color_t bar_color = HPBAR_COLOR;
		constexpr color_t bar_color2 = HPBAR_COLOR2;
		constexpr color_t bar_outline_color = RED;
		constexpr color_t bar_color_lost = BLACK;

		double rate = hp / max_hp;
		double _y = y + 30.0 - H_DRAW_COEFF * h;

		setfillcolor(bar_color);
		bar(x - bar_w / 2, _y, x - bar_w / 2 + rate * bar_w, _y + bar_h);
		setfillcolor(bar_color2);
		bar(x - bar_w / 2, _y + bar_h * 0.6, x - bar_w / 2 + rate * bar_w, _y + bar_h);
		setfillcolor(bar_color_lost);
		bar(x - bar_w / 2 + rate * bar_w, _y, x - bar_w / 2 + rate * bar_w + (1.0 - rate) * bar_w, _y + bar_h);
		setcolor(bar_outline_color);
		rectangle(x - bar_w / 2, _y, x + bar_w / 2, _y + bar_h);
	}
	void DrawPlace()
	{
		constexpr int fs = 24;
		setfont(fs, 0, DEF_FONTNAME);
		setcolor(RainbowColor());
		midprinty_shadow(place_names.at(place), y + 10 - H_DRAW_COEFF * h, x);

		/*if (OnBridge(place))
		{
			midprinty_shadow(sprintf2("%.2f", GetNowBridgePosRate(x)), y-5, x);
		}*/
		if (void_fall)
			midprinty_shadow("坠落中", y - 60 - H_DRAW_COEFF * h, x);
	}
	virtual PIMAGE GetImage()
	{
		string name = "entity_" + str(id);
		if (variant)
		{
			name += "_" + str(variant);
		}

		size_t index = GetFrameIndex();
		if (index)
		{
			name += str(char('a' + index));
		}

		if (FacingLeft())
			name += "left.png";
		else
			name += ".png";
		return imgMgr.images[name];
	}
	virtual size_t GetFrameIndex() const
	{
		return 0U;
	}
	inline bool FacingLeft() const
	{
		return LeftTheta(theta);
	}
	inline bool FacingRight() const
	{
		return RightTheta(theta);
	}
};

#define ET_NEARBY_DISTANCE 200.0
#define ET_MELEE_RANGE 90.0
#define ET_MELEE_RANGE_H 180.0
#define ET_MELEE_CD 1000L

#define ET_DEF_SHOOT_CD 4000L
#define ET_DEF_SHOOT_SPEED 60.0
#define ET_RANGER_VIEW 600.0

#define PL_VARIANT_CNT 6

bool NearbyExistEnemy(TeamID team, double x, double y, double r);
void CreateProjectile(EntityID eid, TeamID team, string_view owner_username, double v, double v_h, double x, double y, double h, double theta, double proj_damage);

vector<string> void_fall_death_msg_suffixes
{	//落虚空死因后缀
	"拥抱了虚空娘！",
	"一失足成千古恨",
	"没站稳！",
	"复刻了星之卡比。",
};

enum PickaxeID
{
	BareHand		= 0,
	WoodenPickaxe,
	StonePickaxe,
	IronPickaxe,
	GoldenPickaxe,
	DiamondPickaxe,
};

const vector<string> pickaxe_names
{
	"徒手",
	"木镐",
	"石镐",
	"铁镐",
	"金镐",
	"钻石镐",
};

const vector<long> pickaxe_prices
{	//镐子价格
	0L,
	30L,
	30L,
	60L,
	100L,
	200L,
};

const vector<double> pickaxe_damages
{	//镐子伤害
	3.0,
	5.0,
	8.0,
	15.0,
	20.0,
	30.0,
};

enum SwordID
{
	Hand = 0,
	WoodenSword,
	StoneSword,
	IronSword,
	DiamondSword,
	NetheriteSword,
};

const vector<string> sword_names
{
	"徒手",
	"木剑",
	"石剑",
	"铁剑",
	"钻石剑",
	"下界合金剑",
};

const vector<long> sword_prices
{	//武器价格
	0L,
	1L,
	30L,
	70L,
	120L,
	300L,
};

const vector<double> sword_damages
{	//武器伤害
	1.0,
	4.0,
	5.0,
	6.0,
	7.0,
	8.0,
};

const vector<double> hasteness_dig_cd_rates
{	// 疯狂矿工 挖掘CD系数
	1.0,
	0.9,
	0.8,
};
const vector<double> sharpness_damage_addition
{	// 锋利附魔 伤害增加值
	0.0,
	0.5,
	1.0,
	1.5,
};

enum ArmorID
{	//装备
	NoArmor			= 0,
	LeatherArmor,
	ChainmailArmor,
	IronArmor,
	DiamondArmor,
	NetheriteArmor,
};

const vector<string> armor_names
{
	"无护甲",
	"皮革护甲",
	"锁链护甲",
	"铁护甲",
	"钻石护甲",
	"下界合金护甲",
};

const vector<string> armor_filename_prefixes
{
	"",
	"leather_armor",
	"chainmail_armor",
	"iron_armor",
	"diamond_armor",
	"netherite_armor",
};

const vector<long> armor_prices
{
	0L,		//默认无护甲！
	50L,
	80L,
	120L,
	300L,
	2000L,
};

const vector<Defense> armor_defense
{	//装备防御值
	0,
	3,	//7,
	5,	//12,
	7,	//15,
	10,	//20,
	12,	//20,
};

void OnPlayerDeath(string_view pname);

class PlayerEntity : public Entity
{	//玩家实体
public:
	PlayerAction action{Defending};
	PickaxeID	pick{ BareHand };		//默认徒手
	SwordID	    sword{ WoodenSword };	//默认木剑
	ArmorID     armor{ NoArmor };		//默认无装备

	ForceCmd	command{ ForceCmd::Cancelled };	//玩家の命令

	double melee_damage{ 0.0 };		//近战
	double ranger_damage{ 0.0 };	//远程

	EntityID proj_id{ Arrow };
	double proj_speed{ ET_DEF_SHOOT_SPEED };
	double proj_damage{ 1.0 };

	clock_t melee_cd{ 1000L };
	clock_t dig_cd  { 1000L };
	clock_t shoot_cd{ 3000L };

	clock_t lastMelee{ 0L };
	clock_t lastDig  { 0L };
	clock_t lastShoot{ 0L };
	clock_t lastForceCmd{ 0L };		//上一次发出运动命令的时间戳

	void InitData() override
	{
		Entity::InitData();

		UpdateDamage();
		UpdatePickaxe();
	}
	void ExtraBehaviours() override
	{
		
	}
	void SetForceCommand(ForceCmd _cmd)
	{	//设置命令
		command = _cmd;
		if (command != ForceCmd::Cancelled)
			lastForceCmd = clock();
	}

#define FORCE_CMD_DURATION	30000L
#define FORCE_CMD_VERY_DURATION 5000L

	bool IsForceCmdValid() const
	{
		return command != ForceCmd::Cancelled 
			&& clock() - lastForceCmd <= FORCE_CMD_DURATION;
	}
	bool ForceThis(ForceCmd cmd) const
	{
		return command == cmd
			&& clock() - lastForceCmd <= FORCE_CMD_DURATION;
	}
	void ForceCommandReset() override
	{
		command = ForceCmd::Cancelled;
	}
	bool VeryForceMoving() const override
	{
		if (!IsForceCmdValid())
			return false;
		return (clock() - lastForceCmd < FORCE_CMD_VERY_DURATION) && (command == ForceCmd::Home || command == ForceCmd::Charge);
	}
	bool ForceMoving() const override
	{
		if (!IsForceCmdValid())
			return false;
		return command == ForceCmd::Home || command == ForceCmd::Charge;
	}
	bool ForceStop() const override
	{
		return ForceThis(ForceCmd::Stop);
	}
	bool ForceGood(DIR	dir) const
	{	//该方向不妨碍命令执行的运动方向吗
		if (!IsForceCmdValid())
			return true;
		if (command == ForceCmd::Home)
			return (dir == RIGHT && team == BlueTeam
				|| dir == LEFT && team == RedTeam);
		if (command == ForceCmd::Charge)
			return (dir == LEFT && team == BlueTeam
				|| dir == RIGHT && team == RedTeam);
		if (command == ForceCmd::Stop)
		{
			return false;
		}
		return false;
	}
	void OnVoidFall() override
	{	//PlayerEntity::
		chat.AddChat(name + Choice(void_fall_death_msg_suffixes), team);
		OnPlayerDeath(name);
		if (!IsGameOver())
			IncUserStat(name, "void_fall_cnt");
		Entity::OnVoidFall();
	}
	void ClearAction() override
	{
		action = Defending;
	}
	void SetAction(PlayerAction act) override
	{
		action = act;
	}
	void UpdateActiveFacing() override;
	void OuttaScreenDeathMessage() override
	{
		chat.AddChat(name + " 掉出了这个世界", team);
		OnPlayerDeath(name);
		Sound(SndOrb);
	}
	double GetKnockbackResistance() const override
	{	//抗击退
		if (armor == NetheriteArmor)
			return 0.4;	//下界合金护甲 40%
		return 0.0;
	}
	Defense GetDefense() const override
	{
		return GetTeamDefense(team) + armor_defense[armor];
	}
	inline bool TimeToMelee() const
	{
		return (clock() - lastMelee >= melee_cd);
	}
	inline bool TimeToDig() const
	{
		return (clock() - lastDig >= dig_cd * GetDigCDRate());
	}
	inline bool TimeToShoot() const
	{
		return (clock() - lastShoot >= shoot_cd);
	}
	bool CanMelee() const
	{
		return true;
	}
	bool CanShoot() const
	{
		return false;
	}
	double GetDamageAddition() const override;
	inline double GetMeleeDamage() const
	{	// 近战伤害
		return sword_damages[sword] + GetDamageAddition();
	}
	inline double GetDigDamage() const
	{	// 挖掘伤害
		return pickaxe_damages[pick];
	}
	inline double GetDigCDRate() const;
	bool IsBridging() const override
	{
		return action == Bridging;
	}
	void CheckVoidFall() override
	{
		if (action == Bridging)
			return;

		Entity::CheckVoidFall();
	}
	bool CanDigBed_Internal() const override
	{
		return true;
	}
	void AfterMelee()
	{
		//chat.AddChat(name + "砍了一刀！", team);
		LockMoveFor(melee_cd);
	}
	void UpdatePickaxe();
	void UpdateDamage()
	{

		//以后由武器决定
	}
	void DrawName() override
	{
		constexpr int fs = 28;
		setfont(fs, 0, DEF_FONTNAME);
		setwcolor(team);
		midprinty_shadow(name, y - H_DRAW_COEFF * h - 50, x);
	}
	void DrawArmor()
	{
		if (armor == NoArmor
			|| buffs.HasBuff(Invisibility))
			return;

		string imgname = armor_filename_prefixes[armor];

		if (variant >= 4)
			imgname += "_b";
		if (LeftTheta(theta))
			imgname += "left";
		imgname += ".png";

		constexpr int delta_x = 40;

		int _x, _y;
		_x = x + (LeftTheta(theta) ? -1 : 1) * delta_x;
		_y = y - H_DRAW_COEFF * h;

		if (variant >= 4)
		{
			if (LeftTheta(theta))
			{
				_x += 7;
				_y -= 49;
			}
			else
			{
				_x -= 75;
				_y -= 68;
			}
		}
		else {
			if (LeftTheta(theta))
			{
				_x += 6;
				_y -= 57;
			}
			else
			{
				_x -= 66;
				_y -= 67;
			}
		}

		putimage_withalpha(nullptr, imgMgr.images[imgname],
			_x, _y);
	}
	void DrawHandItem()
	{
		string imgname{ "" };
		if (action == Digging && pick != BareHand)
		{
			imgname = "pickaxe_" + str(int(pick));
			if (LeftTheta(theta))
				imgname += "left";
			imgname += ".png";
		}
		else if (sword != Hand) 
		{
			imgname = "weapon_" + str(int(sword));
			if (LeftTheta(theta))
				imgname += "left";
			imgname += ".png";
		} 
		int _x = x + (LeftTheta(theta) ? -30 : 5);
		int _y = y + 8 - H_DRAW_COEFF * h;
		putimage_withalpha(nullptr, imgMgr.images[imgname],
			_x, _y);
	}
	void Draw() override
	{
		/*if (!active)
			return;*/

		Entity::Draw();
		DrawArmor();
		DrawHandItem();
	}
	PIMAGE GetImage() override
	{
		if (FacingRight())
			return imgMgr.images["player_" + str(variant) + ".png"];
		return imgMgr.images["player_" + str(variant) + "left.png"];
	}
	void TryToDigOppoBed() override;
	void TryToAttack() override;
	void OnDeath(DamageSource ds) override
	{	//PlayerEntity::
		active = false;

		if (IsGameOver())
			return;

		chat.AddChat(name + "死了。", team);
		Sound(SndHurt);
		if (!ds.owner_username.empty())
		{
			IncUserStat(ds.owner_username, "kill_cnt");
			IncUserStat(ds.owner_username, "kill_player_cnt");
			AddScore(ds.owner_username, ScoreItem::KillPlayer);
		}
		IncUserStat(name, "death_cnt");
		OnPlayerDeath(name);
	}
	void ShootThere(double x2, double y2)
	{
		double theta1 = atan2(y - y2, x2 - x);
		double proj_v_h;
		double proj_v_p = proj_speed;

		if (NoGravityEntity(proj_id))
			proj_v_h = 0.0;
		else
			CalcParabolaSpeed(Distance(x, y, x2, y2), proj_v_p, proj_v_h);

		CreateProjectile(proj_id, team, name, proj_v_p, proj_v_h, x, y, h + 20.0,
			theta1, proj_damage);
		lastShoot = clock();
	}
	void ShootTarget(Entity* pet)
	{
		TurnToEntity(pet);
		ShootThere(pet->x, pet->y);
	}
};

void SpawnPlayerEntity(string_view name, TeamID team, int variant, double x, double y);
PlayerEntity* GetPlayerEntity(string_view name);

inline bool SecondImageMeleeEntity(EntityID eid)
{
	return eid == 3;
}

class MeleeEntity : public virtual Entity
{	//近战敌怪
public:

	double melee_damage{ 1.0 };
	clock_t lastContact{ 0L };

	string owner_username;

	void InitData() override
	{
		Entity::InitData();

		if (entity_melee_damages.find(id) != entity_melee_damages.end())
			melee_damage = entity_melee_damages.at(id);
	}
	size_t GetFrameIndex() const override
	{
		if (SecondImageMeleeEntity(id))
		{
			if (NearbyExistEnemy(team, x, y, ET_NEARBY_DISTANCE))
				return 1U;
			return 0U;
		}
		return 0U;
	}
	bool CanDigBed_Internal() const override
	{
		return true;
	}
	void TryToDigOppoBed() override;
	
	virtual void AfterContact()
	{
		if (entity_explosion_radii.find(id) != entity_explosion_radii.end())
		{	//防止爆炸怪连爆 qwq
			active = false;
		}
		LockMoveFor(ET_MELEE_CD);
	}
	void TryToAttack() override;
	
	inline bool TimeToContact() const
	{
		return (clock() - lastContact >= ET_MELEE_CD);
	}
};

map<EntityID, EntityID> entity_proj_ids
{
	{PlayerAvatar, Arrow},
	{Skeleton, Arrow},
	{Pillager, Arrow},
	{Blaze, SmallFireball},
};
map<EntityID, double> entity_proj_speeds
{	//远程敌怪射击速度
	{PlayerAvatar, 100.0},
	{Skeleton,	100.0},
	{Pillager,	110.5},
	{Blaze,		90.0},
};
map<EntityID, clock_t> entity_shoot_cds
{
	{PlayerAvatar, 2000L},
	{Skeleton, 2000L},
	{Pillager, 3000L},
	{Blaze, 3000L},
};

//map<EntityID, double> proj_ranges
//{
//	{Arrow, 500.0},
//	{SmallFireball, 1000.0},
//	{Fireball, 1920.0},
//	{EnderPearl, 900.0},
//};

inline bool SecondImageRangerEntity(EntityID eid)
{
	return eid == 4;
}

#define ET_PROJ_SPEED_COEFF 5.0

class RangerEntity : public virtual Entity
{		//远程敌怪
public:

	EntityID proj_id;
	double proj_speed{ ET_DEF_SHOOT_SPEED };
	double proj_damage{ 1.0 };

	clock_t shoot_cd{ ET_DEF_SHOOT_CD };
	clock_t lastShoot{ 0L };

	string owner_username;

	void InitData() override
	{
		if (entity_proj_ids.find(id) != entity_proj_ids.end())
			proj_id = entity_proj_ids[id];

		if (entity_proj_speeds.find(id) != entity_proj_speeds.end())
			proj_speed = entity_proj_speeds[id] * ET_PROJ_SPEED_COEFF;

		if (entity_shoot_cds.find(proj_id) != entity_shoot_cds.end())
			shoot_cd = entity_shoot_cds[id];

		Entity::InitData();

		if (entity_ranger_damages.find(id) != entity_ranger_damages.end())
			proj_damage = entity_ranger_damages[id];
	}
	size_t GetFrameIndex() const override
	{
		if (SecondImageRangerEntity(id) && clock() - lastShoot > shoot_cd - 2000L)
			return 1U;
		return 0U;
	}
	bool CanDigBed_Internal() const override
	{
		return true;
	}
	inline bool TimeToShoot() const
	{
		return (clock() - lastShoot >= shoot_cd);
	}
	void TryToAttack() override;
	void TryToDigOppoBed() override;

#define ET_RANGER_STILL_DIST 50.0
	void ExtraBehaviours() override
	{
		auto ptr = GetNearestEnemy();
		if (ptr && Distance(ptr->x, ptr->y, x, y) < ET_RANGER_STILL_DIST)
		{	//太近了，射手停下
			Stop();
		}
		else {
			ResumeMoving();
		}

		Entity::ExtraBehaviours();
	}

	void ShootThere(double x2, double y2)
	{
		double theta1 = atan2(y - y2, x2 - x);
		double proj_v_h;
		double proj_v_p = proj_speed;

		if (fequ(proj_v_p, 0.0))
		{
			chat.AddChat("proj v=0", PINK);
		}

		if (NoGravityEntity(id))
			proj_v_h = 0.0;	//无重力
		else
		{
			//根据抛物线计算垂直方向应有的速度
			CalcParabolaSpeed(Distance(x, y, x2, y2), proj_v_p, proj_v_h);
		}

		//AddTextTip(x, y - 10, "v_p=" + str(proj_v_p), RainbowColor(), 1500L);

		//创建射弹
		CreateProjectile(proj_id, team, owner_username, proj_v_p, proj_v_h, x, y, h+20.0,
			theta1, proj_damage);

		lastShoot = clock();
	}
	void ShootTarget(Entity* pet)
	{
		TurnToEntity(pet);
		LockMoveFor(1500L);
		ShootThere(pet->x, pet->y);
	}
};

class MeleeRangerEntity : public MeleeEntity, public RangerEntity
{
public:
	void InitData() override
	{
		MeleeEntity::InitData();
		RangerEntity::InitData();
	}

	size_t GetFrameIndex() const override
	{
		//<!> WIP
		return 0U;
	}
	bool CanDigBed_Internal() const override
	{
		return true;
	}
	void TryToAttack() override;
	void TryToDigOppoBed() override;
};

template <typename TEntity, typename T2 = TEntity>
shared_ptr<Entity> SpawnEntity(EntityID eid, TeamID team, string_view whose, double x, double y, double theta, double v, double h = 0.0);
bool VoidPos(Place place, double x, double y);

#define ET_PROJ_CONTACT_R 90.0
#define ET_PROJ_CONTACT_R_H 120.0
#define ET_PROJ_RANGE	550.0

#define TNT_FUSE_TIME 3000L

class Projectile : public Entity
{	//弹射物
public:
	//double range{ET_PROJ_RANGE};
	POINT throw_point{ 0, 0 };	//起掷点

	double proj_damage{ 1.0 };

	string owner_username;		//发射者名称

	void Awake() override
	{
		if (id == PrimedTNT)
		{
			Sound(SndFuse);
		}
		else if (id == Arrow)
		{
			Sound(SndBowShoot);
		}
		else if (id == Fireball)
		{
			Sound(SndFireball);
		}
		Entity::Awake();
	}
	void CheckDeath() override
	{
		if (active
			//&& Distance(x, y, throw_point.x, throw_point.y) > range
			)
		{
			if (id == PrimedTNT && clock() - start >= TNT_FUSE_TIME)
			{
				//TNT 到时间后爆炸
		//		chat.AddChat("BOOM!", team);
				MakeExplosion(x, y, entity_explosion_radii[id], id, owner_username, proj_damage);
				active = false;
			}
			else if (id != PrimedTNT && HitGround())
			{	
				//其他弹射物击中地面
				//chat.AddChat(entity_names[id] + " 被销毁！", team);
				OnRangeOver();
				active = false;
			}
			else if (clock() - start > 30000)
			{	//不允许超过 30 秒
				OnRangeOver();
				active = false;
			}
		}
		Entity::CheckDeath();
	}
	bool HitGround() const
	{
		return !VoidPos(place, x, y) && h <= 5.0;
	}
	void OnRangeOver();

	//原图片统一朝右
	void Draw() override
	{	//弹射物の绘制
		if (!pimg)
		{
			pimg = GetImage();
			lastFigureUpdate = clock();
			if (!pimg)
				return;
		}

		/*putimage_withalpha(nullptr, pimg, x - getwidth(pimg) / 2,
			y - getheight(pimg) / 2);*/

		float radian = theta;	//待测试
		if (NonRotativeProjectile(id))	//不旋转
			radian = 0.0;

		putimage_rotate(nullptr, pimg,
			x - getwidth(pimg) / 2, y - getheight(pimg) / 2 - H_DRAW_COEFF * h,
			0.5f, 0.5f, radian, true);
#ifndef GAME_RELEASE
		//DrawPlace();
		//DrawHeight();
#endif
	}
#define TNT_BLINK_FRAME_DURATION 250L
	size_t GetFrameIndex() const override
	{
		if (id == PrimedTNT)
		{	//点燃的TNT的闪烁效果
			return (clock() / TNT_BLINK_FRAME_DURATION % 2);
		}
		return 0U;
	}

	void TryToAttack() override;
};