#pragma once
#include <Windows.h>
#include "Noise3D.h"
#include "NoiseGUISystem.h"

BOOL Init3D(HWND hwnd);
void MainLoop();
void Cleanup();
void	InputProcess();
UINT Button1MsgProc(UINT msg);