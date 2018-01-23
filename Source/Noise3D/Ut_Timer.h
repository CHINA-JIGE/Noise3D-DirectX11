
/***********************************************************************

                           h：NoiseTimer

************************************************************************/

#pragma once


namespace Noise3D
{
	namespace Ut
	{
		enum NOISE_TIMER_TIMEUNIT
		{
			NOISE_TIMER_TIMEUNIT_MILLISECOND = 0,
			NOISE_TIMER_TIMEUNIT_SECOND = 1
		};

		class /*_declspec(dllexport)*/ ITimer
		{
		public:

			ITimer();

			ITimer(NOISE_TIMER_TIMEUNIT timeUnit);

			//在函数名后加const表示此函数不会修改类成员
			//暂停后不再加
			double GetTotalTimeElapsed()const;

			//获取间隔时间
			double GetInterval()const;

			//设置时间单位
			void SetTimeUnit(NOISE_TIMER_TIMEUNIT timeUnit = NOISE_TIMER_TIMEUNIT_MILLISECOND);

			//进入下一帧
			void NextTick();

			//....
			UINT GetFPS() const;

			//暂停计时
			void Pause();

			void Continue();

			//所有数据重置
			void ResetAll();

			void ResetTotalTime();

			void	SetTimeIntervalClamp(double maxInterval);

			void	SetTimeScale(double scaleFactor);

			double GetTimeScale() const ;

		private:
			//更新后的总时间(ms)
			double					mTotalTime;
			//就是INTERVAL 两帧间的时间间隔
			double					mDeltaTime;
			//how many MS does one COUNT takes
			double					mMilliSecondsPerCount;
			//how many ticks are in current second
			UINT						mCurrentSecondTickCount;
			//a rounded total time
			UINT						mCurrentSecondInteger;
			//frames per second
			UINT						mFPS;

			//时间制
			NOISE_TIMER_TIMEUNIT	 mTimeUnit;
			//
			bool					mIsPaused;
			//queryPerformance貌似返回的是绝对count而不是两次query间的count
			INT64					mPrevCount;
			//
			INT64					mCurrentCount;
			//
			INT64					mDeltaCount;
			//max time interval, it means that GetInterval will be CLAMPed;
			double						mMaxInterval;
			//time scale factor, which will be multiplied in Time Getter function
			double						mTimeScaleFactor;
		};
	}
}