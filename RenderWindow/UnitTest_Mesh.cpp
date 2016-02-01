

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
NoiseFontManager fontMgr;
Noise2DTextDynamic myText_fps;

NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtInputEngine inputE;
N_DirectionalLight DirLight1;
N_PointLight	 PointLight1;
N_SpotLight	SpotLight1;

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
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//³õÊ¼»¯Ê§°Ü
	if (!Engine.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	Scene.InitMesh(Mesh1);
	Scene.InitRenderer(Renderer);
	Scene.InitCamera(Camera);
	Scene.InitLightManager(LightMgr);
	Scene.InitMaterialManager(MatMgr);
	Scene.InitTextureManager(TexMgr);
	Scene.InitAtmosphere(Atmos);
	//fontMgr.Initialize();


	//Âþ·´ÉäÌùÍ¼
	TexMgr.CreateTextureFromFile(L"media/Earth.jpg", "Earth", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile(L"media/Jade.jpg", "Jade", FALSE, 256, 267, FALSE);
	TexMgr.CreateTextureFromFile(L"media/Jade.jpg", "JadeNormalMap", FALSE, 256, 256, TRUE);
	TexMgr.CreateTextureFromFile(L"media/universe2.jpg", "Universe", FALSE, 512, 512, FALSE);
	TexMgr.CreateTextureFromFile(L"media/bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);
	TexMgr.ConvertTextureToGreyMap(TexMgr.GetTextureID("JadeNormalMap"));
	TexMgr.ConvertHeightMapToNormalMap(TexMgr.GetTextureID("JadeNormalMap"), 5.0f);


	//create font texture
	fontMgr.CreateFontFromFile("media/STXINWEI.ttf", "myFont", 24);
	fontMgr.InitDynamicTextA(0, "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0, myText_fps);
	myText_fps.SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	myText_fps.SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	myText_fps.SetFont(0);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_BACK);//NOISE_CULLMODE_BACK

	//------------------MESH INITIALIZATION----------------
	//Mesh1.LoadFile_STL("model/teapot7.stl");
	Mesh1.LoadFile_OBJ("model/teapot2.obj");
	//Mesh1.CreateSphere(5.0f, 30, 30);
	Mesh1.SetPosition(0, 0, 0);
	//Mesh1.SetScale(0.2f, 0.2f, 0.2f);
	//----------------------------------------------------------

	Camera.SetPosition(2.0f, 0, 0);
	Camera.SetLookAt(0, 0, 0);
	Camera.SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	Camera.SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	NVECTOR3 AABB_MAX = Mesh1.ComputeBoundingBoxMax();
	float rotateRadius = sqrtf(AABB_MAX.x*AABB_MAX.x + AABB_MAX.z*AABB_MAX.z)*1.2f;
	float rotateY = Mesh1.ComputeBoundingBoxMax().y*1.3f;
	Camera.SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	Camera.SetLookAt(0, rotateY / 2, 0);

	Atmos.SetFogEnabled(FALSE);
	Atmos.SetFogParameter(7.0f, 8.0f, NVECTOR3(0, 0, 1.0f));
	Atmos.CreateSkyDome(4.0f, 4.0f, TexMgr.GetTextureID("Universe"));

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	DirLight1.mAmbientColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	DirLight1.mDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	DirLight1.mSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	DirLight1.mDirection = NVECTOR3(0.0f, -0.0f, 1.0f);
	DirLight1.mSpecularIntensity = 1.5f;
	DirLight1.mDiffuseIntensity = 1.0f;
	LightMgr.AddDynamicDirLight(DirLight1);

	N_Material Mat1;
	Mat1.baseMaterial.mBaseAmbientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	Mat1.baseMaterial.mBaseDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.baseMaterial.mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.baseMaterial.mSpecularSmoothLevel = 40;
	Mat1.baseMaterial.mNormalMapBumpIntensity = 0.1f;
	Mat1.baseMaterial.mEnvironmentMapTransparency = 0.05f;
	Mat1.diffuseMapID =TexMgr.GetTextureID("Jade");//"Earth");
	Mat1.normalMapID =TexMgr.GetTextureID("JadeNormalMap");
	Mat1.cubeMap_environmentMapID = NOISE_MACRO_INVALID_TEXTURE_ID;//TexMgr.GetTextureID("EnvironmentMap");
	UINT	 Mat1_ID = MatMgr.CreateMaterial(Mat1);

	//set material
	Mesh1.SetMaterial(Mat1_ID);

	GraphicObjBuffer.AddRectangle(NVECTOR2(340.0f, 430.0f), NVECTOR2(640.0f, 480.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), TexMgr.GetTextureID("BottomRightTitle"));

	return TRUE;
};


void MainLoop()
{
	//GUIMgr.Update();
	InputProcess();
	Renderer.ClearBackground();
	NTimer.NextTick();

	//update fps lable
	std::stringstream tmpS;
	tmpS << "fps :" << NTimer.GetFPS() << std::endl;
	myText_fps.SetTextAscii(tmpS.str());


	//add to render list
	Renderer.AddObjectToRenderList(Mesh1);
	Renderer.AddObjectToRenderList(GraphicObjBuffer);
	Renderer.AddObjectToRenderList(Atmos);
	Renderer.AddObjectToRenderList(myText_fps);

	//render
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	Renderer.RenderMeshes();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_OPAQUE);
	//Renderer.RenderAtmosphere();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ADDITIVE);
	Renderer.RenderGraphicObjects();
	Renderer.SetBlendingMode(NOISE_BLENDMODE_ALPHA);
	Renderer.RenderTexts();

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

	//quit main loop and terminate program
	if (inputE.IsKeyPressed(NOISE_KEY_ESCAPE))
	{
		Engine.SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

};


void Cleanup()
{
	inputE.SelfDestruction();
	NTimer.SelfDestruction();
	//fontMgr.SelfDestruction();
	//GUIMgr.SelfDestruction();

	Engine.ReleaseAll();
	Scene.ReleaseAllChildObject();
};
