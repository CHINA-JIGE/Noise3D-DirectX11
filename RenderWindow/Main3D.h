#pragma once
#include <Windows.h>
#include "Noise3D.h"

const TCHAR AppName[] = "Render Window" ; //窗体名
const TCHAR WindowClassName[] = "RENDER";//"窗体类名

//窗体：
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM); //声名消息处理函数(处理windows和接收windows消息)
BOOL InitWindowClass(WNDCLASS* wc,HINSTANCE hInstance);
HWND InitWindow(HINSTANCE hInstance);

//3D：
BOOL Init3D(HWND hwnd);
void MainLoop();
void Cleanup();