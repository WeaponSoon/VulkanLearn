#pragma once
#include <Windows.h>
#include <tchar.h>

class WindowsWnd
{
private:
	static bool isClassRegister;
	HWND m_hWnd;
	static const TCHAR* m_ClassName;
public:
	virtual LRESULT WndProcInner(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT (*WndProcImpl)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	WindowsWnd();
	WindowsWnd(int x, int y, int w, int h);
	
	void SetTitle(const TCHAR* name);
	void Show(int cmdShow);

	const HWND& GetWindow() const;


	WindowsWnd(const WindowsWnd&) = delete;
	WindowsWnd(WindowsWnd&&) = delete;
	WindowsWnd& operator=(const WindowsWnd&) = delete;
	WindowsWnd& operator=(WindowsWnd&&) = delete;
};