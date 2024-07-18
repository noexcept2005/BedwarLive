#pragma once

map<string, size_t> globalstats;

const vector<string> globalstats_keys
{
	"game_cnt",
};

void InitGlobalsStatKeys()
{
	for (const auto& k : globalstats_keys)
	{
		globalstats[k] = 0L;
	}
}

#define GLOBALSTATS_PATH L"./global/stats.csv"
void ReadGlobalStats()
{
	vector<string> lines = ReadFileLines(GLOBALSTATS_PATH);

	//No such file
	if (lines.empty())
	{
		InitGlobalsStatKeys();
		return;
	}

	for (const auto& line : lines)
	{
		vector<string> cut = CutLine(line, '=');
		if (cut.size() < 2)
			continue;
		string key = cut[0];
		string value = cut[1];
		globalstats[key] = ToSize_t(value);
	}
}
void SaveGlobalStats()
{
	if (!ExistFile(L"./global"))
		_wmkdir(L"./global");
	fstream fout(GLOBALSTATS_PATH, ios::out);

	//Open file failed
	if (!fout || fout.fail())
	{
		MessageBoxA(nullptr, "Cannot Save Global Stats!", "SaveGlobalStats", 
			MB_SYSTEMMODAL | MB_ICONERROR);
		return;
	}

	for (auto& pr : globalstats)
	{
		fout << pr.first << "=" << pr.second << '\n';
	}

	fout.close();
}
