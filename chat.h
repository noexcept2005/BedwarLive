#pragma once

#define CHAT_FONTNAME "Minecraft AE Pixel"
#define CHAT_SHOW_TIME 60000L
#define CHAT_FS 24
#define CHAT_V_ADD 28
#define CHAT_SHOW_MAX_CNT 7U

struct ChatItem
{
	string text;
	color_t color;

	clock_t start;

	inline bool IsShown(clock_t duration) const
	{
		return (!text.empty() && clock() - start <= duration);
	}
};

class Chat		//聊天栏
{
public:

	vector<ChatItem> items;

	static constexpr clock_t chat_single_show_time = CHAT_SHOW_TIME;
	static constexpr size_t chat_capacity = 36;
	static constexpr int chat_fs = CHAT_FS;
	static constexpr int chat_v_addition = CHAT_V_ADD;

	clock_t lastChat;

	void Clear()
	{
		items.clear();
	}
	void AddChat(const string& text, color_t color = EGERGB(255, 255, 255))
	{
		if (text.empty())
			return;

		ChatItem ci;
		ci.text = text;
		ci.color = color;
		ci.start = clock();
		items.emplace_back(ci);

		Update();
		//MessageBeep(MB_ICONEXCLAMATION);

		lastChat = clock();
	}
	void Update()
	{
		while (items.size() > chat_capacity)
			items.erase(items.begin());
	}
	void Display() const
	{
		constexpr int chat_left = 1050;
		constexpr int chat_bottom_gap = 50;
		static const int chat_width = uix - chat_left;

		int chat_bottom = uiy - chat_bottom_gap;
		if (items.empty())
			return;

		auto _cnt = min(CHAT_SHOW_MAX_CNT, items.size());
		setfont(chat_fs, 0, CHAT_FONTNAME);
		for (int i = 0; i < _cnt; ++i)
		{	//从下往上绘制
			//if (items.at(items.size() - i - 1).IsShown(chat_single_show_time))
			{
				setcolor(WaveColor(items.at(items.size() - i - 1).color));
				xyprintf_shadow(chat_left, chat_bottom - chat_v_addition * (i + 1),
					//chat_width, chat_fs, 
					items.at(items.size() - i - 1).text.c_str());
			}
			//else break;
		}
	}
}chat;
