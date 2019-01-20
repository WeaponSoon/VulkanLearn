// VulkanLearn.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "VulkanCore.h"
LRESULT Test1Func(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Test2Func(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int main()
{
	CORE->ShowGlobalLayersAndExtensions();
	std::cout << std::endl;
	std::cout << std::endl;
	CORE->ShowPhysicsDevicesInfo();
	CORE->ShowWindow();
	
	WindowsWnd test1;
	test1.SetTitle(_T("test1"));
	test1.WndProcImpl = Test1Func;
	test1.Show(SW_SHOW);

	WindowsWnd test2;
	test2.SetTitle(_T("test2"));
	test2.WndProcImpl = Test2Func;
	test2.Show(SW_SHOW);

	
	CORE->Run();
	//getchar();
	return 0;
}

LRESULT Test1Func(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		std::cout << "Test1 Quit" << std::endl;
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return LRESULT();
}
LRESULT Test2Func(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		std::cout << "Test2 Quit" << std::endl;
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return LRESULT();
}