#include "RenderWindow.h"


NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseMesh Mesh1;
NoiseMesh Mesh2;
NoiseMesh Mesh3;
NoiseCamera Camera;
NoiseLightManager LightMgr;
NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);

float Angle;

N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

BOOL Init3D(HWND hwnd)
{
	
	//≥ı ºªØ ß∞‹
	if(!Engine.InitD3D( hwnd,640,480,TRUE))return FALSE;

	Scene.CreateMesh(&Mesh1);
	Scene.CreateMesh(&Mesh2);
	Scene.CreateMesh(&Mesh3);
	Scene.CreateRenderer(&Renderer);
	Scene.CreateCamera(&Camera);
	Scene.CreateLightManager(&LightMgr);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Mesh1.CreateCylinder(1.0f,2.0f,30,30);
	Mesh1.SetPosition(0,0,0);
	Mesh2.CreateSphere(2.0f,30,30);
	Mesh2.SetPosition(1.0f,0,0);
	//Mesh3.CreateBox(2.0f,2.0f,2.0f,2,2,2);
	Mesh3.LoadFile_STL("teapot5.STL");
	Mesh3.SetPosition(0.0f,0,0);
	Mesh3.SetScale(0.4f, 0.4f, 0.4f);
	Camera.SetPosition(2.0f,0,0);
	Camera.SetLookAt(0,0,0);

	DirLight1.mAmbientColor = NVECTOR3(0.1f,0.1f,0.1f);
	DirLight1.mDiffuseColor	=	NVECTOR3(1.0f,0.3f,0.3f);
	DirLight1.mDirection			=NVECTOR3(0,-1.0f,1.0);
	DirLight1.mSpecularColor	=NVECTOR3(0.5f,1.0f,0.5f);
	DirLight1.mSpecularIntensity	=1.1f;
	DirLight1.mDiffuseIntensity = 1.0f;

	PointLight1.mAmbientColor = NVECTOR3(0.1f,0.1f,0.1f);
	PointLight1.mDiffuseColor	=	NVECTOR3(0.3f,1.0f,0.3f);
	PointLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	PointLight1.mPosition			=NVECTOR3(-3.0f,5.0f,-3.0f);
	PointLight1.mSpecularIntensity	=1.1f;
	PointLight1.mDiffuseIntensity = 1.0f;
	PointLight1.mAttenuationFactor = 0.0f;
	PointLight1.mLightingRange = 100.0f;

	SpotLight1.mAmbientColor = NVECTOR3(0.1f,0.1f,0.1f);
	SpotLight1.mDiffuseColor	=	NVECTOR3(0.3f,0.3f,1.0f);
	SpotLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	SpotLight1.mPosition			=NVECTOR3(3.0f,5.0f,3.0f);
	SpotLight1.mLitAt					=	NVECTOR3(0,0,0);
	SpotLight1.mSpecularIntensity	=1.1f;
	SpotLight1.mDiffuseIntensity = 1.0f;
	SpotLight1.mAttenuationFactor = 0.01f;
	SpotLight1.mLightingRange = 100.0f;
	SpotLight1.mLightingAngle	= MATH_PI /12;


	//LightMgr.AddDynamicDirLight(&DirLight1);
	LightMgr.AddDynamicPointLight(&PointLight1);
	//LightMgr.AddDynamicSpotLight(&SpotLight1);


	N_Material Mat1;
	Mat1.mAmbientColor	= NVECTOR3(0.1f,  0.1f,0.1f);
	Mat1.mDiffuseColor		= NVECTOR3(1.0f,  1.0f, 1.0f);
	Mat1.mSpecularColor	=	NVECTOR3(1.0f, 1.0f,1.0f);
	Mat1.mSpecularSmoothLevel	=	10;

	Mesh1.SetMaterial(Mat1);
	Mesh2.SetMaterial(Mat1);
	Mesh3.SetMaterial(Mat1);

	return TRUE;
};

void MainLoop()
{
	Angle += 0.0005f;
	Camera.SetPosition(5*cos(Angle),3.0f,5*sin(Angle));
	Camera.SetLookAt(0,0,0);

	Renderer.ClearViews();

	//Mesh1.AddToRenderList();
	//Mesh2.AddToRenderList();
	Mesh3.AddToRenderList();

	Renderer.Draw_CoordinateFrame(5.0f);

	Renderer.RenderMeshInList();

	Renderer.RenderToScreen();
};

void Cleanup()
{
	Engine.ReleaseAll();
};