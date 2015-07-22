
/***********************************************************************

                           h£ºNoiseEngine

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseEngine
{
public:

	//¹¹Ôìº¯Êý
	NoiseEngine();

	BOOL	InitD3D(HWND RenderHWND,UINT BufferWidth,UINT BufferHeight,BOOL IsWindowed);

	void	ReleaseAll();

};

