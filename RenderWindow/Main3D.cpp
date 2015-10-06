

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
NoiseAtmosphere			Atmos;

NoiseGraphicObject	GraphicObjBuffer;
NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtSlicer Slicer;
NoiseUtInputEngine inputE;
N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

float moveForwardVelocity;
std::vector<NVECTOR3>	 lineSegmentBuffer;
std::vector<N_LineStrip>	lineStrip;




//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = Engine.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	inputE.Initialize(hInstance, windowHWND);
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
	Scene.CreateAtmosphere(&Atmos);

	//Âþ·´ÉäÌùÍ¼
	//TexMgr.CreateTextureFromFile(L"Earth.jpg", "Earth", TRUE, 0, 0);
	//TexMgr.CreateTextureFromFile(L"texture2.jpg", "Wood", TRUE, 0, 0);
	//TexMgr.CreateTextureFromFile(L"button.dds", "Button", TRUE, 0, 0);
	//TexMgr.CreateTextureFromFile(L"planet.png", "Planet", TRUE, 0, 0);
	TexMgr.CreateTextureFromFile(L"universe.jpg", "Universe", TRUE, 0, 0);
	TexMgr.CreateTextureFromFile(L"bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0);
	//TexMgr.CreateTextureFromFile(L"Earth_Bump.bmp", "NormalMap_Earth", TRUE, 0, 0);
	//TexMgr.CreateCubeMapFromDDS(L"UniverseEnv.dds", "EnvironmentMap",NOISE_CUBEMAP_SIZE_256x256);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_NONE);//NOISE_CULLMODE_BACK

	Mesh1.LoadFile_STL("object.stl");
	Mesh1.SetPosition(0,0,0);

	Camera.SetPosition(2.0f,0,0);
	Camera.SetLookAt(0,0,0);
	Camera.SetViewAngle(MATH_PI / 2.0f, 1.333333333f);
	Camera.SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	NVECTOR3 AABB_MAX = Mesh1.GetBoundingBoxMax();
	float rotateRadius = sqrtf(AABB_MAX.x*AABB_MAX.x + AABB_MAX.z*AABB_MAX.z)*1.2f;
	float rotateY = Mesh1.GetBoundingBoxMax().y*1.3f;
	Camera.SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	Camera.SetLookAt(0, rotateY / 2, 0);


	Atmos.SetFogEnabled(FALSE);
	Atmos.SetFogParameter(7.0f, 8.0f, NVECTOR3(0, 0, 1.0f));
	Atmos.CreateSkyDome(4.0f, 4.0f, TexMgr.GetIndexByName("Universe"));
	//Atmos.CreateSkyBox(10.0f, 10.0f, 10.0f, TexMgr.GetIndexByName("EnvironmentMap"));

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	/*DirLight1.mAmbientColor = NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mDiffuseColor	=	NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mSpecularColor	=NVECTOR3(1.0f,1.0f,1.0f);
	DirLight1.mDirection = NVECTOR3(0.0f, -0.5f, 1.0f);
	DirLight1.mSpecularIntensity	=1.5f;
	DirLight1.mDiffuseIntensity = 1.0f;
	LightMgr.AddDynamicDirLight(&DirLight1);*/

	/*N_Material Mat1;
	Mat1.baseMaterial.mBaseAmbientColor	= NVECTOR3(0.1f,  0.1f,0.1f);
	Mat1.baseMaterial.mBaseDiffuseColor		= NVECTOR3(1.0f,  1.0f, 1.0f);
	Mat1.baseMaterial.mBaseSpecularColor	=	NVECTOR3(1.0f, 1.0f,1.0f);
	Mat1.baseMaterial.mSpecularSmoothLevel	=	40;
	Mat1.baseMaterial.mNormalMapBumpIntensity = 0.3f;
	Mat1.baseMaterial.mEnvironmentMapTransparency = 0.2f;
	Mat1.diffuseMapID = TexMgr.GetIndexByName("Earth");
	Mat1.normalMapID = TexMgr.GetIndexByName("NormalMap_Earth");
	//Mat1.specularMapID = TexMgr.GetIndexByName("Earth");
	Mat1.cubeMap_environmentMapID = TexMgr.GetIndexByName("EnvironmentMap");
	UINT	 Mat1_ID = MatMgr.CreateMaterial(Mat1);

	//set material
	Mesh1.SetMaterial(Mat1_ID);*/
	//inputE.Initialize(Engine.GetRenderWindowHINSTANCE(), Engine.GetRenderWindowHWND());

	GraphicObjBuffer.AddRectangle(NVECTOR2(340.0f, 430.0f), NVECTOR2(640.0f, 480.0f), NVECTOR4(0, 0, 0, 0), TexMgr.GetIndexByName("BottomRightTitle"));
	Slicer.Step1_LoadPrimitiveMeshFromSTLFile("object.stl");
	Slicer.Step2_Intersection(5);
	Slicer.Step3_GenerateLineStrip();// extremely neccessary for optimization
	Slicer.Step4_SaveLayerDataToFile("object.LayerOutput");
	//Slicer.Step3_LoadLineStripsFrom_NOISELAYER_File("object.LayerOutput");


	NVECTOR3 v1, v2, n;

	for (UINT i = 0;i < Slicer.GetLineStripCount();i++)
	{

		Slicer.GetLineStrip(lineStrip, i);
		for (UINT j = 0;j < lineStrip.at(i).pointList.size() - 1;j++)
		{
			v1 = lineStrip.at(i).pointList.at(j);
			v2 = lineStrip.at(i).pointList.at(j + 1);
			n = lineStrip.at(i).normalList.at(j);
			//vertex
			GraphicObjBuffer.AddLine3D(v1, v2);
			//normal
			GraphicObjBuffer.AddLine3D((v1 + v2) / 2, ((v1 + v2) / 2) + n / 4, NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4(1.0f, 0.2f, 0.2f, 1.0f));
		}
	}
	
	return TRUE;
};


void MainLoop()
{

	InputProcess();

	Renderer.ClearBackground();

	//add to render list
	//Mesh1.AddToRenderList();
	GraphicObjBuffer.AddToRenderList();
	Atmos.AddToRenderList();

	//render
	//Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	//Renderer.RenderMeshInList();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	Renderer.RenderAtmosphereInList();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ADDITIVE);
	Renderer.RenderGraphicObjectInList();

	//present
	Renderer.RenderToScreen();
};

void InputProcess()
{
	inputE.Update();

	if (inputE.IsKeyPressed(NOISE_KEY_A))
	{
		Camera.fps_MoveRight(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_D))
	{
		Camera.fps_MoveRight(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_W))
	{
		Camera.fps_MoveForward(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_S))
	{
		Camera.fps_MoveForward(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_SPACE))
	{
		Camera.fps_MoveUp(0.1f);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_LCONTROL))
	{
		Camera.fps_MoveUp(-0.1f);
	}

	if (inputE.IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
	{
		Camera.RotateY_Yaw((float)inputE.GetMouseDiffX() / 200.0f);
		Camera.RotateX_Pitch((float)inputE.GetMouseDiffY() / 200.0f);
	}

};


void Cleanup()
{
	inputE.SelfDestruction();
	Slicer.SelfDestruction();
	NTimer.SelfDestruction();

	Engine.ReleaseAll();
	Scene.ReleaseAllChildObject();
};