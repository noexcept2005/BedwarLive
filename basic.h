#pragma once
#define fequ(f1,f2) (abs(f1-f2)<0.1)

int CJZAPI RandomRange(int Min = 0, int Max = 32767, bool rchMin = true, bool rchMax = true)
{ //随机数值区间 
	auto a = rand();
	if (rchMin && rchMax)	//[a,b]
		return (a % (Max - Min + 1)) + Min;
	else if (rchMin && !rchMax)		//[a,b)
		return (a % (Max - Min)) + Min;
	else if (!rchMin && rchMax)		//(a,b]
		return (a % (Max - Min)) + Min + 1;
	else							//(a,b)
		return (a % (Max - Min - 1)) + Min + 1;
}
double CJZAPI RandomRangeDouble(double _min, double _max,	//min,max
	bool rchMin = true, bool rchMax = true,	//开/闭 
	UINT uPrec = 2	//精度（位数） 
)
{	//随机小数区间 
	double p_small = pow(10.0f, -int(uPrec));
	int p_big = (int)pow(10, uPrec);
	int l = int(p_big * _min);
	int r = int(p_big * _max);
	int res;
	res = RandomRange(l, r, rchMin, rchMax);
	return (p_small * (double)res);
}
template<typename _T>
inline _T CJZAPI Choice(initializer_list<_T> choices) {
	vector<_T> vec(choices);
	return vec[RandomRange(0, vec.size()-1, true, true)];
}
template<typename _T>
inline _T CJZAPI Choice(const vector<_T>& choices_vector) {
	return choices_vector[RandomRange(0, choices_vector.size()-1, true, true)];
}

template<typename _T>
inline bool CJZAPI Percent(_T prob) {
	return (RandomRange(0, 100, true, false) < prob);
}
template<typename _T>
inline bool CJZAPI Permille(_T prob) {	//千分数
	return (RandomRange(0, 1000, true, false) < prob);
}
inline bool CJZAPI Decision(float prob)
{
	return Percent(prob * 100.0f);
}
inline bool CJZAPI FractionProb(auto denominator, int numerator = 1)
{
#ifndef GAME_RELEASE
	frac = 100.0 * numerator / double(denominator);
#endif
	return Percent(100.0 * numerator / double(denominator));
}

template <typename _T>
_T CJZAPI Clamp(_T val, _T min = 0, _T max = 2147483647)	//限定范围 
{
	if (val < min) val = min;
	else if (val > max) val = max;
	return val;
}
template <typename _T>
_T CJZAPI ClampA(_T& val, _T min = 0, _T max = 2147483647)	//限定范围 
{
	if (val < min) val = min;
	else if (val > max) val = max;
	return val;
}
double GenerateNormalRandom(double mean, double stddev)
{	//正态分布随机数
	random_device rd; // 使用随机设备作为种子
	mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
	normal_distribution<double> dist(mean, stddev); // 创建正态分布对象

	return Clamp(dist(gen), 0.0, 1000000.0); // 生成并返回随机数
}
template <typename _T>
inline double CJZAPI Lerp(_T startValue, _T endValue, double rat)
{
	return startValue + (endValue - startValue) * rat;
}
inline double CJZAPI EaseInExpo(double _x)
{
	return fequ(_x, 0.0f) ? 0.0 : pow(2.0, 10.0 * _x - 10.0);
}
inline double CJZAPI EaseInOutSine(double _x)
{	//retval,_x ∈ [0,1]
	return -(cos(PI * _x) - 1) / 2;
}
inline double CJZAPI EaseInOutBack(double _x)
{
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;
	return _x < 0.5
		? (pow(2 * _x, 2) * ((c2 + 1) * 2 * _x - c2)) / 2
		: (pow(2 * _x - 2, 2) * ((c2 + 1) * (_x * 2 - 2) + c2) + 2) / 2;
}
inline double CJZAPI EaseOutCubic(double _x)
{
	return 1 - pow(1 - _x, 3);
}
inline double CJZAPI EaseOutExpo(double _x)
{
	return fequ(_x, 1.0) ? 1.0 : 1.0 - pow(2.0, -10.0 * _x);
}
inline double CJZAPI EaseInOutElastic(double _x)
{
	const double c5 = (2 * PI) / 4.5;
	return fequ(_x, 0.0)
		? 0.0
		: fequ(_x, 1.0)
		? 1.0
		: _x < 0.5
		? -(pow(2, 20 * _x - 10) * sin((20 * _x - 11.125) * c5)) / 2
		: (pow(2, -20 * _x + 10) * sin((20 * _x - 11.125) * c5)) / 2 + 1;
}
inline double CJZAPI EaseOutBounce(double _x)
{
	const double n1 = 7.5625;
	const double d1 = 2.75;

	if (_x < 1 / d1) {
		return n1 * _x * _x;
	}
	else if (_x < 2 / d1) {
		return n1 * (_x -= 1.5 / d1) * _x + 0.75;
	}
	else if (_x < 2.5 / d1) {
		return n1 * (_x -= 2.25 / d1) * _x + 0.9375;
	}
	else {
		return n1 * (_x -= 2.625 / d1) * _x + 0.984375;
	}
}
inline double CJZAPI EaseInOutBounce(double _x)
{
	return _x < 0.5
		? (1 - EaseOutBounce(1 - 2 * _x)) / 2
		: (1 + EaseOutBounce(2 * _x - 1)) / 2;
}
inline double CJZAPI EaseInOutExpo(double _x)
{
	return fequ(_x, 0.0)
		? 0.0
		: fequ(_x, 1.0)
		? 1.0
		: _x < 0.5 ? pow(2, 20 * _x - 10) / 2
		: (2 - pow(2, -20 * _x + 10)) / 2;
}

//C++20
template <typename T>
concept NoWString = requires (T a)
{
	requires !same_as<T, wstring>;
};
template <typename T>
concept NoString = requires (T a)
{
	requires !same_as<T, string>;
};

template <NoString _T1, typename _T2>
_T2 CJZAPI Convert(const _T1& src)
{
	stringstream ss;
	ss << src;
	_T2 ret;
	ss >> ret;
	return ret;
}
template <NoWString _T1, typename _T2>
_T2 CJZAPI Convert(const _T1& src)
{
	stringstream ss;
	ss << src;
	_T2 ret;
	ss >> ret;
	return ret;
}
template <typename _T>
inline string CJZAPI str(_T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}
template <typename _ET>
inline string CJZAPI str(const vector<_ET>& _list)
{
	stringstream ss;
	ss << '[';
	int i{ 0 };
	for (const auto& s : _list)
	{
		ss << s;
		if (i != _list.size() - 1)
			ss << ",";
		++i;
	}
	ss << ']';
	return ss.str();
}
#define ToString str

template <NoString _T>
int CJZAPI ToInt(_T value)
{
	wstringstream ss;
	ss << value;
	int ret;
	ss >> ret;
	return ret;
}
template <NoWString _T>
int CJZAPI ToInt(_T value)
{
	stringstream ss;
	ss << value;
	int ret;
	ss >> ret;
	return ret;
}

template <NoString _T>
size_t CJZAPI ToSize_t(_T value)
{
	wstringstream ss;
	ss << value;
	size_t ret;
	ss >> ret;
	return ret;
}
template <NoWString _T>
size_t CJZAPI ToSize_t(_T value)
{
	stringstream ss;
	ss << value;
	size_t ret;
	ss >> ret;
	return ret;
}

template <typename _T>
float CJZAPI ToFloat(_T value)
{
	stringstream ss;
	ss << value;
	float ret;
	ss >> ret;
	return ret;
}

template <typename _T>
double CJZAPI ToDouble(_T value)
{
	stringstream ss;
	ss << value;
	double ret;
	ss >> ret;
	return ret;
}
string CJZAPI sprintf2(const char* szFormat, ...)
{
	va_list _list;
	va_start(_list, szFormat);
	char szBuffer[1025]{ "\0" };
	_vsnprintf(szBuffer, 1024, szFormat, _list);
	va_end(_list);
	return string{ szBuffer };
}
string CJZAPI TimeString(clock_t sec)
{
	size_t h = sec / 3600;
	size_t m = sec % 3600 / 60;
	size_t s = sec % 60;
	return sprintf2("%02zu:%02zu:%02zu", h, m, s);
}

// wstring=>string
string WString2String(wstring_view ws)
{
	string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.data();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char* chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	string strResult = chDest;
	delete[] chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

// string => wstring
wstring String2WString(string_view s)
{
	string strLocale{ setlocale(LC_ALL, "") };
	const char* chSrc = s.data();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	wstring wstrResult = wchDest;
	delete[] wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}
//from https://blog.csdn.net/qq_30386941/article/details/126814596

string CJZAPI strtail(const string& s, int cnt = 1)
{
	if (cnt > s.size())
		return s;
	return s.substr(s.size() - cnt, cnt);
}
string CJZAPI strhead(const string& s, int cnt = 1)
{
	if (cnt > s.size())
		return s;
	return s.substr(0, cnt);
}
string CJZAPI strxtail(const string& s, int cnt = 1)
{
	if (cnt > s.size())
		return "";
	return s.substr(0, s.size() - cnt);
}
string CJZAPI strxhead(const string& s, int cnt = 1)
{
	if (cnt > s.size())
		return "";
	return s.substr(cnt, s.size() - cnt);
}


VOID CJZAPI InfoTip(HWND hwnd = nullptr, string_view strText = "Information", string_view strTitle = "InfoTip")
{
	MessageBoxW(hwnd, String2WString(strText).c_str(), String2WString(strTitle).c_str(), MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL);
}
VOID CJZAPI ExclaTip(HWND hwnd = nullptr, string_view strText = "Exclamation", string_view strTitle = "ExclaTip")
{
	MessageBoxW(hwnd, String2WString(strText).c_str(), String2WString(strTitle).c_str(), MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
}
VOID CJZAPI ErrorTip(HWND hwnd = nullptr, string_view strText = "Error", string_view strTitle = "ErrorTip")
{
	MessageBoxW(hwnd, String2WString(strText).c_str(), String2WString(strTitle).c_str(), MB_ICONERROR | MB_OK | MB_SYSTEMMODAL);
}
VOID CJZAPI MsgSndTip(VOID)
{
	MessageBeep(MB_ICONINFORMATION);
}


//MUST X86
BOOL PreventSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER myFilterFunc)
{
	//ILHook
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void* pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;
	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void* pNewFunc = myFilterFunc;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[0] = 0xE9;  // JMP absolute
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
	return bRet;
}

inline BOOL CJZAPI ExistFile(string_view strFile) {
	//文件或文件夹都可以
	return !_access(strFile.data(), S_OK);//S_OK表示只检查是否存在
}
inline BOOL CJZAPI ExistFile(wstring_view strFile) {
	//文件或文件夹都可以
	return !_waccess(strFile.data(), S_OK);//S_OK表示只检查是否存在
}
void ErrorLogTip(HWND hwnd, string_view strText,
	string_view moduleName = ""
)
{
	string db;
	db += strText;
	db += sprintf2("  (%s)", moduleName.data());
	//db = string(strrpc((char*)db.c_str(), "\n", " | "));	//删除内部换行 
	cout << (db + "\n"s);
	ErrorTip(hwnd, strText, "BedwarLive " + str(moduleName) + " ERROR");
}

vector<string> CJZAPI CutLine(string line, char sep = ' ')
{
	vector<string> ret{};
	if (line.empty()) return ret;
	int p1 = 0, p2 = 0;
	for (int i = 0; i < line.size(); i++)
	{
		if (line.at(i) == sep
			|| i == line.size() - 1)
		{
			p2 = i;
			string s = line.substr(p1, p2 - p1 + (i == line.size() - 1 ? 1 : 0));
			ret.push_back(s);
			p1 = i + 1;
		}
	}
	return ret;
}
vector<string> CJZAPI ReadFileLines(const wstring& filePath)
{	//读取文件每一行 
	vector<string> ret{};
	constexpr int lineMaxCharCnt = 2048;
	fstream fin(filePath, ios::in);
	if (fin.fail())
		return ret;
	while (1)
	{
		char s[lineMaxCharCnt];

		if (fin.eof())
			break;

		fin.getline(s, lineMaxCharCnt);
		ret.push_back(ToString(s));
	}
	fin.close();
	return ret;
}
inline bool CJZAPI strcaseequ(char* str, const char* obj)
{	//无视大小写的比较 
	return (_stricmp((const char*)str, obj) == 0 ? true : false);
}
inline bool CJZAPI sequ(const string& s, const string& obj)
{
	return strcaseequ((char*)s.c_str(), obj.c_str());
}

//↓ must ege

int g_lineNum = 0;	//行数 , 从零开始 

void CJZAPI initprint(void)
{
	g_lineNum = 0;
}
void CJZAPI cls(bool bZeroLineNum = true)
{	//清屏 
	cleardevice();
	if (bZeroLineNum)
		g_lineNum = 0;
}
void CJZAPI blur_cls(bool bZeroLineNum = true)
{	//模糊清屏 
	imagefilter_blurring(NULL, 0x20, 0xFF);
	if (bZeroLineNum)
		g_lineNum = 0;
}

#define CUR_LINE_NUM (g_lineNum)	//当前行 
#define NEXT_LINE_NUM (++g_lineNum)	//下一行 

#define _OFFSET_X 0
#define drawtext(s,x,y)	outtextxy(x,y,s)

RECT CJZAPI midprint(wstring_view strText, int lineNum = NEXT_LINE_NUM, int midx = uix / 2)
{	//居中显示句子 
	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;

	int y = fy * lineNum;

	//xyprintf(x, y, "%ws", strText.data());
	drawtext(strText.data(), x, y);

	RECT rt;
	rt.left = x;
	rt.right = x + sx;
	rt.top = y;
	rt.bottom = y + fy;
	return rt;	//返回矩形
}
RECT CJZAPI midprint(string_view strText, int lineNum = NEXT_LINE_NUM, int midx = uix / 2)
{
	return midprint(String2WString(strText), lineNum, midx);
}
RECT CJZAPI midprinty(wstring_view strText, int y, int midx = uix / 2)
{	//自定义 y
	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	//xyprintf(x, y, "%ws", strText.data());
	drawtext(strText.data(), x, y);

	RECT rt;
	rt.left = x;
	rt.right = x + sx;
	rt.top = y;
	rt.bottom = y + fy;
	return rt;	//返回矩形
}
RECT CJZAPI midprinty(string_view strText, int y, int midx = uix / 2)
{
	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	//xyprintf(x, y, "%s", strText.data());
	drawtext(strText.data(), x, y);

	RECT rt;
	rt.left = x;
	rt.right = x + sx;
	rt.top = y;
	rt.bottom = y + fy;
	return rt;	//返回矩形
}
RECT CJZAPI midprinty_shadow(wstring_view strText, int y, int midx = uix / 2, int shadow_distance = 1)
{
	color_t color = getcolor();
	setcolor(EGERGBA(1, 1, 1, EGEGET_A(color)));

	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	//xyprintf(x + shadow_distance, y + shadow_distance, "%s", strText.data());
	drawtext(strText.data(), x + shadow_distance, y + shadow_distance);

	setcolor(color);
	return midprinty(strText, y, midx);
}
RECT CJZAPI midprinty_shadow(string_view strText, int y, int midx = uix / 2, int shadow_distance = 1)
{
	color_t color = getcolor();
	setcolor(EGERGBA(1, 1, 1, EGEGET_A(color)));

	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	//xyprintf(x + shadow_distance, y + shadow_distance, "%s", strText.data());
	drawtext(strText.data(), x + shadow_distance, y + shadow_distance);
	setcolor(color);
	return midprinty(strText, y, midx);
}
RECT CJZAPI midprinty_stroke(wstring_view strText, int y, int midx = uix / 2, int stroke_distance = 1)
{
	color_t color = getcolor();
	setcolor(EGERGB(1, 1, 1));

	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	//xyprintf(x , y , "%ws", strText.data());
	drawtext(strText.data(), x + stroke_distance, y + stroke_distance);
	//xyprintf(x , y , "%ws", strText.data());
	drawtext(strText.data(), x - stroke_distance, y + stroke_distance);
	//xyprintf(x , y , "%ws", strText.data());
	drawtext(strText.data(), x - stroke_distance, y - stroke_distance);
	//xyprintf(x , y , "%ws", strText.data());
	drawtext(strText.data(), x + stroke_distance, y - stroke_distance);

	setcolor(color);
	return midprinty(strText, y, midx);
}
RECT CJZAPI midprinty_stroke(string_view strText, int y, int midx = uix / 2, int stroke_distance = 1)
{
	color_t color = getcolor();
	setcolor(EGERGB(1, 1, 1));

	int fy = textheight('1');	//font height
	int sx = textwidth(strText.data());
	int x = midx - sx / 2 + _OFFSET_X;
	drawtext(strText.data(), x + stroke_distance, y + stroke_distance);
	drawtext(strText.data(), x - stroke_distance, y + stroke_distance);
	drawtext(strText.data(), x - stroke_distance, y - stroke_distance);
	drawtext(strText.data(), x + stroke_distance, y - stroke_distance);

	setcolor(color);
	return midprinty(strText, y, midx);
}

int CJZAPI xyprintf_shadow(int x, int y, const wchar_t* szFormat, ...)
{
	constexpr int shadow_distance{ 1 };
	wchar_t buffer[4096]{ L"\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = _vsnwprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	//outtextxy(x + shadow_distance, y + shadow_distance, buffer);
	drawtext(buffer, x + shadow_distance, y + shadow_distance);
	setcolor(ocolor);
	//outtextxy(x, y, buffer);
	drawtext(buffer, x, y);
	return wcslen(buffer);
}
int CJZAPI xyprintf_shadow(int x, int y, const char* szFormat, ...)
{
	constexpr int shadow_distance{ 1 };
	char buffer[4096]{ "\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = _vsnprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	//outtextxy(x + shadow_distance, y + shadow_distance, buffer);
	drawtext(buffer, x + shadow_distance, y + shadow_distance);
	setcolor(EGECOLORA(ocolor, 255));
	//outtextxy(x, y, buffer);
	drawtext(buffer, x, y);
	return strlen(buffer);
}
int CJZAPI xyprintf_stroke(int x, int y, const char* szFormat, ...)
{
	constexpr int stroke_distance{ 1 };
	char buffer[4096]{ "\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = _vsnprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	drawtext(buffer, x + stroke_distance, y + stroke_distance);
	drawtext(buffer, x + stroke_distance, y - stroke_distance);
	drawtext(buffer, x - stroke_distance, y - stroke_distance);
	drawtext(buffer, x - stroke_distance, y + stroke_distance);
	setcolor(ocolor);
	//outtextxy(x, y, buffer);
	drawtext(buffer, x, y);
	return strlen(buffer);
}
int CJZAPI rectprintf_shadow(int x, int y, int w, int h, const wchar_t* szFormat, ...)
{
	constexpr int shadow_distance{ 1 };
	wchar_t buffer[4096]{ L"\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = _vsnwprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	outtextrect(x + shadow_distance, y + shadow_distance, w, h, buffer);
	setcolor(ocolor);
	outtextrect(x, y, w, h, buffer);
	return wcslen(buffer);
}
int CJZAPI rectprintf_shadow(int x, int y, int w, int h, const char* szFormat, ...)
{
	constexpr int shadow_distance{ 1 };
	char buffer[4096]{ "\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = vsnprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	outtextrect(x + shadow_distance, y + shadow_distance, w, h, buffer);
	setcolor(ocolor);
	outtextrect(x, y, w, h, buffer);
	return strlen(buffer);
}
int CJZAPI rectprintf_stroke(int x, int y, int w, int h, const char* szFormat, ...)
{
	constexpr int stroke_distance{ 1 };
	char buffer[4096]{ "\0" };
	va_list va;
	va_start(va, szFormat);
	int ret = vsnprintf(buffer, 4096, szFormat, va);
	va_end(va);

	color_t ocolor{ getcolor() };
	setcolor(EGERGB(1, 1, 1));
	outtextrect(x + stroke_distance, y + stroke_distance, w, h, buffer);
	outtextrect(x + stroke_distance, y - stroke_distance, w, h, buffer);
	outtextrect(x - stroke_distance, y - stroke_distance, w, h, buffer);
	outtextrect(x - stroke_distance, y + stroke_distance, w, h, buffer);
	setcolor(ocolor);
	outtextrect(x, y, w, h, buffer);
	return strlen(buffer);
}

#define KEY_DOWN(vk) ((GetAsyncKeyState(vk)& 0x8000 ? 1:0))
#define KEY_DOWN_FOREMOST(hWnd,vk) (KEY_DOWN(vk) && GetForegroundWindow()==hWnd)	//最前面 
#		define KEY_DOWN_FOCUSED(hWnd,vk) KEY_DOWN_FOREMOST(hWnd,vk)	//带焦点 
#define K(vk) KEY_DOWN_FOCUSED(g.hwnd,vk)


double GetFadeinoutRate(clock_t fadeinout_time, clock_t total_time, clock_t now_duration)
{
	if (now_duration >= total_time)
		return 0.0;
	if (now_duration < fadeinout_time)
		return now_duration / double(fadeinout_time);
	if (now_duration > total_time - fadeinout_time)
		return 1.0 - (now_duration - total_time + fadeinout_time) / double(fadeinout_time);
	return 1.0;
}

inline color_t CJZAPI InvertedColor(color_t clr) {
	return EGERGB(255 - EGEGET_R(clr), 255 - EGEGET_G(clr), 255 - EGEGET_B(clr));
}
inline color_t CJZAPI AlphaColor(color_t clr, float rate)
{
	return EGECOLORA(clr, int(255 * rate));
}

COLORREF CJZAPI StepColor0(COLORREF startColor, COLORREF endColor, double rate)
{	//颜色的渐变 
	int r = (GetRValue(endColor) - GetRValue(startColor));
	int g = (GetGValue(endColor) - GetGValue(startColor));
	int b = (GetBValue(endColor) - GetBValue(startColor));

	int nSteps = max(abs(r), max(abs(g), abs(b)));
	if (nSteps < 1) nSteps = 1;

	// Calculate the step size for each color
	float rStep = r / (float)nSteps;
	float gStep = g / (float)nSteps;
	float bStep = b / (float)nSteps;

	// Reset the colors to the starting position
	float fr = GetRValue(startColor);
	float fg = GetGValue(startColor);
	float fb = GetBValue(startColor);

	COLORREF color;
	for (int i = 0; i < int(nSteps * rate); i++)
	{
		fr += rStep;
		fg += gStep;
		fb += bStep;
		color = RGB((int)(fr + 0.5), (int)(fg + 0.5), (int)(fb + 0.5));
		//color 即为重建颜色
	}
	return color;
}//from https://bbs.csdn.net/topics/240006897 , owner: zgl7903

COLORREF CJZAPI WinColor(color_t clr)
{	//EGERGB -> RGB
	return RGB(EGEGET_R(clr),EGEGET_G(clr),EGEGET_B(clr));
}
color_t CJZAPI EgeColor(COLORREF clr)
{	//RGB -> EGERGB
	return EGERGB(GetRValue(clr),GetGValue(clr),GetBValue(clr));
}
inline color_t CJZAPI StepColorOld(color_t startColor, color_t endColor, double rate)
{
	return EgeColor(StepColor0(WinColor(startColor), WinColor(endColor), rate));
}
color_t CJZAPI StepColor(color_t startColor, color_t endColor, double rate)
{
	int r = EGEGET_R(endColor) - EGEGET_R(startColor);
	int g = EGEGET_G(endColor) - EGEGET_G(startColor);
	int b = EGEGET_B(endColor) - EGEGET_B(startColor);
	int a = EGEGET_A(endColor) - EGEGET_A(startColor);

	int nSteps = max(abs(a), max(abs(r), max(abs(g), abs(b))));
	if (nSteps < 1) nSteps = 1;

	// Calculate the step size for each color
	float rStep = r / (float)nSteps;
	float gStep = g / (float)nSteps;
	float bStep = b / (float)nSteps;
	float aStep = a / (float)nSteps;

	// Reset the colors to the starting position
	float fr = EGEGET_R(startColor);
	float fg = EGEGET_G(startColor);
	float fb = EGEGET_B(startColor);
	float fa = EGEGET_A(startColor);

	color_t color = 0;
	for (int i = 0; i < int(nSteps * rate); i++)
	{
		fr += rStep;
		fg += gStep;
		fb += bStep;
		fa += aStep;
		color = EGERGBA((int)(fr + 0.5), (int)(fg + 0.5), (int)(fb + 0.5), (int)(fa + 0.5));
		//color 即为重建颜色
	}
	return color;
}
#define WVC_AMP 12
#define WVC_OMEGA 13.0
#define WVC_PHASE0 0
clock_t lastWvBeg = 0;
inline color_t WaveColor(color_t originClr, float phi = 0.0f) {	//originClr将成为最大值
	//闪烁的颜色 赋予游戏文字灵性
	short val = WVC_AMP * sin(WVC_OMEGA*((clock() - lastWvBeg) / 1000.0) + WVC_PHASE0 + phi) - WVC_AMP * 2;
	short r = EGEGET_R(originClr) + val, g = EGEGET_G(originClr) + val, b = EGEGET_B(originClr) + val;
	ClampA<short>(r, 2, 255);
	ClampA<short>(g, 2, 255);
	ClampA<short>(b, 2, 255);
	return EGERGB(r, g, b);
}
#define setwcolor(clr) setcolor(WaveColor(clr))
#define setwcolor2(clr,phi) setcolor(WaveColor(clr, phi))
#define setcolor2(clr,phi) setcolor(clr)
#define setcolora(clr, rate) setcolor(AlphaColor(clr, rate))
#define setwcolora(clr, rate) setwcolor(AlphaColor(clr, rate))
#define setwcolora2(clr, rate, phi) setwcolor2(AlphaColor(clr, rate), phi)
color_t UnstableColor(color_t originClr) noexcept
{
	short r = EGEGET_R(originClr);
	short g = EGEGET_G(originClr);
	short b = EGEGET_B(originClr);
	r += Choice({ -10, -5,-3,0,3,5 ,10 });
	g += Choice({ -10, -5,-3,0,3,5 ,10 });
	b += Choice({ -10, -5,-3,0,3,5 ,10 });
	ClampA<short>(r, 1, 255);
	ClampA<short>(g, 1, 255);
	ClampA<short>(b, 1, 255);
	return EGERGB(r, g, b);
}

// 辅助函数：RGB到HSV颜色转换
void RGBtoHSV(COLORREF rgb, double& h, double& s, double& v) {
	double r = GetRValue(rgb) / 255.0;
	double g = GetGValue(rgb) / 255.0;
	double b = GetBValue(rgb) / 255.0;

	double minVal = min(min(r, g), b);
	double maxVal = max(max(r, g), b);
	double delta = maxVal - minVal;

	// 计算色相
	if (delta > 0) {
		if (maxVal == r)
			h = 60.0 * fmod((g - b) / delta, 6.0);
		else if (maxVal == g)
			h = 60.0 * ((b - r) / delta + 2.0);
		else if (maxVal == b)
			h = 60.0 * ((r - g) / delta + 4.0);
	}
	else {
		h = 0.0;
	}

	// 计算饱和度和亮度
	s = (maxVal > 0) ? (delta / maxVal) : 0.0;
	v = maxVal;
}

// 辅助函数：HSV到RGB颜色转换
COLORREF HSVtoRGB(double h, double s, double v) {
	int hi = static_cast<int>(floor(h / 60.0)) % 6;
	double f = h / 60.0 - floor(h / 60.0);
	double p = v * (1.0 - s);
	double q = v * (1.0 - f * s);
	double t = v * (1.0 - (1.0 - f) * s);

	switch (hi) {
	case 0: return RGB(static_cast<int>(v * 255), static_cast<int>(t * 255), static_cast<int>(p * 255));
	case 1: return RGB(static_cast<int>(q * 255), static_cast<int>(v * 255), static_cast<int>(p * 255));
	case 2: return RGB(static_cast<int>(p * 255), static_cast<int>(v * 255), static_cast<int>(t * 255));
	case 3: return RGB(static_cast<int>(p * 255), static_cast<int>(q * 255), static_cast<int>(v * 255));
	case 4: return RGB(static_cast<int>(t * 255), static_cast<int>(p * 255), static_cast<int>(v * 255));
	case 5: return RGB(static_cast<int>(v * 255), static_cast<int>(p * 255), static_cast<int>(q * 255));
	default: return RGB(0, 0, 0);  // Shouldn't reach here
	}
}

// 主函数：返回随时间变化的彩虹色
color_t RainbowColor() {
	// 假设时间按秒计算，这里使用系统时间或其他适当的时间源
	double timeInSeconds = GetTickCount() / 1000.0;

	// 色相按时间变化
	double hue = fmod(timeInSeconds * 30.0, 360.0);  // 假设每秒变化30度

	// 饱和度和亮度保持不变
	double saturation = 1.0;
	double value = 1.0;

	// 将HSV颜色转换为RGB并返回
	return EgeColor(HSVtoRGB(hue, saturation, value));
}

// 定义一个颜色映射，存储每个字符串对应的颜色
unordered_map<string, color_t> colorMap;

// 函数：生成饱和度最高的随机颜色
color_t GetHighSaturationColor() {
	// 使用 C++ 随机数引擎生成随机的 RGB 分量
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(0, 255);

	// 随机选择一个颜色分量作为最大值，其余两个分量设置为较小值
	int r = dis(gen);
	int g = dis(gen);
	int b = dis(gen);

	if (r >= g && r >= b) {
		g = dis(gen) / 2;  // 降低 g 的值
		b = dis(gen) / 2;  // 降低 b 的值
	}
	else if (g >= r && g >= b) {
		r = dis(gen) / 2;  // 降低 r 的值
		b = dis(gen) / 2;  // 降低 b 的值
	}
	else {
		r = dis(gen) / 2;  // 降低 r 的值
		g = dis(gen) / 2;  // 降低 g 的值
	}

	// 返回最终的 RGB 颜色值
	return EGERGB(r, g, b);
}

// 函数：根据字符串返回对应颜色
color_t GetStringColor(string_view s) {
	// 查找字符串是否已经有对应的颜色
	auto it = colorMap.find(string(s));
	if (it != colorMap.end()) {
		// 如果已存在，直接返回存储的颜色
		return it->second;
	}
	else {
		// 否则生成一个新的颜色并存储
		color_t newColor = GetHighSaturationColor();
		colorMap[string(s)] = newColor;
		return newColor;
	}
}

//橙黄系列
#define IVORY EGERGB(255,255,240)	//极淡黄 
#define CORNSILK EGERGB(255,248,220)	//淡黄 
#define GOLD EGERGB(255,215,0)
#define ORANGE EGERGB(255,165,0)

//绿色系列
#define DARKGREEN EGERGB(0,100,0)

//蓝系列
#define POWDERBLUE EGERGB(176,224,230)
#define AQUA EGERGB(0,0xFC,0xFC)
#define ROYALBLUE EGERGB(65,105,225)
#define STEELBLUE EGERGB(70,130,180)
#define CADETBLUE EGERGB(95,158,160)
#define LIGHTBLUE1 EGERGB(191,239,255)
#define LIGHTCYAN1 EGERGB(224,255,255)
#define TURQUOISE EGERGB(64,224,208)	//青色 
#define CYAN EGERGB(0,0xA8,0xA8)

//粉紫系列
#define PINK EGERGB(255,105,180)
#define LIGHTPINK EGERGB(255,182,193)
#define MAROON EGERGB(176,48,96)
#define VIOLET EGERGB(238,130,238)
#define PLUM EGERGB(221,160,221)
#define PURPLE EGERGB(160,32,240)

//黑白系列
#define GRAY_11 EGERGB(28,28,28)
#define DIMGRAY EGERGB(105,105,105)
#define SLATEGRAY EGERGB(112,128,144)	//略带蓝色

////////////////////////////////////////////////////////

typedef USHORT DIR, * PDIR;	//方向
#define DIR2	DIR
#define DIR4	DIR
#define DIR8	DIR
#define PDIR2	PDIR
#define PDIR4	PDIR
#define PDIR8	PDIR

#define NO_DIR 0x00
#define UP 0x01
#define DIR_FIRST UP
#define RIGHTUP 0x02
#define RIGHT 0x03
#define RIGHTDOWN 0x04
#define DOWN 0x05
#define LEFTDOWN 0x06
#define LEFT 0x07
#define LEFTUP 0x08
#define DIR_LAST LEFTUP

#define VERT	0x01
#define HORIZ	0x03


const vector<string> dir_names
{
	"none", "up", "rightup", "right", "rightdown", "down", "leftdown", "left", "leftup",
};
DIR ParseDir(const string& s)
{
	for (DIR dir = 0; dir <= DIR_LAST; ++dir)
		if (s == dir_names.at(dir))
			return dir;
	ErrorLogTip(nullptr, "Invalid direction name: " + s, "ParseDir");
	return 0;
}
inline string GetDirName(DIR dir)
{
	return dir_names.at(dir);
}

inline DIR OppoDir(DIR dir)
{
	return (dir + 4) % 8;
}

void DirOffsetPos(double& x, double& y, DIR dir, double offset = 10.0) {
	//依据方向位移坐标

	if (dir == UP)
		y -= offset;
	else if (dir == RIGHTUP)
		x += offset, y -= offset;
	else if (dir == RIGHT)
		x += offset;
	else if (dir == RIGHTDOWN)
		x += offset, y += offset;
	else if (dir == DOWN)
		y += offset;
	else if (dir == LEFTDOWN)
		x -= offset, y += offset;
	else if (dir == LEFT)
		x -= offset;
	else if (dir == LEFTUP)
		x -= offset, y--;
	else {
		return;
	}
}

DIR RandomDir(void)
{
	return DIR(RandomRange(0, 7, true, true));
}
double StdRadian(double rad) 
{
	static const double TwoPi = 2.0 * PI; // 定义2π的值

	// 将弧度值调整到范围 [0, 2π]
	rad = fmod(rad, TwoPi);
	if (rad < 0.0) {
		rad += TwoPi;
	}

	return rad;
}
inline double OppoRadian(double rad)
{
	return StdRadian(rad + PI);
}
double RadianDiff(double rad_1, double rad_2)
{
	double diff = abs(StdRadian(rad_1) - StdRadian(rad_2));
	if (diff > PI)
		diff = 2 * PI - diff;
	return diff;
}
bool LeftTheta(double radian)
{
	double stdrad = StdRadian(radian);
	return (stdrad > PI / 2.0 && stdrad < PI * 1.5);
}
bool RightTheta(double radian)
{
	double stdrad = StdRadian(radian);
	return (stdrad < PI / 2.0 || stdrad > PI * 1.5);
}
bool DownTheta(double radian)
{
	double stdrad = StdRadian(radian);
	return (stdrad > PI);
}
bool UpTheta(double radian)
{
	double stdrad = StdRadian(radian);
	return (stdrad < PI);
}
double RandomTheta()
{
	return RandomRangeDouble(0.0, PI * 2.0);
}

auto pow2(auto&& __x)
{
	return __x * __x;
}
inline double Distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow2(x1 - x2) + pow2(y1 - y2));
}

///////////////////////////////////////////////


class Vector2D {	//二维矢量
public:
	double x;
	double y;

	Vector2D(double _x, double _y) : x{ _x }, y{ _y } {}
	Vector2D(const Vector2D& src)
	{
		x = src.x;
		y = src.y;
	}
	Vector2D()
	{
		x = y = 0.0;
	}
	double Theta() const
	{
		return atan2(-y, x);
	}
	void Reset(double theta, double length)
	{	//用极坐标
		x = cos(theta) * length;
		y = -sin(theta) * length;
	}
	Vector2D operator+ (Vector2D& second) const
	{	//向量加法 
		return Vector2D(x + second.x, y + second.y);
	}
	Vector2D operator- (Vector2D& second) const
	{	//向量减法 
		return Vector2D(x - second.x, y - second.y);
	}
	double operator*(const Vector2D& second) const
	{	//数量积 
		return (x * second.x + y * second.y);
	}
	Vector2D operator*(double a) const
	{	//数乘 
		return Vector2D(x * a, y * a);
	}
	Vector2D operator*(int a) const
	{	//数乘 
		return Vector2D(x * a, y * a);
	}
	Vector2D operator/(double a) const
	{
		return Vector2D(x / a, y / a);
	}
	Vector2D operator/(int a) const
	{
		return Vector2D(x / a, y / a);
	}
	Vector2D& operator=(const Vector2D& src)
	{	//赋值 
		x = src.x;
		y = src.y;
		return *this;
	}
	double len()	const//模 
	{
		return double(sqrt(x * x + y * y));
	}
	double operator()() const
	{	//模 
		return double(sqrt(x * x + y * y));
	}
	operator double() const
	{	//模 
		return double(sqrt(x * x + y * y));
	}
	bool operator== (const Vector2D& sec) const
	{	//相等 
		return (x == sec.x && y == sec.y);
	}
	bool operator!= (const Vector2D& sec) const
	{	//不等 
		return (x != sec.x || y != sec.y);
	}
	bool operator > (Vector2D sec) const
	{
		return len() > sec.len();
	}
	bool operator >= (Vector2D sec) const
	{
		return len() >= sec.len();
	}
	bool operator < (Vector2D sec) const
	{
		return len() < sec.len();
	}
	bool operator <= (Vector2D sec) const
	{
		return len() <= sec.len();
	}

	Vector2D& operator+= (const Vector2D& sec) {
		x += sec.x;
		y += sec.y;
		return *this;
	}
	Vector2D& operator-= (const Vector2D& sec) {
		x -= sec.x;
		y -= sec.y;
		return *this;
	}
	Vector2D& operator *= (const double t) {
		x *= t;
		y *= t;
		return *this;
	}
	Vector2D& operator /= (const double t) {
		x /= t;
		y /= t;
		return *this;
	}
	bool IsZero() const
	{	//是否是零向量 
		return (fequ(x, 0.0) && fequ(y, 0.0));
	}
	Vector2D ReversedVector()	 const//返回 相反向量 
	{
		return Vector2D(-x, -y);
	}
	inline void Reverse()	//相反向量 赋值
	{
		x = -x, y = -y;
	}
	Vector2D Unit() const
	{	//该向量方向上的单位向量
		Vector2D ret;
		double len = operator()();
		if (fequ(len, 0.0))
		{	//零向量 
			ret.x = ret.y = sqrt(2.0) / 2.0;
			return ret;
		}
		ret.x = x * (1.0f / len);
		ret.y = y * (1.0f / len);
		return ret;
	}
	Vector2D ClampMagnitude(double maxLength) const
	{	//限制向量长度
		Vector2D ret = *this;
		double len = ret.operator()();
		if (len > maxLength)
		{
			ret.x = ret.x * (maxLength / len);
			ret.y = ret.y * (maxLength / len);
		}
		return ret;
	}
	void ClampMagnitudeA(double maxLength)
	{
		double len = operator()();
		if (len > maxLength)
		{
			x = x * (maxLength / len);
			y = y * (maxLength / len);
		}
	}
	void SetLength(double newlen)
	{
		double len = operator()();
		if (fequ(len, 0.0))
		{	//零向量 
			x = y = newlen / sqrt(2.0);
			return;
		}
		x = x * (newlen / len);
		y = y * (newlen / len);
	}
	inline void Clear()
	{
		x = y = 0.0;
	}
	DIR4 Dir4()	 const//归类到四个方向中的一个
	{
		double dx = 0.0 - x;
		double dy = 0.0 - y;
		DIR4 adir = RIGHT;
		double k = abs(dy) / (double)abs(dx);
		if (dx > 0)
		{
			if (k <= 1.0)
			{
				adir = RIGHT;
			}
			else {
				adir = (dy < 0 ? UP : DOWN);
			}
		}
		else {
			if (k <= 1.0)
			{
				adir = LEFT;
			}
			else {
				adir = (dy < 0 ? UP : DOWN);
			}
		}
		return adir;
	}
	DIR8 Dir8()	 const//归类到八个方向中的一个
	{
		double dx = 0.0 - x;
		double dy = 0.0 - y;
		DIR8 adir = RIGHT;
		double k = abs(dy) / (double)abs(dx);
		if (dx > 0)
		{	//右

			if (k <= tan(PI / 8.0))
			{
				adir = RIGHT;
			}
			else if (k <= tan(PI * 3.0 / 8.0))
			{
				adir = (dy < 0 ? RIGHTUP : RIGHTDOWN);
			}
			else {
				adir = (dy < 0 ? UP : DOWN);
			}
		}
		else {	//左
			if (k <= tan(PI / 8.0))
			{
				adir = LEFT;
			}
			else if (k <= tan(PI * 3.0 / 8.0))
			{
				adir = (dy < 0 ? LEFTUP : LEFTDOWN);
			}
			else {
				adir = (dy < 0 ? UP : DOWN);
			}
		}
		return adir;
	}
};

///////////////////////////////////////////////

using Defense = short;

void DefendDamage(double& damage, Defense def)
{
	damage = damage - def / 2.0;
	ClampA(damage, 0.5, 100.0);
}
