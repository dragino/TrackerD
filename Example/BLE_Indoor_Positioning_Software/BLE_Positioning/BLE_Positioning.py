import tkinter as tk
from tkinter import *
import paho.mqtt.subscribe as subscribe #导入MQTT库
import json
import os , sys
import threading
from queue import Queue
import re
import time
#MQTT方法
def mqtt1(q):
    a = q.get()#获取线程间的变量
    if getattr(sys, 'frozen', False):#获取路径
        absPath = os.path.dirname(os.path.abspath(sys.executable))
    elif __file__:
        absPath = os.path.dirname(os.path.abspath(__file__))
    path = os.path.join(absPath)
    while True:#链接MQTT
        with open(f'{path}\log.txt','a+') as f:
            try:
                m = subscribe.simple(topics=a[0], hostname=a[1], retained=False, port=1883,
                                     auth={'username': a[2],
                                           'password': a[3]},
                                     msg_count=1)
                #print(m)
                data = m.payload  # 获取mqtt订阅到的数据
                data1 = str(data, "utf-8")  # bytes转为字符串
                data2 = json.loads(data1)  # 通过json将字符串转为字典
                if data2["uplink_message"]["f_port"]==6:
                    data3 = data2["uplink_message"]["decoded_payload"]["MAJOR"]
                    data4 = data2["uplink_message"]["decoded_payload"]["MINOR"]
                    data5 = data2["uplink_message"]["decoded_payload"]["Time"]
                    f.write(f"{str(data3)},{str(data4)},{str(data5)}\n")
            except  Exception as e:
                #print(e)
                continue
#构建TK
def tk1():
    if getattr(sys, 'frozen', False):
        absPath = os.path.dirname(os.path.abspath(sys.executable))
    elif __file__:
        absPath = os.path.dirname(os.path.abspath(__file__))
    path = os.path.join(absPath)
    win = tk.Tk()
    win.title('DRAGINO positioning simulator')
    win.geometry('1000x800')
    win.config(background ="#87CEFA")
    l1 = tk.Label(win, text="Subscribe to topics",bg="#87CEFA",fg="black")#订阅主题
    t1 = tk.Entry()#订阅主题
    l1.place(x=0,y=0)
    t1.place(x=0,y=30)
    l2 = tk.Label(win, text="server address", bg="#87CEFA", fg="black")  # 服务器地址
    t2 = tk.Entry()  # 服务器地址
    l2.place(x=150, y=0)
    t2.place(x=150, y=30)
    l3 = tk.Label(win, text="Application ID", bg="#87CEFA", fg="black")  # APIID
    t3 = tk.Entry()  # APIID
    l3.place(x=300, y=0)
    t3.place(x=300, y=30)
    l4 = tk.Label(win, text="APIKEY", bg="#87CEFA", fg="black")  # 密钥
    t4 = tk.Entry()  # 密钥
    l4.place(x=450, y=0)
    t4.place(x=450, y=30)
    def time1():
        #import time
        l5 = tk.Label(win,text=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),bg="#87CEFA", fg="black")
        l5.place(x=750,y=0)
        l5.after(1000, time1)
    frame = tk.Frame(win)
    frame.place(x=100,y=70)

    listNodes = Listbox(frame, width=88, height=5,bg="#F5F5DC", font=("Helvetica", 12))
    listNodes.pack(side="left", fill="y")

    scrollbar = Scrollbar(frame, orient="vertical")
    scrollbar.config(command=listNodes.yview)
    scrollbar.pack(side="right", fill="y")

    listNodes.config(yscrollcommand=scrollbar.set)
    def putdata():
        q=Queue()#构建线程之间的联通
        data11=t1.get()#获取输入框的值
        data22=t2.get()
        data33=t3.get()
        data44=t4.get()
        q.put([data11,data22,data33,data44])#向线程间发送变量
        th = threading.Thread(target=mqtt1, args=(q,))#构建子进程
        th.setDaemon(True)
        th.start()#开始进程
        b1.configure(state=DISABLED)
    #构建画布
    cv = tk.Canvas(win, width=800, height=600)
    cv.place(x=100, y=180)
    img1 = PhotoImage(file=f"{path}/office_map.png")
    img = PhotoImage(file=f"{path}/icon.png")
    arc4 = cv.create_image(0, 0, image=img1, anchor='nw',tag='dd4')
    def mqtt2():
        try:
            a = []
            b = []
            c = []
            with open(f'{path}\log.txt', 'r') as f1:
                for i in f1:
                    a1 = (re.findall(r"(.+),(.+),(.+)", i))  # 正则
                    a.append(a1[0][0]),b.append(a1[0][1]),c.append(a1[0][2])
            if a[len(a)-1]=='0' and b[len(b)-1]=="60245" :
                if b[len(b) - 1] == b[len(b) - 2]:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                    ll1 = cv.create_text(118, 422, text=f"{c[len(c)-1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l1')
                    arc1 = cv.create_image(118, 462, image=img, tag='d1')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
                else:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), \
                    cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5")
                    ll1 = cv.create_text(118, 422, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l1')
                    arc1 = cv.create_image(118, 462, image=img, tag='d1')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
            elif a[len(a)-1]=='10006' and b[len(b)-1]=="79" :
                if b[len(b) - 1] == b[len(b) - 2]:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                    ll2 = cv.create_text(324, 456, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l2')
                    arc2 = cv.create_image(324, 496, image=img, tag='d2')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
                else:
                    cv.delete("d1"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), \
                        cv.delete("l1"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5")
                    ll2 = cv.create_text(324, 456, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l2')
                    arc2 = cv.create_image(324, 496, image=img, tag='d2')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
            elif a[len(a)-1]=='0' and b[len(b)-1]=="48710" :
                if b[len(b) - 1] == b[len(b) - 2]:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                    ll3 = cv.create_text(646, 468, text=f"{c[len(c)-1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l3')
                    arc3 = cv.create_image(646, 508, image=img, tag='d3')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
                else:
                    cv.delete("d2"), cv.delete("d1"), cv.delete("d4"), cv.delete("d5"), \
                        cv.delete("l2"), cv.delete("l1"), cv.delete("l4"), cv.delete("l5")
                    ll3 = cv.create_text(646, 468, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l3')
                    arc3 = cv.create_image(646, 508, image=img, tag='d3')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
            elif a[len(a)-1]=='1' and b[len(b)-1]=="59" :
                if b[len(b) - 1] == b[len(b) - 2]:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                    ll4 = cv.create_text(229, 97, text=f"{c[len(c)-1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l4')
                    arc4 = cv.create_image(229, 137, image=img, tag='d4')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
                else:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d1"), cv.delete("d5"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l1"), cv.delete("l5")
                    ll4 = cv.create_text(229, 97, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l4')
                    arc4 = cv.create_image(229, 137, image=img, tag='d4')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
            elif a[len(a)-1]=='0' and b[len(b)-1]=="55185" :
                if b[len(b) - 1] == b[len(b) - 2]:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                    cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                    ll5 = cv.create_text(621, 124, text=f"{c[len(c)-1]}", fill='#7CCD7C', anchor=N,
                                          font=('微软雅黑', 8, 'bold'), tags='l5')
                    arc5 = cv.create_image(621, 164, image=img, tag='d5')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
                else:
                    cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d1"), \
                        cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l1")
                    ll5 = cv.create_text(621, 124, text=f"{c[len(c) - 1]}", fill='#7CCD7C', anchor=N,
                                         font=('微软雅黑', 8, 'bold'), tags='l5')
                    arc5 = cv.create_image(621, 164, image=img, tag='d5')
                    listNodes.delete(0, END)
                    listNodes.insert(END, '| Positioning succeeded\n')
                    win.update()  # 更新界面
            elif a[len(a) - 1] == '65535' and b[len(b) - 1] == "65535":
                cv.delete("d2"), cv.delete("d3"), cv.delete("d4"), cv.delete("d5"), cv.delete("d1"), \
                cv.delete("l2"), cv.delete("l3"), cv.delete("l4"), cv.delete("l5"), cv.delete("l1")
                listNodes.delete(0, END)
                listNodes.insert(END,'| No base station is scanned\n')
                win.update()  # 更新界面
        except:
            pass
        win.after(1000,mqtt2)#定义每一秒触发一次按钮
    #可以利用这个方法找画布的像素位置
    # def handleMotion(event):
    #     print('光标当前位置', event.x, event.y)

    b1=tk.Button(win,text='Save',command=putdata,bg="#87CEFA")#按钮
    b1.place(x=610,y=20)
    b2=tk.Button(win,text='location',command=mqtt2,bg="#87CEFA")
    b2.place(x=10,y=300)
    #找画布位置
    # cv.bind('<Motion>',handleMotion)
    time1()
    win.update()#更新界面
    win.mainloop()#启动TK
if __name__=='__main__':
    tk1()
