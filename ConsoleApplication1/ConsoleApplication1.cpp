// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "Driver.h"
#include <locale>
#include <TlHelp32.h>
#include <chrono>


uint64_t GetProcessId(const wchar_t * name)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W process{ 0 };
	uint64_t pid = 0;
	process.dwSize = sizeof(process);
	while (Process32NextW(hSnap, &process))
	{
		if (wcscmp(process.szExeFile, name) == 0)
		{
			pid = process.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnap);
	return pid;
}

int main()
{
	uint64_t pid = 0;
	setlocale(LC_ALL, "");
	SetConsoleTitleA("DOSBox");
	pid = GetProcessId(L"steam.exe");
	printf("PID: %d\n", pid);
	
	Driver driver;
	driver.Init(pid);
	uint64_t address = driver.GetModuleAddress32(L"steam.exe");
	printf("地址: %p\n", address);
	
	printf("读取: %llx\n", driver.Read<uint64_t>(address));
	//driver.Write<char>(0x11000000,0x66);
	//getchar();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
