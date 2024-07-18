#pragma once


const vector<string> chat_illegal_chars
{	//奇葩的 Unicode 控制字符全枪毙掉
	"\u2000", "\u2001","\u2002","\u2003","\u2004","\u2005","\u2006","\u2007","\u2008","\u2009","\u200A","\u200B","\u200C","\u200D","\u200E","\u200F",
	"\u2028", "\u2029","\u202A","\u202B","\u202C","\u202D","\u202E","\u202F",
	"\u205F", "\u2060",
	"\u2066", "\u2067", "\u2068", "\u2069", "\u206A", "\u206B", "\u206C", "\u206D", "\u206E", "\u206F",
};
bool FilteredChat(const string& s)
{	//弹幕指令过滤器
	for (int i = 0; i < s.length(); ++i)
	{
		if (s.at(i) == '%' && i != s.length() - 1 &&
			isalnum(s.at(i + 1)))
			return true;	//稍微松一点
	}
	for (const auto& ic : chat_illegal_chars)
		if (s.find(ic.c_str()) != string::npos)
			return true;

	//if (s.find('%') != string::npos)
	//	return true;
	return false;
}

map<string, string> chat_replace	//其他说法自动替换
{
	{"重来",		"下一把"},
	{"吃金苹果",	"金苹果"},
	{"喝治疗",	"治疗药水"},
	{"治疗",		"治疗药水"},
	{"回血",		"治疗药水"},
	{"heal",	"治疗药水"},
	{"珍珠",		"末影珍珠"},
	{"pearl",	"末影珍珠"},
	{"火球",		"烈焰弹"},
	{"扔火球",	"烈焰弹"},
	{"fireball","烈焰弹"},
	{"见钻石",	"捡钻石"},
	{"钻石",		"捡钻石"},
	{"diamond", "捡钻石"},
	{"拿钻石",	"捡钻石"},
	{"拣钻石",	"捡钻石"},
	{"往右走",	"向右"},
	{"往右",		"向右"},
	{"往东",		"向右"},
	{"往东走",	"向右"},
	{"right",	"向右"},
	{"east",	"向右"},
	{"往左走",	"向左"},
	{"往左",		"向左"},
	{"往西",		"向左"},
	{"往西走",	"向左"},
	{"left",	"向左"},
	{"west",	"向左"},
	{"home",	"回家"},
	{"归去",		"回家"},
	{"归",		"回家"},
	{"回",		"回家"},
	{"家",		"回家"},
	{"调头",		"回头"},
	{"stop",	"停住"},
	{"不要动",	"停住"},
	{"走",		"继续走"},
	{"go",		"继续走"},
	{"走啊",		"继续走"},
	{"走路",		"继续走"},
	{"往红家走",		"去红家"},
	{"往蓝家走",		"去蓝家"},
	{"往红队走",		"去红家"},
	{"往蓝队走",		"去蓝家"},
	{"去红",		"去红家"},
	{"去蓝",		"去蓝家"},
	{"打红",		"去红家"},
	{"打蓝",		"去蓝家"},
	{"攻红",		"去红家"},
	{"攻蓝",		"去蓝家"},
	{"去对面",	"冲"},
	{"打对面",	"冲"},
	{"往对面走",	"冲"},
	{"过去",		"冲"},
	{"进攻",		"冲"},
	{"respawn",	"复活"},
	{"隐身",		"隐身药水"},
	{"隐形",		"隐身药水"},
	{"隐形药水",		"隐身药水"},
	{"迅捷药水",	"速度药水"},
	{"跳跃提升药水",	"跳跃药水"},
	{"速度",	"速度药水"},
	{"迅捷",	"速度药水"},
	{"跳跃提升",	"跳跃药水"},
	{"力量",	"力量药水"},
	{"喝力量",	"力量药水"},
	{"喝速度",	"速度药水"},
	{"喝跳跃",	"跳跃药水"},
	{"喝隐身",	"隐身药水"},
	{"炸弹",		"TNT"},
	{"扔炸弹",	"TNT"},
	{"放TNT",	"TNT"},
	{"炸",		"TNT"},
	{"爆破",		"TNT炸床"},
	{"起飞",		"TNT跳"},
	{"珍珠高抛",	"珍珠往高处扔"},
	{"珍珠扔对面",	"珍珠突袭"},
	{"镐子升级",	"升级镐子"},
	{"升级镐",	"升级镐子"},
	{"钻剑",		"钻石剑"},
	{"合金剑",	"下界合金剑"},
	{"钻甲",		"钻石护甲"},
	{"皮甲",		"皮革护甲"},
	{"合金甲",	"下界合金护甲"},
	{"皮革盔甲",	"皮革护甲"},
	{"锁链盔甲",	"锁链护甲"},
	{"铁盔甲",	"铁护甲"},
	{"钻石盔甲",	"钻石护甲"},
	{"下界合金盔甲",	"下界合金护甲"},
	{"hp",			"血量"},
	{"还剩多少血",	"血量"},
	{"pos",	"位置"},
	{"height",	"高度"},
	{"防爆玻璃",	"玻璃"},
	{"help",	"帮助"},
};

void RunChatCommand(string s, const string& username)
{
	if (s.empty())
		return;

	if (FilteredChat(s))
	{	//含敏感词汇的弹幕
		chat.AddChat(username + ">> [FILTERED]", GetStringColor(username));
		return;
	}

	bool in_team = bw.players.find(username) != bw.players.end();

	if (s == "帮助" || s == "查看帮助")
	{
		chat.AddChat("语法：帮助/查看帮助+关键词  例如：查看帮助末影珍珠", GOLD);
		return;
	}else if (s.length() >= 6 && sequ(strhead(s, 6), "加入"))
	{
		bool res{ false };
		TeamID team;
		if (sequ(s, "加入"))
		{
			if (!bw.started)
			{
				for (auto const& name : bw.random_team_players)
				{
					if (name == username)
					{	//已在随机队伍中
						Sound(SndBurp);
						return;
					}
				}
				if (ExistPlayer(username))
					RemovePlayer(username);

				bw.random_team_players.push_back(username);
				Sound(SndEnter);
				return;
			}
			team = Choice({ RedTeam, BlueTeam });
			res = true;
		}else
			team = ParseTeamID(strxhead(s, 6), &res);
		if (res)
		{
			if (ExistPlayer(username))
				RemovePlayer(username);
			if (!bw.started)
			{
				bw.random_team_players.erase(
					remove(bw.random_team_players.begin(), bw.random_team_players.end(),
							username), 
					bw.random_team_players.end());
			}
			AddPlayer(username, team);
			Sound(SndEnter);
		}
		else
		{
			chat.AddChat("不存在该队伍！", ORANGE);
		}
	}
	else if (!bw.started && (sequ(s, "退出") || sequ(s, "不玩了") || sequ(s, "取消") || sequ(s, "离开") || sequ(s, "退出队伍")))
	{
		if (ExistPlayer(username))
			RemovePlayer(username);
		bw.random_team_players.erase(
			remove(bw.random_team_players.begin(), bw.random_team_players.end(),
				username),
			bw.random_team_players.end());
	}
	else if (sequ(s, "选红队") || sequ(s, "红") || sequ(s, "红队") || sequ(s, "red"))
	{
		if (ExistPlayer(username))
			RemovePlayer(username);
		if (!bw.started)
		{
			bw.random_team_players.erase(
				remove(bw.random_team_players.begin(), bw.random_team_players.end(),
					username),
				bw.random_team_players.end());
		}
		AddPlayer(username, RedTeam);
	}
	else if (sequ(s, "选蓝队") || sequ(s, "蓝") || sequ(s, "蓝队") || sequ(s, "blue"))
	{
		if (ExistPlayer(username))
			RemovePlayer(username);
		if (!bw.started)
		{
			bw.random_team_players.erase(
				remove(bw.random_team_players.begin(), bw.random_team_players.end(),
					username),
				bw.random_team_players.end());
		}
		AddPlayer(username, BlueTeam);
	}
	if (s.length() > 6 && s.substr(0, 6) == "帮助" ||
		s.length() > 12 && s.substr(0, 12) == "查看帮助")
	{
		string kw;
		if (s.substr(0, 6) == "帮助")
			kw = strxhead(s, 6);
		else
			kw = strxhead(s, 12);
		ApproachKeyword(kw);
		PrintHelp(kw);
	}
	else if (sequ(s, "蹲搭次数") || sequ(s, "蹲起搭次数")
		|| sequ(s, "月步次数") || sequ(s, "神桥次数"))
	{
		chat.AddChat(sprintf2("%s %s：%zu", username.data(), s.data(),
			user_stats[str(username)].stat[bridging_statmap[
				ParseBridgingMethod(strxtail(s, 6))
			]]), GetStringColor(username));
		return;
	}

	if (!bw.started)
		return;

	//////////////////////////////////////////////////////////////////////////////////////
	bw.stats.chat_cnt++;

	string info;
	info = username + ">>  " + s;
	//info = username + ">>  " + s + " | head4=\"" + (s.length() > 4 ? strhead(s, 4):" too short "s) + "\" len=" + str(s.length());

	chat.AddChat(info, GetStringColor(username));
	cout << info << '\n';

	if (chat_replace.find(s) != chat_replace.end())
		s = chat_replace[s];

	if (bw.over
		&& (s == "下一把" || s == "继续"))
	{
		chat.AddChat("进入下一把！", ORANGE);
		g.retry = true;
		g.playing = false;
		return;
	}

	{
		int i = 0;
		for (const auto& bname : block_names)
		{
			if (sequ(bname, s))
			{	//Purchase Blocks
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买方块！", ORANGE);
					return;
				}
				AddDefense(bw.players[username].team, username, BlockType(i));
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : bridging_names)
		{	//搭路方式
			if (s.length() >= name.length()
				&& sequ(s.substr(0, name.length()), name))
			{
				//匹配！
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能搭路", ORANGE);
					return;
				}
				auto ptr = GetPlayerEntity(username);
				if (!ptr)
				{
					chat.AddChat(username + " 已死，无法搭路！", ORANGE);
					return;
				}

				//不够熟练，无法使用的搭路方式
				if (i > Sneaking && !CanPlayerBridgeThisWay(username, BridgingMethod(i)))
				{
					chat.AddChat(username + " 目前还掌握不了" + bridging_names[i]
						+ "，他对" + bridging_names[i - 1] + "还不够熟练！",
						ORANGE);
					return;
				}
				//获得最适合搭路的桥的下标
				int index = ParseBridgeIndex(ptr->x > uix / 2 ? BlueTeam : RedTeam, s, name,
					true);
				if (-1 == index)
					return;
				ChooseBridge(username, BridgingMethod(i), index);
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : item_names)
		{
			if (sequ(name, s))
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能使用物品！", ORANGE);
					return;
				}
				UseItem(bw.players[username].team, username, ItemID(i), 1);
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : team_upgrade_names)
		{
			if (name == s)
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能进行团队升级！", ORANGE);
					return;
				}
				//团队升级
				TeamUpgrade(bw.players[username].team, TeamUpgradeID(i));
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : armor_names)
		{
			if (name == s)
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买护甲！", ORANGE);
					return;
				}
				PurchaseArmor(username, ArmorID(i));
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : sword_names)
		{
			if (name == s)
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买武器！", ORANGE);
					return;
				}
				PurchaseSword(username, SwordID(i));
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& name : entity_names)
		{
			if (entity_types.at(i) == ProjectileType)
			{	//都说了弹射物不卖
				++i;
				continue;
			}

			if (s.length() >= name.length()
				&& sequ(strhead(s, name.length()), name))
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买军队！", ORANGE);
					return;
				}
				int index = ParseBridgeIndex(bw.players[username].team, s, name);
				/*if (-1 == index)
					return;*/
				SummonArmy(bw.players[username].team, username, EntityID(i), index);
				return;
			}
			++i;
		}
	}

	if (sequ(s, "基本信息"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
		{
			chat.AddChat(sprintf2("%s %s active=%d x=%.0f y=%.0f h=%.0f",
				username.data(), place_names[ptr->place].c_str(),
				ptr->active,
				ptr->x, ptr->y, ptr->h	
			),
				ptr->team);

			chat.AddChat(sprintf2("vx=%.1f vy=%.1f vh=%.1f move=%d fall=%d ",
				ptr->v.x, ptr->v.y, ptr->v_h, ptr->move, ptr->void_fall
			),
				ptr->team);

			chat.AddChat(sprintf2("hp=%.1f max_hp=%.1f",
				ptr->hp, ptr->max_hp
			),
				ptr->team);
		}
	}
	else if (sequ(s, "位置") || sequ(s, "查看位置"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s %s x=%.0f y=%.0f h=%.0f", username.data(), place_names[ptr->place].c_str(), ptr->x, ptr->y, ptr->h), 
				ptr->team);
	}
	else if (sequ(s, "速率") || sequ(s, "查看速率") || sequ(s, "查看速度"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s vx=%.1f vy=%.1f vh=%.1f still=%d stillstrict=%d", username.data(), ptr->v.x, ptr->v.y, ptr->v_h, 
				ptr->Still(), ptr->StillStrict()),
				ptr->team);
	}
	else if (sequ(s, "高度") || sequ(s, "查看高度"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s h=%.0f", username.data(), ptr->h),
				ptr->team);
	}
	else if (sequ(s, "x"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s x=%.0f", username.data(), ptr->x),
				ptr->team);
	}
	else if (sequ(s, "y"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s y=%.0f", username.data(), ptr->y),
				ptr->team);
	}
	else if (sequ(s, "血量") || sequ(s, "查看血量"))
	{
		auto ptr = GetPlayerEntity(username);
		if (ptr)
			chat.AddChat(sprintf2("%s: %.1f / %.1f", username.c_str(), ptr->hp, ptr->max_hp), ptr->team);
	}
	else if (sequ(s, "分数") || sequ(s, "查看分数"))
	{
		chat.AddChat(sprintf2("%s 分数为：%zu", username.data(),
			user_stats[str(username)].stat["score"]), GetStringColor(username));
	}
	else if (s == "退出" || s.length() > 6 && sequ(strhead(s, 6), "退出"))
	{
		RemovePlayer(username);
	}
	else if (s == "复活" || s == "重生")
	{
		if (!in_team)
		{
			chat.AddChat(username + "还没有加入队伍，无法重生！", LIGHTRED);
			return;
		}
		if (nullptr != GetPlayerEntity(username))
		{
			chat.AddChat(username + "还活着！", CYAN);
			return;
		}
		if (!bw.teams[bw.players[username].team].CanRespawn())
		{
			chat.AddChat(username + "无法复活，因为" + team_names[bw.players[username].team] + "床已被破坏！");
			return;
		}

		bw.players[username].Spawn();
		auto ptr = GetPlayerEntity(username);
		if (ptr)
		{
			AddTextTip(ptr->x, ptr->y - 10, "已重生！", CYAN);
		}
		else {
			chat.AddChat(username + "复活失败！", bw.players[username].team);
		}
	}
	else if (sequ(s, "升级镐子"))
	{
		if (!in_team)
		{
			chat.AddChat(username + "还没有加入队伍，无法升级！", LIGHTRED);
			return;
		}
		if (bw.players[username].UpgradePickaxe(bw.teams[bw.players[username].team].exp))
		{
			chat.AddChat(username + "已升级到" + pickaxe_names[bw.players[username].pickaxe]);
		}
	}
	/*else if (sequ(s, "升级武器") || sequ(s, "升级剑"))
	{
		if (!in_team)
		{
			chat.AddChat(username + "还没有加入队伍，无法升级！", LIGHTRED);
			return;
		}
		if (bw.players[username].UpgradeSword(bw.teams[bw.players[username].team].exp))
		{
			chat.AddChat(username + "已升级到" + sword_names[bw.players[username].sword]);
		}
	}*/
	else if (sequ(s, "末影珍珠往高处扔") || sequ(s, "珍珠往高处扔")
		|| sequ(s, "末影珍珠平抛") || sequ(s, "珍珠平抛")
		|| sequ(s, "末影珍珠回城") || sequ(s, "珍珠回城")
		|| sequ(s, "末影珍珠突袭") || sequ(s, "珍珠突袭"))
	{	//末影珍珠使用方法
		
		if (!in_team)
		{
			chat.AddChat(username + " 还没有加入队伍，不能使用末影珍珠！", ORANGE);
			return;
		}
		int param = USEITEM_THROW_MEDIUM;
		if (sequ(s, "末影珍珠往高处扔") || sequ(s, "珍珠往高处扔"))
			param = USEITEM_THROW_STRONG;
		else if (sequ(s, "末影珍珠平抛") || sequ(s, "珍珠平抛"))
			param = USEITEM_THROW_WEAK;
		else if (sequ(s, "末影珍珠回城") || sequ(s, "珍珠回城"))
			param = USEITEM_THROW_HOME;
		else if (sequ(s, "末影珍珠突袭") || sequ(s, "珍珠回城"))
			param = USEITEM_THROW_OPPO;
		UseItem(bw.players[username].team, username, EnderPearlItem, param);
		return;
	}
	else if (sequ(s, "炸床") || sequ(s, "TNT炸床") || sequ(s, "TNT向左跳") || sequ(s, "TNT向右跳")
		|| sequ(s, "TNT跳") || sequ(s, "TNT回跳"))
	{
		if (!in_team)
		{
			chat.AddChat(username + " 还没有加入队伍，不能使用TNT！", ORANGE);
			return;
		}
		int param = 1;
		if (sequ(s, "炸床") || sequ(s, "TNT炸床"))
			param = USEITEM_TNT_BLOW_BED;
		else if (sequ(s, "TNT向左跳"))
			param = USEITEM_TNT_JUMP_LEFT;
		else if (sequ(s, "TNT向右跳"))
			param = USEITEM_TNT_JUMP_RIGHT;

		if (sequ(s, "TNT跳") || sequ(s, "TNT回跳"))
		{
			auto ptr = GetPlayerEntity(username);
			if (!ptr)
			{
				chat.AddChat("玩家" + username + "并不存在！", bw.players[username].team);
				return;
			}
			if (sequ(s, "TNT跳"))
				param = LeftTheta(ptr->theta) ? USEITEM_TNT_JUMP_LEFT : USEITEM_TNT_JUMP_RIGHT;
			else
				param = LeftTheta(ptr->theta) ? USEITEM_TNT_JUMP_RIGHT : USEITEM_TNT_JUMP_LEFT;
		}
			
		UseItem(bw.players[username].team, username, TNT, param);

		return;
	}
	else if (sequ(s, "火球跳") || sequ(s, "火球回跳"))
	{
		if (!in_team)
		{
			chat.AddChat(username + " 还没有加入队伍，不能使用烈焰弹！", ORANGE);
			return;
		}
		int param = 1;
		if (sequ(s, "火球跳"))
			param = USEITEM_FIREBALL_JUMP;
		else if (sequ(s, "火球回跳"))
			param = USEITEM_FIREBALL_JUMP_BACK;

		UseItem(bw.players[username].team, username, FireballItem, param);
	}
	else if (s.length() >= 4 && sequ(s.substr(0, 4), "搭路"))
	{
		if (!in_team)
		{
			chat.AddChat(username + "还没有加入队伍，无法搭路！", LIGHTRED);
			return;
		}
		//默认选择蹲起搭
		auto ptr = GetPlayerEntity(username);
		if (!ptr)
		{
			chat.AddChat(username + " 已死，无法搭路！", ORANGE);
			return;
		}
		int index = ParseBridgeIndex(ptr->x > uix / 2 ? BlueTeam : RedTeam, s, username, true);
		if (-1 == index)
			return;
		ChooseBridge(username, NinjaBridging, index);
	}
	else if (sequ(s, "回家") || sequ(s, "返回") || sequ(s, "回城")
		|| sequ(s, "转身") || sequ(s, "掉头") || sequ(s, "回去")
		|| sequ(s, "回头")
		|| sequ(s, "冲")
		|| sequ(s, "停住") || sequ(s, "站住") || sequ(s, "停下") || sequ(s, "停")
		|| sequ(s, "起步") || sequ(s, "继续走")
		|| sequ(s, "朝左") || sequ(s, "向左")
		|| sequ(s, "朝右") || sequ(s, "向右")
		|| sequ(s, "去红家") || sequ(s, "去蓝家")
		|| sequ(s, "去红队") || sequ(s, "去蓝队")
		)
	{
		if (!in_team)
		{
			chat.AddChat(username + " 还没有加入任何队伍！", YELLOW);
			return;
		}
		auto ptr = GetPlayerEntity(username);
		if (ptr)
		{
			if (ptr->action == Bridging)
			{
				if (sequ(s, "起步") || sequ(s, "继续走"))
				{
					AddTextTip(ptr->x, ptr->y - 10, "已经在搭路了！", CYAN, 2000L);
					return;
				}
				chat.AddChat("中断" + team_names[bw.players[string(username)].team] + "的搭桥操作", RED);
				//bw.teams[bw.players[string(name)].team].bridges[bw.teams[bw.players[string(name)].team].building_bridge];
				bw.teams[bw.players[string(username)].team].InterruptBridging(username);
				//Interrupt Bridging
			}
			ptr->ResumeMoving();
			if (sequ(s, "回家") || sequ(s, "返回") || sequ(s, "回城"))
			{
				ptr->SetForceCommand(ForceCmd::Home);
				ptr->TurnToHome();
			}
			else if (sequ(s, "冲"))
			{
				ptr->SetForceCommand(ForceCmd::Charge);
				ptr->TurnToOppoBed();
			}
			else if (sequ(s, "转身") || sequ(s, "掉头") || sequ(s, "回去")
				|| sequ(s, "回头"))
			{
				ForceCmd _cmd;
				if (LeftTheta(ptr->theta))
				{
					_cmd = ptr->team == RedTeam ? ForceCmd::Charge : ForceCmd::Home;
				}
				else {
					_cmd = ptr->team == BlueTeam ? ForceCmd::Charge : ForceCmd::Home;
				}

				ptr->SetForceCommand(_cmd);
				ptr->TurnAround();
			}
			else if (sequ(s, "朝左") || sequ(s, "向左") || sequ(s, "去红家") || sequ(s, "去红队"))
			{
				ptr->SetForceCommand(ptr->team == BlueTeam ? ForceCmd::Charge : ForceCmd::Home);
				ptr->TurnToLeft();
			}
			else if (sequ(s, "朝右") || sequ(s, "向右") || sequ(s, "去蓝家") || sequ(s, "去蓝队"))
			{
				ptr->SetForceCommand(ptr->team == RedTeam ? ForceCmd::Charge : ForceCmd::Home);
				ptr->TurnToRight();
			}
			else if (sequ(s, "停住") || sequ(s, "站住") || sequ(s, "停下") || sequ(s, "停"))
			{
				ptr->SetForceCommand(ForceCmd::Stop);
				ptr->Stop();
			}
			else if (sequ(s, "起步") || sequ(s, "继续走"))
			{
				ForceCmd _cmd;
				if (RightTheta(ptr->theta))
				{
					_cmd = ptr->team == RedTeam ? ForceCmd::Charge : ForceCmd::Home;
				}
				else {
					_cmd = ptr->team == BlueTeam ? ForceCmd::Charge : ForceCmd::Home;
				}

				ptr->SetForceCommand(_cmd);
				ptr->ResumeMoving();
			}

			AddTextTip(ptr->x, ptr->y - 50, s, ORANGE, 3000L);
		}
		else {

		}
	}
	else if (s.length() > 6 && sequ(strhead(s, 6), "购买"))
	{
		int i{ 0 };
		for (const auto& name : item_names)
		{
			if (sequ(name, strxhead(s, 6)))
			{
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买物品！", ORANGE);
					return;
				}
				PurchaseItem(bw.players[username].team, ItemID(i));
				return;
			}
			++i;
		}

		i = 0;
		for (const auto& bname : block_names)
		{
			if (i > 0 && sequ(bname, strxhead(s, 6)))
			{	//Purchase Blocks
				if (!in_team)
				{
					chat.AddChat(username + " 还没有加入队伍，不能购买方块！", ORANGE);
					return;
				}
				AddDefense(bw.players[username].team, username, BlockType(i));
				return;
			}
			++i;
		}
	}
	else if (s == "捡钻石" || s == "收集钻石")
	{
		auto pet = GetPlayerEntity(username);
		if (!in_team || !pet)
		{
			chat.AddChat("玩家" + username + "并不在场，无法捡钻石！", bw.players[username].team);
			return;
		}

		if (pet->action == Bridging)
		{

			AddTextTip(pet->x, pet->y - 30,
				"忙着搭路", LIGHTRED, 2000L);
			return;
		}
		if (bw.teams[bw.players[username].team].NeedBridge())
		{
			if (bw.teams[bw.players[username].team].IsBridging())
			{
				chat.AddChat(team_names[bw.players[username].team] + "正在搭桥中，无法捡钻石！", bw.players[username].team);
				return;
			}
			chat.AddChat(team_names[bw.players[username].team] + "还没有搭桥，无法捡钻石！", bw.players[username].team);
			return;
		}
		auto ptr = GetPlayerEntity(username);
		if (!ptr)
		{
			chat.AddChat(username + "还没有复活！", bw.players[username].team);
			return;
		}
		if (ptr->place != CenterIsland)
		{
			AddTextTip(ptr->x, ptr->y - 10, "离得太远了！", ROYALBLUE, 3000L);
			return;
		}

		int index{ -1 };
		if (bw.diamond_cnt[0] > 0)
			index = 0;
		else if (bw.diamond_cnt[1] > 0)
			index = 1;
		else
			return;

		CollectDiamond(bw.players[username].team, index);
	}
	else if (s == "捡绿宝石" || s == "收集绿宝石")
	{
		chat.AddChat("绿宝石只能亲自去捡哦", bw.players[username].team);
	}
	//管理员命令
	else if (AdminName(username))
	{
		bool res{ false };
		TeamID team = ParseTeamID(s, &res);
		if (res)
		{
			bw.teams[team].ShowList();
		}
		else if (s == "我要经验")
		{
			Sound(SndLevelup);
			AddTextTip(bw.players[username].team == RedTeam ? 220 : 1800,
				400, "经验 UP", YELLOW, 3000L);
			bw.teams[bw.players[username].team].exp = 9990L;
		}
		else if (s == "我要钻石")
		{
			Sound(SndIcechime);
			AddTextTip(bw.players[username].team == RedTeam ? 220 : 1800,
				330, "钻石 UP", AQUA, 3000L);
			bw.teams[bw.players[username].team].diamonds = 990L;
		}
		else if (s == "敌人防御")
		{
			AddDefense(bw.players[username].team == RedTeam ? BlueTeam : RedTeam, username,
				Endstone);
			AddDefense(bw.players[username].team == RedTeam ? BlueTeam : RedTeam, username,
				Plank);
			AddDefense(bw.players[username].team == RedTeam ? BlueTeam : RedTeam, username,
				Wool);
		}
		else if (s == "清空弹射物" || s == "清空射弹")
		{
			bw.entities.erase(
				remove_if(bw.entities.begin(), bw.entities.end(),
					[](auto& that)
			{
				return that->GetType() == ProjectileType;
			}
				),
				bw.entities.end());
			chat.AddChat("已清空弹射物", GOLD);
		}
		else if (s == "瞬间回城")
		{
			Sound(SndIcechime);
			auto ptr = GetPlayerEntity(username);
			if (ptr)
			{
				ptr->SetPos(point_beds[ptr->team].x, point_beds[ptr->team].y);
				AddTextTip(ptr->x, ptr->y - 5, "回城成功", CYAN, 1000L);
			}
		}
		else if (s == "更换皮肤")
		{
			Sound(SndIcechime);
			auto ptr = GetPlayerEntity(username);
			if (ptr)
			{
				ptr->variant = RandomRange(0, PL_VARIANT_CNT, true, false);
			}
		}
		else if (s == "拆桥")
		{
			for (auto& b : bw.teams[bw.players[username].team].bridges)
				b.Destroy();
			Sound(SndToast);
			AddTextTip(bw.players[username].team == RedTeam ? 500 : 1400,
				360, "桥梁已拆除", GOLD, 3000L);
			//chat.AddChat("桥梁已拆除", bw.players[username].team);
		}
		else if (s == "重启游戏")
		{
			Sound(SndPling);
			chat.AddChat("收到管理员命令，游戏即将重启！", RED);
			Sleep(3000);
			g.retry = true;
			g.playing = false;
		}
		else if (s == "关闭游戏")
		{
			Sound(SndPling);
			chat.AddChat("收到管理员命令，游戏即将退出！", RED);
			Sleep(3000);
			g.retry = false;
			g.playing = false;
		}
	}
}