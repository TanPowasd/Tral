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


unsigned int LuckIndex() {//��������
    // ��ȡ��ǰʱ�䲢ת��Ϊ����ʱ��
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);

    // ��ȡ�����ղ�����Ψһ����
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    int todaySeed = year * 10000 + month * 100 + day;

    // ����������Ӳ�����0-100�������
    srand(todaySeed);
    int luckIndex = rand() % 101;
    return luckIndex;
}

inline void Message_LuckIndex() {
    unsigned int MyLuck = LuckIndex();
    string MyMessage = "��������ƷֵΪ:" + to_string(MyLuck)+"! ";

    if (MyLuck==0) {
        MyMessage += "?";
    }else if (MyLuck < 10) {
        MyMessage += "? (��ʾ:�ǰٷ���Ŷ)��";
    }else if (MyLuck < 30) {
        MyMessage += "�Ǻ��������ˡ�";
    }
    else if (MyLuck < 60) {
        MyMessage += "������,��������";
    }else if(MyLuck < 80){
        MyMessage += "��������·��";
    }
    else {
        MyMessage += "��ѡ֮�գ�";
    }

    char* _temp{}; _temp = new char[MyMessage.size() + 1];
    int _strlen = MyMessage.size(), i(0);
    for (i; i <= _strlen; ++i) {
        _temp[i] = MyMessage[i];
    }
    //��string���char
    size_t _TCHAR_len = MultiByteToWideChar(CP_ACP, 0, _temp, strlen(_temp), NULL, 0);
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _temp, strlen(_temp), _Tstr, _TCHAR_len);
    delete[]_temp; _temp = NULL;//_temp:�ҵ����������������������������(ͻ����)
    _Tstr[_TCHAR_len] = '\0';

    MessageBox(NULL, _Tstr, TEXT("������Ʒ"), MB_OK | MB_ICONQUESTION);
}