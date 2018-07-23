
/***********************************************************************

								h£ºSweepingTrailManager


		Desc: Sweeping Trail is an special effect that uses
		a sequence of triangle strip to simulate/visualize a
		path that a line segment sweep through 3D spaces.

		picture ref:https://blog.csdn.net/cbbbc/article/details/70863580
************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ ISweepingTrailManager :
		public IFactory<ISweepingTrail>
	{
	public:

		ISweepingTrail*			CreateSweepingTrail(N_UID objName, uint32_t vertexPoolSize);

		ISweepingTrail*			GetSweepingTrail(N_UID objName);

		ISweepingTrail*			GetSweepingTrail(UINT index);

		bool				DestroySweepingTrail(N_UID objName);

		bool				DestroySweepingTrail(ISweepingTrail* pST);

		void				DestroyAllSweepingTrail();

		UINT			GetSweepingTrailCount();

	private:

		friend  class IScene;

		friend IFactory<ISweepingTrailManager>;

		ISweepingTrailManager();

		~ISweepingTrailManager();
	};
}