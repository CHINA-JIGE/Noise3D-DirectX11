
/***********************************************************************

							h：标志

***********************************************************************/

#pragma once

enum NOISE_FILLMODE
{
	NOISE_FILLMODE_SOLID = D3D11_FILL_SOLID,
	NOISE_FILLMODE_WIREFRAME = D3D11_FILL_WIREFRAME,
};

enum NOISE_VERTEX_TYPE
{
	NOISE_VERTEX_TYPE_SIMPLE =0,
	NOISE_VERTEX_TYPE_DEFAULT =1,
};

//灯光类型
enum NOISE_LIGHT_TYPE
{
	NOISE_LIGHT_TYPE_DIRECTIONAL = 0,
	NOISE_LIGHT_TYPE_POINT = 1,
	NOISE_LIGHT_TYPE_SPOT = 2,
};

//NoiseTimer的时间单位
enum NOISE_TIMER_TIMEUNIT
{
	NOISE_TIMER_TIMEUNIT_MILLISECOND = 0,
	NOISE_TIMER_TIMEUNIT_SECOND = 1,
};
