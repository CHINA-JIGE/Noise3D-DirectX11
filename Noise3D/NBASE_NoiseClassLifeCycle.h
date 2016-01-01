
/***********************************************************************

					CLASS:  Noise Class Life Cycle

		Every Class(not Ut/Base) should be derived from this 
		class. The derived class should overload a "Destroy" function to
		do specific clear job .The SelfDestruction functions provide a
		template method to prevent one class from being destroyed
		twice.
************************************************************************/

#pragma once

//a base class that provide general life cycle management
	class _declspec(dllexport) NoiseClassLifeCycle
	{
	public:
		/*~NoiseClassLifeCycle()
		{
			SelfDestruction();
		}*/


		void	SelfDestruction()
		{
			if (mIsInitialized)
			{
				Destroy();//!!!!!!!!!!!!!!!!!!!! A DETAILED destruction must be implemented by derived class
				mIsInitialized = FALSE;
			}
		};


	protected:
		void	   SetStatusToBeInitialized()
		{
			if (mIsInitialized)
			{
				//catched by "create" functions or high level initialization
				throw(std::runtime_error("NoiseLifeCycle_ObjectInitialized"));
			}
			else
			{
				mIsInitialized = TRUE;
			}
		};

		BOOL	IsInitialized() { return mIsInitialized; };

		virtual	void	Destroy() = 0; //!!!!!!!!!!!!!!!!!!!! A DETAILED destruction must be implemented by derived class

	private:
		BOOL	mIsInitialized = FALSE;
	};
