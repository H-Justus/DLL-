// loadDll.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
using namespace std;

typedef void (*FUNC)(const char *a, const char *b, const char *c);
int main()
{
    HMODULE hModule = LoadLibrary(L"Export.dll");
    if (hModule == NULL) {
        MessageBox(NULL, L"Dll 加载失败", L"Export.dll", MB_OK);
    }
    else {
        FUNC dllFunc = (FUNC)::GetProcAddress(hModule, "fnExport");
        const char* a = "C:\\Users\\Administrator\\Documents\\Tencent Files\\1587836884\\FileRecv\\图像样例\\";
        const char* b = "bbbbb";
        const char* c = "C:\\Users\\Administrator\\source\\repos\\Dll1\\处理结果\\";
        dllFunc(a,b,c);
    }
    return 0;
}
