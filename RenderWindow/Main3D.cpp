#include "RenderWindow.h"
#include <sstream>

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseMesh Mesh1;
NoiseMesh Mesh2;
NoiseMesh Mesh3;
NoiseCamera Camera;
NoiseLightManager LightMgr;
NoiseLineBuffer	lineBuffer;
NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtSlicer Slicer;
N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

float Angle;
std::vector<NVECTOR3>	 lineSegmentBuffer;
std::vector<NVECTOR3>	lineStrip;


BOOL Init3D(HWND hwnd)
{
	
	//≥ı ºªØ ß∞‹
	if(!Engine.InitD3D( hwnd,640,480,TRUE))return FALSE;

	Scene.CreateMesh(&Mesh1);
	Scene.CreateRenderer(&Renderer);
	Scene.CreateCamera(&Camera);
	Scene.CreateLightManager(&LightMgr);
	Scene.CreateLineBuffer(&lineBuffer);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Mesh1.LoadFile_STL("teapot7.STL");
	Mesh1.SetPosition(0.0f,0,0);
	//Mesh1.SetScale(0.4f, 0.4f, 0.4f);
	Camera.SetPosition(2.0f,0,0);
	Camera.SetLookAt(0,0,0);


	PointLight1.mAmbientColor = NVECTOR3(0.1f,0.1f,0.1f);
	PointLight1.mDiffuseColor	=	NVECTOR3(0.3f,1.0f,0.3f);
	PointLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	PointLight1.mPosition			=NVECTOR3(-3.0f,5.0f,-3.0f);
	PointLight1.mSpecularIntensity	=1.1f;
	PointLight1.mDiffuseIntensity = 1.0f;
	PointLight1.mAttenuationFactor = 0.0f;
	PointLight1.mLightingRange = 100.0f;


	LightMgr.AddDynamicPointLight(&PointLight1);


	N_Material Mat1;
	Mat1.mAmbientColor	= NVECTOR3(0.1f,  0.1f,0.1f);
	Mat1.mDiffuseColor		= NVECTOR3(1.0f,  1.0f, 1.0f);
	Mat1.mSpecularColor	=	NVECTOR3(1.0f, 1.0f,1.0f);
	Mat1.mSpecularSmoothLevel	=	10;

	Mesh1.SetMaterial(Mat1);


	//Noise Slicer
	Slicer.Step1_LoadPrimitiveMeshFromSTLFile("teapot7.STL");
	Slicer.Step2_Intersection(50);
	//Slicer.GetLineSegmentBuffer(lineSegmentBuffer);
	Slicer.Step3_GenerateLineStrip();
	Slicer.GetLineStrip(lineStrip,0);

	/*for (UINT i = 0;i < lineSegmentBuffer.size();i += 2)
	{
		lineBuffer.AddLine3D(lineSegmentBuffer.at(i), lineSegmentBuffer.at(i+1));
	}
	std::ostringstream s;
	s << Slicer.GetLineStripCount();
	::MessageBoxA(0,s.str().c_str(),0,0);*/

	for (UINT i = 0;i < Slicer.GetLineStripCount();i++)
	{
		
		Slicer.GetLineStrip(lineStrip, i);
		for (UINT j = 0;j < lineStrip.size() - 1;j++)
		{
			lineBuffer.AddLine3D(lineStrip.at(j), lineStrip.at(j + 1));
		}
	}

	return TRUE;
};


void MainLoop()
{
	Angle += 0.0005f;
	Camera.SetPosition(6*cos(Angle),7.0f,6*sin(Angle));
	Camera.SetLookAt(0,0,0);

	Renderer.ClearViews();

	Mesh1.AddToRenderList();
	lineBuffer.AddToRenderList();

	//Renderer.RenderMeshInList();
	Renderer.RenderLine3DInList();

	Renderer.RenderToScreen();
};


void Cleanup()
{
	Engine.ReleaseAll();
};