#pragma once

enum BuffID
{
	NoBuff		= 0,
	Speed,
	JumpBoost,			//就当是bw中的跳跃5了
	Regeneration,
	Poisoned,
	Strength,
	Invisibility,
	InstantHeal,
};
vector<string> buff_names
{
	"无效果",
	"迅捷",
	"跳跃提升",
	"生命恢复",
	"中毒",
	"力量",
	"隐身",
	"瞬间治疗",
};

#define BUFF_SPEED_RATE 1.2
#define BUFF_JUMP_BOOST_RATE 2.5
#define BUFF_STRENGTH_DAMAGE_ADDITION	3.0
#define BUFF_MAX_CNT 6

class Buff			//增益/减益
{
public:

	BuffID id;
	clock_t duration{ 0L };

	clock_t lastAction{ 0L };	//上一次触发buff效果的时间戳
	clock_t start{ 0L };

	inline bool IsActive() const
	{
		return duration > 0L && (clock() - start <= duration);
	}
	clock_t RemainTime() const
	{
		return Clamp(duration - (clock() - start), 0L, duration);
	}
	void Clear()
	{
		duration = 0L;
	}
};

class BuffAttribute
{
public:
	array<Buff, BUFF_MAX_CNT> buffs;

	BuffAttribute()
	{
		Clear();
	}

	void Clear()
	{
		for (auto& b : buffs)
			b.Clear();
	}
	inline bool HasBuff(BuffID bid) const
	{
		return buffs[bid].IsActive();
	}
	void AddBuff(BuffID bid, clock_t duration)
	{
		buffs[bid].duration = duration;
		buffs[bid].start = clock();
	}
	void RemoveBuff(BuffID bid)
	{
		buffs[bid].Clear();
	}
};
