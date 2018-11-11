
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
	class /*_declspec(dllexport)*/ SweepingTrailManager :
		public IFactory<SweepingTrail>
	{
	public:

		SweepingTrail*			CreateSweepingTrail(N_UID objName, uint32_t maxVertexCount);

		SweepingTrail*			GetSweepingTrail(N_UID objName);

		SweepingTrail*			GetSweepingTrail(UINT index);

		bool				DestroySweepingTrail(N_UID objName);

		bool				DestroySweepingTrail(SweepingTrail* pST);

		void				DestroyAllSweepingTrail();

		UINT			GetSweepingTrailCount();

	private:

		friend  class SceneManager;

		friend IFactory<SweepingTrailManager>;

		SweepingTrailManager();

		~SweepingTrailManager();
	};
}