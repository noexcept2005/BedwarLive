import threading
import time
import tkinter.simpledialog  # 使用Tkinter前需要先导入
from tkinter import END, messagebox

import requests

WORMWAKER_ROOM_ID = 31196635
CLENIGHTEN_ROOM_ID = 30972823

# 全局变量，用于标识线程是否退出
is_exit = True

my_room_id = WORMWAKER_ROOM_ID		
cd_seconds = 1.5

dlist = []
chat_cnt = 0

def GetCDSeconds():
	if chat_cnt == 0:
		return 2.5
	if chat_cnt == 1:
		return 2.0
	if chat_cnt <= 4:
		return 1.5
	if chat_cnt <= 8:
		return 1.0
	return 0.8

# B站获取弹幕对象
class ChatManager():
    def __init__(self, room_id):
        # 弹幕url
        self.url = 'https://api.live.bilibili.com/xlive/web-room/v1/dM/gethistory'
        # 请求头
        self.headers = {
            'Host': 'api.live.bilibili.com',
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:78.0) Gecko/20100101 Firefox/78.0',
        }
        # 定义POST传递的参数
        self.data = {
            'roomid': room_id,
            'csrf_token': '',
            'csrf': '',
            'visit_id': '',
        }
        # 日志写对象
        self.log_file_write = open('chat.log', mode='a', encoding='utf-8')
        # 读取日志
        log_file_read = open('chat.log', mode='r', encoding='utf-8')
        self.log = log_file_read.readlines()

    def get_chat(self):
        global dlist
        # 暂停0.5防止cpu占用过高
        
        # 获取直播间弹幕
        html = requests.post(url=self.url, headers=self.headers, data=self.data).json()
        # 解析弹幕列表
        rl = []
        for content in html['data']['room']:
            # 获取昵称
            nickname = content['nickname']
            # 获取发言
            text = content['text']
            # 获取发言时间
            timeline = content['timeline']
            dr = ChatRecord(nickname, text, timeline)

            if dr.gt() not in dlist:
                dlist.append(dr.gt())
                #print("新的弹幕:{} : {}  ({})".format(nickname, text, timeline))
                rl.append(dr)
                #print(text)
            # 记录发言
            msg = timeline + ' ' + nickname + ': ' + text
            # 清空变量缓存
            nickname = ''
            text = ''
            timeline = ''
            msg = ''
        return rl
class ChatRecord:
    def __init__(self, a, b, c):
        self.nickname = a
        self.text = b
        self.time = c
    def gt(self):
        return "[time {} nickname {} content {}]".format(self.time, self.nickname, self.text)

# 线程对象
def bilibili(room_id):
    # 创建bChatManager实例
    bChatManager = ChatManager(room_id)
    # 获取弹幕
    return bChatManager.get_chat()
if __name__ == '__main__':
	try:
		print("直播间弹幕获取已开启")
		print("===========================")
		while 1:
			time.sleep(GetCDSeconds())
			wtd = ""
			chat_cnt = 0
			for i in bilibili(my_room_id):
				print(i.nickname, '>> ', i.text, '        ', i.time, sep='')
				wtd += i.text + '`' + i.nickname + '\n'
				chat_cnt += 1
			f = open("commands.txt", "a", encoding='utf-8')
			f.write(wtd)
			f.close()
	except ConnectionError as e:
		print("===========================")
		print("连接错误，请检查网络连接！")
		print("ConnectionError:")
		print(e)
		print("===========================")
	except ConnectionResetError as e:
		print("===========================")
		print("连接被重置！")
		print("远程主机强迫关闭了一个现有的连接。")
		print("===========================")