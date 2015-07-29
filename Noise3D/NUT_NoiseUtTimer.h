
/***********************************************************************

                           h：NoiseTimer

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseUtTimer
{
public:

	//构造函数
	NoiseUtTimer(NOISE_TIMER_TIMEUNIT timeUnit);

	//在函数名后加const表示此函数不会修改类成员
	//暂停后不再加
	double GetTotalTime()const;

	//获取间隔时间
	double GetInterval()const;

	//设置时间单位
	void SetTimeUnit(NOISE_TIMER_TIMEUNIT timeUnit);

	//进入下一帧
	void Next();

	//暂停计时
	void Pause();

	void Continue();	

	//所有数据重置
	void ResetAll();

	void ResetTotalTime();

private:
	//更新后的总时间
	double					mTotalTime;
	//就是INTERVAL 两帧间的时间间隔
	double					mDeltaTime;				
	//每一count占了多少毫秒
	double					mMilliSecondsPerCount;	
	//时间制
	NOISE_TIMER_TIMEUNIT	mTimeUnit; 
	//
	BOOL					mIsPaused;
	//queryPerformance貌似返回的是绝对count而不是两次query间的count
	INT64					mPrevCount	;		
	//
	INT64					mCurrentCount;
	//
	INT64					mDeltaCount;
};
