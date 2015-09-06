

#include "Main3D.h"
#include <sstream>

NoiseEngine Engine;
NoiseRenderer Renderer;
NoiseScene Scene;
NoiseMesh Mesh1;
NoiseCamera Camera;
NoiseLightManager LightMgr;
NoiseMaterialManager	MatMgr;
NoiseTextureManager	TexMgr;

NoiseGraphicObject	GraphicObjBuffer;
NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtSlicer Slicer;
N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

float Angle;
std::vector<NVECTOR3>	 lineSegmentBuffer;
std::vector<N_LineStrip>	lineStrip;


//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = Engine.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	Init3D(windowHWND);
	Engine.SetMainLoopFunction(MainLoop);
	Engine.Mainloop();
	Cleanup();
	return 0;
}



BOOL Init3D(HWND hwnd)
{
	
	//³õÊ¼»¯Ê§°Ü
	if(!Engine.InitD3D( hwnd,640,480,TRUE))return FALSE;

	Scene.CreateMesh(&Mesh1);
	Scene.CreateRenderer(&Renderer);
	Scene.CreateCamera(&Camera);
	Scene.CreateLightManager(&LightMgr);
	Scene.CreateGraphicObject(&GraphicObjBuffer);
	Scene.CreateMaterialManager(&MatMgr);
	Scene.CreateTextureManager(&TexMgr);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_BACK);
	//Mesh1.LoadFile_STL("table.STL");
	//Mesh1.CreateBox(5.0f,5.0f,5.0f,5,5,5);
	Mesh1.CreateSphere(6.0, 30, 30);
	Mesh1.SetPosition(0,0,0);
	Mesh1.SetScale(0.6f, 0.6f, 0.6f);
	Camera.SetPosition(2.0f,0,0);
	Camera.SetLookAt(0,0,0);

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	PointLight1.mAmbientColor = NVECTOR3(1.0f,1.0f,1.0f);
	PointLight1.mDiffuseColor	=	NVECTOR3(1.0f,1.0f,1.0f);
	PointLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	PointLight1.mPosition			=NVECTOR3(-3.0f,5.0f,-3.0f);
	PointLight1.mSpecularIntensity	=1.1f;
	PointLight1.mDiffuseIntensity = 1.2f;
	PointLight1.mAttenuationFactor = 0.0f;
	PointLight1.mLightingRange = 100.0f;
	LightMgr.AddDynamicPointLight(&PointLight1);


	//Âþ·´ÉäÌùÍ¼
	TexMgr.CreateTextureFromFile(L"texture.jpg", "Earth", TRUE, 0, 0);
	TexMgr.CreateTextureFromFile(L"texture2.jpg", "Wood", TRUE, 0, 0);


	N_Material Mat1;
	Mat1.baseColor.mBaseAmbientColor	= NVECTOR3(0.1f,  0.1f,0.1f);
	Mat1.baseColor.mBaseDiffuseColor		= NVECTOR3(1.0f,  1.0f, 1.0f);
	Mat1.baseColor.mBaseSpecularColor	=	NVECTOR3(1.0f, 1.0f,1.0f);
	Mat1.baseColor.mSpecularSmoothLevel	=	20;
	Mat1.diffuseMapID = TexMgr.GetIndexByName("Earth");
	UINT	 Mat1_ID = MatMgr.CreateMaterial(Mat1);

	//set material
	Mesh1.SetMaterial(Mat1_ID);


	/*Slicer.Step1_LoadPrimitiveMeshFromSTLFile("table.STL");
	Slicer.Step2_Intersection(50);
	Slicer.Step3_GenerateLineStrip();*/
	//Slicer.Step4_SaveLayerDataToFile("table.NOISELAYER");
	//Slicer.Step3_LoadLineStripsFrom_NOISELAYER_File("table.NOISELAYER");


	/*NVECTOR3 v1, v2,n;
	for (UINT i = 0;i < Slicer.GetLineStripCount();i++)
	{
		
		Slicer.GetLineStrip(lineStrip, i);
		for (UINT j = 0;j < lineStrip.at(i).pointList.size() - 1;j++)
		{
			v1 =	lineStrip.at(i).pointList.at(j);
			v2 =	lineStrip.at(i).pointList.at(j + 1);
			n =	lineStrip.at(i).normalList.at(j);
			//vertex
			GraphicObjBuffer.AddLine3D(v1, v2);
			//normal
			GraphicObjBuffer.AddLine3D((v1+v2)/2 , ((v1 + v2) / 2)+n,NVECTOR4(1.0f,1.0f,1.0f,1.0f), NVECTOR4(1.0f, 0.2f, 0.2f, 1.0f));
		}
	}*/
	GraphicObjBuffer.AddLine2D(NVECTOR2(-1.0f, 0), NVECTOR2(1.0f, 0));
	GraphicObjBuffer.AddLine2D(NVECTOR2(0, 1.0f), NVECTOR2(0,-1.0f));
	GraphicObjBuffer.AddPoint2D(NVECTOR2(0.3f, 0.3f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	GraphicObjBuffer.AddPoint2D(NVECTOR2(0.3f, 0.4f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	GraphicObjBuffer.AddPoint2D(NVECTOR2(0.3f, 0.5f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	UINT myTexID = TexMgr.GetIndexByName("Earth");
	GraphicObjBuffer.AddRectangle(NVECTOR2(-0.7f, -0.7f), NVECTOR2(-0.5f, -0.5f), NVECTOR4(1.0f,0,0,1.0f), myTexID);
	GraphicObjBuffer.AddRectangleOutline(NVECTOR2(-0.7f, -0.7f), NVECTOR2(-0.5f, -0.5f), NVECTOR4(0, 0, 0, 1.0f));
	GraphicObjBuffer.AddEllipse(0.3f, 0.3f, NVECTOR2(0.6f, 0.6f), NVECTOR4(0, 1.0f, 0, 1.0f), 20, myTexID);
	GraphicObjBuffer.AddEllipseOutline(0.3f, 0.3f, NVECTOR2(0.6f, 0.6f), NVECTOR4(0, 0, 0, 1.0f), 20);
	return TRUE;
};


void MainLoop()
{
	Angle += 0.0005f;
	Camera.SetPosition(7.0f*cos(Angle),5.0f,7.0f*sin(Angle));
	Camera.SetLookAt(0,0,0);
	UINT textureID = TexMgr.GetIndexByName("Wood");
	GraphicObjBuffer.SetRectangle(0, NVECTOR2(sin(Angle), cos(Angle)), NVECTOR2(-0.5f + sin(Angle), -0.5f + cos(Angle)), NVECTOR4(0.3f, 0.3f, 0.9f, 0.5f), textureID);
	GraphicObjBuffer.SetRectangleOutline(0, NVECTOR2(sin(Angle), cos(Angle)), NVECTOR2(-0.5f + sin(Angle), -0.5f + cos(Angle)), NVECTOR4(0, 0, 0, 1.0f));
	GraphicObjBuffer.SetEllipse(0, 0.3f, 0.3f, NVECTOR2(sin(-Angle), cos(-Angle)), NVECTOR4(0, 1.0f, 0, 0.5f), textureID);
	GraphicObjBuffer.SetEllipseOutline(0, 0.3f, 0.3f, NVECTOR2(sin(-Angle), cos(-Angle)), NVECTOR4(0,0, 0, 1.0f));


	Renderer.ClearBackground();

	//add to render list
	Mesh1.AddToRenderList();
	GraphicObjBuffer.AddToRenderList();

	//render
	Renderer.RenderMeshInList();
	Renderer.RenderGraphicObjectInList();

	//present
	Renderer.RenderToScreen();
};


void Cleanup()
{
	Engine.ReleaseAll();
	Scene.ReleaseAllChildObject();
};