#pragma once

#define TEXTTIP_DURATION 2000L

class TextTip
{
public:
	int x;
	int y;

	string text{"提示"};
	color_t color{GOLD};

	clock_t duration{ TEXTTIP_DURATION };
	clock_t start{ 0L };

	static constexpr int fs = 25;
	static constexpr const char* fontname = DEF_FONTNAME;

	double GetRate() const
	{
		return Clamp((clock() - start) / double(duration), 0.0, 1.0);
	}
	bool IsActive() const
	{
		return clock() >= start && clock() - start <= duration;
	}

	void Draw() const
	{
		if (!IsActive())
			return;

		double rate = GetRate();
		setfont(fs, 0, fontname);
		setwcolor(color);
		midprinty_stroke(text, y - rate * 50, x);
	}
};

vector<TextTip> text_tips;
map<string, clock_t> tip_timestamps;

void AddTextTip(int x, int y, string_view s, color_t color, clock_t duration = TEXTTIP_DURATION, bool allow_spam = false)
{
	//同一句话不能频繁出现
	if (!allow_spam
		&& tip_timestamps.find(str(s)) != tip_timestamps.end()
		&& clock() - tip_timestamps[str(s)] < 3000L)
		return;

	TextTip tip;
	tip.x = x;
	tip.y = y;
	tip.text = str(s);
	tip.color = color;
	tip.duration = duration;
	tip.start = clock();

	tip_timestamps[str(s)] = clock();

	text_tips.push_back(tip);
}
void DrawTextTips()
{
	for (const auto& tip : text_tips)
		tip.Draw();
}
