#pragma once

#include <easyx.h>
#include <string>

const int type = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
const RECT r = { 0,0,100,100 };;

using namespace std;

inline int RND(int a, int b) {
    if (a > b) {
        a = a ^ b;
        b = a ^ b;
        a = a ^ b;
        //sawp(a,b)
    }
    return (rand() % (b - a + 1)) + a;
}

inline string num_to_string(long long __num) {
    string numstring="";
    while (__num >= 1) {
        numstring += (__num % 10) + '0';
        __num /= 10;
    }
    size_t _string_len_ = numstring.size();
    for (size_t i = 0; i < _string_len_/2; ++i) {
        swap(numstring[i], numstring[_string_len_ - 1 - i]);
    }
    return numstring;
}




inline void drawstring(string _str, RECT* _pRect, UINT _uFormat) {
    char* _temp{}; _temp = new char[_str.size() + 1];
    int _strlen = _str.size(), i(0);
    for (i; i <= _strlen; ++i) {
        _temp[i] = _str[i];
    }
    //将string拆成char
    size_t _TCHAR_len = MultiByteToWideChar(CP_ACP, 0, _temp, strlen(_temp), NULL, 0);
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _temp, strlen(_temp), _Tstr, _TCHAR_len);
    delete[]_temp; _temp = NULL;//_temp:我滴任务，完成啦，啊哈哈哈哈哈哈哈(突发恶疾)
    _Tstr[_TCHAR_len] = '\0';
    //将char转换成TCHAR
    drawtext(_Tstr, _pRect, _uFormat);//输出
    delete[]_Tstr; _Tstr = NULL;//释放内存
    return;
}//在指定位置(变量_pRECT)输出指定格式(变量_uFormat)的字符串(变量_str)
//成功解决了使用原版EasyX不能直接输出string的问题

inline void outstringxy(int _out_x,int _out_y,string _str) {
    char* _temp{}; _temp= new char[_str.size() + 1];
    int _strlen = _str.size(), i(0);
    for (i; i <= _strlen; ++i) {
        _temp[i] = _str[i];
    }
    //将string拆成char
    size_t _TCHAR_len = MultiByteToWideChar(CP_ACP, 0, _temp, strlen(_temp), NULL, 0);
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _temp, strlen(_temp), _Tstr, _TCHAR_len);
    delete[]_temp; _temp = NULL;//_temp:我滴任务，完成啦，啊哈哈哈哈哈哈哈(突发恶疾)
    _Tstr[_TCHAR_len] = '\0';
    //将char转换成TCHAR
    outtextxy(_out_x, _out_y, _Tstr);//输出
    delete[]_Tstr; _Tstr = NULL;//释放内存
    return;
}//在指定位置(变量_out_x,_out_y)输出字符串(变量_str)
//成功解决了使用原版EasyX不能直接输出string的问题

inline void drawnum(long long __num, RECT* _pRect, UINT _uFormat) {
    long long _numstring_tmp_ = __num;//_numstring_tmp_赋值成__num,用于运算__num的位数
    unsigned short num_len(0);
    while (_numstring_tmp_ >= 1) {
        _numstring_tmp_ /= 10;
        ++num_len;
    }
    char* _charstring_ = new char[num_len];
    _numstring_tmp_ = 0;//现在_numstring_tmp_成为_charstring_的指针
    while (__num > 0) {
        _charstring_[_numstring_tmp_] = (__num % 10) + '0';
        __num /= 10;
        ++_numstring_tmp_;
    }
    for (unsigned short i = 0; i < num_len/2; ++i) {
        swap(_charstring_[i], _charstring_[num_len - 1 - i]);
    }
    //以上是把数字转换成char
    size_t _TCHAR_len = num_len;
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _charstring_, strlen(_charstring_), _Tstr, _TCHAR_len);
    delete[]_charstring_; _charstring_ = NULL;
    _Tstr[_TCHAR_len] = '\0';
    //将char转换成TCHAR
    drawtext(_Tstr, _pRect, _uFormat);//输出
    delete[]_Tstr; _Tstr = NULL;//释放内存
    return;
}


inline void outnumxy(int _outx,int _outy,long long __num) {
    long long _numstring_tmp_ = __num;//_numstring_tmp_赋值成__num,用于运算__num的位数
    unsigned short num_len(0);
    while (_numstring_tmp_ >= 1) {
        _numstring_tmp_ /= 10;
        ++num_len;
    }
    char* _charstring_ = new char[num_len];
    _numstring_tmp_ = 0;//现在_numstring_tmp_成为_charstring_的指针
    while (__num > 0) {
        _charstring_[_numstring_tmp_] = (__num % 10) + '0';
        __num /= 10;
        ++_numstring_tmp_;
    }
    for (unsigned short i = 0; i < num_len / 2; ++i) {
        swap(_charstring_[i], _charstring_[num_len - 1 - i]);
    }
    //以上是把数字转换成char
    size_t _TCHAR_len = num_len;
    TCHAR* _Tstr = new TCHAR[_TCHAR_len + 1];
    MultiByteToWideChar(0, 0, _charstring_, strlen(_charstring_), _Tstr, _TCHAR_len);
    delete[]_charstring_; _charstring_ = NULL;
    _Tstr[_TCHAR_len] = '\0';
    //将char转换成TCHAR
    outtextxy(_outx,_outy,_Tstr);//输出
    delete[]_Tstr; _Tstr = NULL;//释放内存
    return;
}
