#pragma once

vector<string> user_stat_keys
{
	"score",

	"death_cnt",
	"win_cnt",
	"kill_cnt",
	"kill_player_cnt",
	"bed_destructor_cnt",
	"void_fall_cnt",

	"sneaking_cnt",
	"ninjabridging_cnt",
	"moonwalk_cnt",
	"godbridging_cnt",

	"ender_pearl_cnt",
};

//掌握下一门搭路技巧前至少要试过上一门的数量
#define NINJA_AVAILABLE_BASE_CNT		5U
#define MOONWALK_AVAILABLE_BASE_CNT		32U
#define GODBRIDGE_AVAILABLE_BASE_CNT	64U

const vector<string> bridging_cnt_keys
{
	"sneaking_cnt",
	"ninjabridging_cnt",
	"moonwalk_cnt",
	"godbridging_cnt",
};

void InitUserStatsKeys()
{
	sort(user_stat_keys.begin(), user_stat_keys.end());
}

class UserStat	//用户数据，将会存在数据库中
{
public:
	string username;

	map<string, size_t> stat;

	UserStat()
	{
		Init();
	}

	void Init()
	{
		for (const auto& key : user_stat_keys)
		{
			SetValue(key, 0U);
		}
	}
	void SetValue(string_view key, size_t value)
	{
		stat[str(key)] = value;
	}
	string GetDataRow() const
	{
		string row;
		int i = 0;
		row += username;
		row += ',';
		for (auto const& pr : stat)
		{
			row += str(pr.second);
			++i;
			if (i != stat.size())
				row += ",";
		}
		return row;
	}
};

map<string, UserStat> user_stats;

template<typename T = size_t>
struct PlayerSortStruct {
	string name;
	T value;

	bool operator < (const PlayerSortStruct& p2) const
	{
		return value < p2.value;
	}
	bool operator > (const PlayerSortStruct& p2) const
	{
		return value > p2.value;
	}
};

void UpdateRankScore()
{
	vector<PlayerSortStruct<size_t>> a;
	for (auto& users : user_stats)
	{
		a.push_back({ users.first, users.second.stat["score"] });
	}
	sort(a.begin(), a.end(), greater<>());

	g.rank_score.clear();
	g.rank_score.resize(a.size());
	for (int i = 0; i < a.size(); ++i)
	{
		g.rank_score.at(i) = a.at(i).name;
	}
}
void UpdateRanks()
{
	UpdateRankScore();
}

size_t& GetUserStat(string_view name, string_view key)
{
	if (user_stats.find(str(name)) == user_stats.end())
	{
		user_stats.insert(make_pair(name, UserStat()));
		user_stats[str(name)].username = name;
	}
	return user_stats[str(name)].stat[str(key)];
}
void IncUserStat(string_view name, string_view key)
{
	if (user_stats.find(str(name)) == user_stats.end())
	{
		user_stats.insert(make_pair(name, UserStat()));
		user_stats[str(name)].username = name;
	}
	user_stats[str(name)].stat[str(key)]++;
}

#define USER_STATS_FILENAME L"./user/stats.csv"

void ReadUserStats()
{
	if (!ExistFile(USER_STATS_FILENAME))
		return;

	vector<string> lines = ReadFileLines(USER_STATS_FILENAME);
	if (lines.empty())
		return;
	string firstrow = lines.at(0);
	vector<string> firstrow_cut = CutLine(firstrow, ',');
	for (int u = 1; u < lines.size(); ++u)
	{
		vector<string> line_cut = CutLine(lines.at(u), ',');
		if (line_cut.size() < firstrow_cut.size())
			continue;

		UserStat us;
		us.username = line_cut.at(0);
		for (int i = 1; i < firstrow_cut.size(); ++i)
		{
			us.stat[firstrow_cut.at(i)] = ToSize_t(line_cut.at(i));
		}
		user_stats.insert(make_pair(us.username, us));
	}
	
}
void SaveUserStats()
{
	fstream fout(USER_STATS_FILENAME, ios::out);
	int i = 0;
	fout << "username,";
	for (const auto& _ : user_stat_keys)
	{
		fout << _;
		++i;
		if (i != user_stat_keys.size())
			fout << ',';
	}
	fout << '\n';
	for (const auto& profile : user_stats)
	{
		fout << profile.second.GetDataRow() << '\n';
	}
	fout.close();
}

enum FallReason			//坠落原因
{
	MeleeKnockback,		//近战击退
	ProjKnockback,		//弹射物击退
	ExplosionKnockback,	//爆炸击退
};

double UserFallRateCoeff(string_view pname, FallReason reason)
{	//击退掉进虚空概率的系数，针对某一用户而言，掉的次数越多概率越低
	if (user_stats.find(str(pname)) == user_stats.end())	//新人
		return 1.0;

	constexpr size_t _min_cnt = 20U;		//门槛，大于等于这个值时才开始生效
	constexpr double _min = 1.0, 
					 _max = 0.75;			//顶级玩家有低达 3/4 的系数
	constexpr double _max_fall_cnt = 1000;	//如果大于等于500次，孩子你无敌了

	//低于门槛值时，与新手一个概率
	if (user_stats[str(pname)].stat["void_fall_cnt"] < _min_cnt)
		return 1.0;

	return Lerp(_min, _max,
		Clamp(user_stats[str(pname)].stat["void_fall_cnt"] 
		/
		double(_max_fall_cnt), 
			0.0, 1.0));
}


enum class ScoreItem
{
	Nope,
	Win,
	DestroyBed,
	KillMob,
	KillPlayer,
	Bridge,
	CollectDiamond,
	CollectEmerald,
};

vector<size_t> score_item_values
{
	0L,
	100L,
	500L,
	10L,
	30L,
	5L,
	1L,
	1L,
};

vector<string> score_item_names
{
	"愣着不动",
	"胜利",
	"摧毁敌床",
	"击杀敌怪",
	"击杀敌人",
	"搭桥",
	"收集钻石",
	"收集绿宝石",
};

void AddScore(string_view who, ScoreItem reason);