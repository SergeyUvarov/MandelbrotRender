#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <thread>
#include <iostream>
#include "Photo.hpp"

#define MAX_LOADSTRING 100

void displayFrames(HWND hWnd, Photo* canvas, void (*renderFrameFunc)(HWND, Photo*, bool*), bool *workStat);

void def(int32_t, int32_t);
void def1(int32_t);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class WindowWrapper
{
public:
	WindowWrapper(const WCHAR* szTitle, void (*renderFrameFunc)(HWND, Photo*, bool*), void (*mouseClickFunction)(int32_t, int32_t) = def, void (*mouseWheelFunction)(int32_t) = def1, void (*childCommand)(int32_t) = def1);
	~WindowWrapper();

	void run();

	HWND getHwnd();
	
protected:
	HWND _hWnd;
	HINSTANCE _hInstance;
	WCHAR _wndClassName[MAX_LOADSTRING];
	Photo _canvas;

	std::thread _displayThread;
	std::thread _renderThread;
	bool _workStat;
};