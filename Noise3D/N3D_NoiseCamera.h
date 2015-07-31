
/***********************************************************************

                           h：NoiseCamera

************************************************************************/

#pragma once

public class _declspec(dllexport) NoiseCamera
{
public:

	friend class NoiseScene;//scene是他的友元类
	friend class NoiseRenderer;

	NoiseCamera();
	~NoiseCamera();

	void				SetLookAt(NVECTOR3* vLookat);//要更新旋转角
	void				SetLookAt(float x,float y,float z);//要更新旋转角
	void				GetLookAt(NVECTOR3& out_vLookat);

	void				SetPosition(NVECTOR3* vPos);
	void				SetPosition(float x,float y,float z);
	void				GetPosition(NVECTOR3& vPos);
	void				Move(NVECTOR3* vRelativePos);
	void				Move(float relativeX,float relativeY,float relativeZ);

	void				SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ);
	void				SetViewAngle(float iViewAngleY,float iAspectRatio);

	void				SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll);//要更新Lookat
	void				SetRotationY_Yaw(float AngleX);//用setRotation呗
	void				SetRotationX_Pitch(float AngleY);
	void				SetRotationZ_Roll(float AngleZ);

/*
	void				fps_Walk();
	void				fps_Strafe();

	void				sm_Update();
	void				sm_LinearMoveTo();
	void				sm_SineMoveTo();
	void				sm_RotateAroundAxis();*/


private:
	NoiseScene*			m_pFatherScene;

	void						mFunction_UpdateProjMatrix();
	void						mFunction_UpdateViewMatrix();
	void						mFunction_UpdateRotation();
	void						mFunction_UpdateDirection();
	float						mViewAngleY;
	float						mAspectRatio;
	float						mNearPlane;
	float						mFarPlane;
	NVECTOR3*			m_pPosition;
	NVECTOR3*			m_pLookat;
	NVECTOR3*			m_pDirection;

	NMATRIX*			m_pMatrixView;
	NMATRIX*			m_pMatrixProjection;

	float						mRotateX_Pitch;
	float						mRotateY_Yaw;
	float						mRotateZ_Roll;

};




