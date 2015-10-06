#pragma once
#include <Windows.h>
#include "Noise3D.h"
#include "NoiseUtility.h"

BOOL Init3D(HWND hwnd);
void MainLoop();
void Cleanup();
void	InputProcess();