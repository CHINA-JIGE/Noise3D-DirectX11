
/***********************************************************************

						h£ºNoiseGUI Basic Container Info


			Description : encapsule basic container information like
			position, size ..... this is a base class to be inherited.


************************************************************************/

#pragma once

namespace Noise3D
{
		//a base class that encapsulate basic information of a container 
		class /*_declspec(dllexport)*/ IBasicContainerInfo
		{
		public:
			IBasicContainerInfo();

			void  SetDiagonal(float x_topLeft, float y_topLeft, float x_bottomRight, float y_bottomRight);

			void  SetDiagonal(Vec2 v_topLeft, Vec2 v_bottomRight);

			Vec2 GetTopLeft();

			Vec2 GetBottomRight();

			void	SetCenterPos(float x, float y);

			void	SetCenterPos(Vec2 v);

			void	Move(float relativeX, float relativeY);

			void	Move(Vec2 relativePos);

			Vec2 GetCenterPos();

			void	SetWidth(float w);

			void	SetHeight(float h);

			float	GetWidth();

			float GetHeight();

			void	SetBasicColor(Vec4 c);//if texture is invalid,use basic color to draw solid rect

			Vec4 GetBasicColor();//if texture is invalid,use basic color to draw solid rect

			void	SetEnabled(bool isEnabled);

			bool IsEnabled();

			void	SetPosZ(float posZ);

			float	GetPosZ();

		protected:
			bool		 IsPointInContainer(Vec2 v);
			bool		mIsEnabled;
			Vec2	mPositionCenter;
			float		mWidth;
			float		mHeight;
			float		mPosZ;//the real "Z" coordinate of 2D triangle
			Vec4	mBasicColor;
		};
}