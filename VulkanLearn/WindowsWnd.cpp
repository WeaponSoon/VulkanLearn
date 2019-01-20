#include "pch.h"
#include "WindowsWnd.h"
#include <mutex>
#include <iostream>
static std::mutex lock;
bool WindowsWnd::isClassRegister = false;
const TCHAR* WindowsWnd::m_ClassName = _T("VK_LEARN_WND_CLASS");
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR obj = GetWindowLongPtr(hWnd, 0);
	if (obj == 0)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return ((WindowsWnd*)obj)->WndProcInner(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWnd::WndProcInner(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (WndProcImpl != nullptr)
	{
		return WndProcImpl(hWnd, msg, wParam, lParam);
	}
	
	switch (msg)
	{
	case WM_DESTROY:
		std::cout << "Quit" << std::endl;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return LRESULT();
}

WindowsWnd::WindowsWnd() : WndProcImpl(nullptr)
{
	auto m_hInstance = GetModuleHandle(NULL);
	if (!isClassRegister)
	{
		lock.lock();
		if (!isClassRegister)
		{
			WNDCLASS wndClass;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 8;
			wndClass.style = 0;
			wndClass.hInstance = m_hInstance;
			wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wndClass.lpszClassName = m_ClassName;
			wndClass.lpszMenuName = NULL;
			wndClass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
			wndClass.lpfnWndProc = WndProc;
			isClassRegister = true;
			RegisterClass(&wndClass);
		}
		lock.unlock();
	}
	
	m_hWnd = CreateWindow(m_ClassName, _T("VkLearnWnd"), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, m_hInstance, NULL);
	LONG_PTR obj = (LONG_PTR)this;
	SetWindowLongPtr(m_hWnd, 0, obj);
}

WindowsWnd::WindowsWnd(int x, int y, int w, int h) : WndProcImpl(nullptr)
{
	
	auto m_hInstance = GetModuleHandle(NULL);
	if (!isClassRegister)
	{
		lock.lock();
		if (!isClassRegister)
		{
			WNDCLASS wndClass;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 8;
			wndClass.style = 0;
			wndClass.hInstance = m_hInstance;
			wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wndClass.lpszClassName = m_ClassName;
			wndClass.lpszMenuName = NULL;
			wndClass.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
			wndClass.lpfnWndProc = WndProc;
			isClassRegister = true;
			RegisterClass(&wndClass);
			auto s = GetLastError();
			s = s;
		}
		lock.unlock();
	}

	
	m_hWnd = CreateWindow(m_ClassName, _T("VkLearnWnd"), WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, m_hInstance, NULL);
	auto lastError=  GetLastError();
	LONG_PTR obj = (LONG_PTR)this;
	SetWindowLongPtr(m_hWnd, 0, obj);
}

void WindowsWnd::SetTitle(const TCHAR * name)
{
	SetWindowText(m_hWnd, name);
}

void WindowsWnd::Show(int cmdShow)
{
	ShowWindow(m_hWnd, cmdShow);
	UpdateWindow(m_hWnd);
}

const HWND & WindowsWnd::GetWindow() const
{
	return m_hWnd;
	// TODO: 在此处插入 return 语句
}
