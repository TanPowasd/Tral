#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include"base.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

struct SETTINGS {
    size_t SCREEN_HEIGHT=500;
    size_t SCREEN_WIDTH=600;
    
}Settings;

struct LuckIndexButton {
    int LeftUp_x;
    int LeftUp_y;
    unsigned int ButtonHeight;
    unsigned int ButtonWidth;
    int ButtonColour;

};


unsigned int LuckIndex() {//今日运势
    // 获取当前时间并转换为本地时间
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);

    // 提取年月日并生成唯一种子
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    int todaySeed = year * 10000 + month * 100 + day;

    // 设置随机种子并生成0-100的随机数
    srand(todaySeed);
    int luckIndex = rand() % 101;
    return luckIndex;
}

inline void Message_LuckIndex() {
    unsigned int MyLuck = LuckIndex();
    string MyMessage = "你今天的人品值为:" + to_string(MyLuck)+"! ";

    if (MyLuck==0) {
        MyMessage += "?";
    }else if (MyLuck < 10) {
        MyMessage += "? (提示:是百分制哦)。";
    }else if (MyLuck < 30) {
        MyMessage += "那很有生活了。";
    }
    else if (MyLuck < 60) {
        MyMessage += "还行拉,还行拉。";
    }else if(MyLuck < 80){
        MyMessage += "好运正在路上";
    }
    else {
        MyMessage += "天选之日！";
    }

    char* _temp{}; _temp = new char[MyMessage.size() + 1];
    int _strlen = MyMessage.size(), i(0);
    for (i; i <= _strlen; ++i) {
        _temp[i] = MyMessage[i];
    }
    //将string拆成char
    size_t _TCHAR_len = MultiByteToWideChar(CP_ACP, 0, _temp, strlen(_temp), NULL, 0);
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _temp, strlen(_temp), _Tstr, _TCHAR_len);
    delete[]_temp; _temp = NULL;//_temp:我滴任务，完成啦，啊哈哈哈哈哈哈哈(突发恶疾)
    _Tstr[_TCHAR_len] = '\0';

    MessageBox(NULL, _Tstr, TEXT("今日人品"), MB_OK | MB_ICONQUESTION);
}