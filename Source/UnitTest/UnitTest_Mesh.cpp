

#include "Main3D.h"
#include <sstream>

IRoot IRoot;
IRenderer Renderer;
IScene IScene;
IMesh Mesh1;
ICamera ICamera;
ILightManager LightMgr;
IAtmosphere Atmos;
IMaterialManager	MatMgr;
ITextureManager	TexMgr;
IGraphicObject	GraphicObjBuffer;

IFontManager fontMgr;
IDynamicText myText_fps;

NoiseUtTimer NTimer(NOISE_TIMER_TIMEUNIT_MILLISECOND);
NoiseUtInputEngine inputE;
IDirLightD DirLight1;
IPointLightD	 PointLight1;
ISpotLightD	SpotLight1;

//Main Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND windowHWND;
	windowHWND = IRoot.CreateRenderWindow(640, 480, L"Hahaha Render Window", hInstance);
	inputE.Initialize(hInstance, windowHWND);
	Init3D(windowHWND);
	IRoot.SetMainLoopFunction(MainLoop);
	IRoot.Mainloop();
	Cleanup();
	return 0;
}

BOOL Init3D(HWND hwnd)
{
	const UINT bufferWidth = 640;
	const UINT bufferHeight = 480;

	//³õÊ¼»¯Ê§°Ü
	if (!IRoot.InitD3D(hwnd, bufferWidth, bufferHeight, TRUE))return FALSE;

	IScene.CreateMesh(Mesh1);
	IScene.CreateRenderer(Renderer);
	IScene.CreateCamera(ICamera);
	IScene.CreateLightManager(LightMgr);
	IScene.CreateMaterialManager(MatMgr);
	IScene.CreateTextureManager(TexMgr);
	IScene.CreateAtmosphere(Atmos);
	//fontMgr.Initialize();


	//Âþ·´ÉäÌùÍ¼
	TexMgr.CreateTextureFromFile("media/Earth.jpg", "Earth", TRUE, 0, 0, FALSE);
	TexMgr.CreateTextureFromFile("media/Jade.jpg", "Jade", FALSE, 256, 256, FALSE);
	TexMgr.CreateTextureFromFile("media/Jade.jpg", "JadeNormalMap", FALSE, 256, 256, TRUE);
	TexMgr.CreateTextureFromFile("media/universe2.jpg", "Universe", FALSE, 256, 256, FALSE);
	TexMgr.CreateTextureFromFile("media/bottom-right-conner-title.jpg", "BottomRightTitle", TRUE, 0, 0, FALSE);
	TexMgr.CreateCubeMapFromDDS("media/UniverseEnv.dds", "AtmoTexture", NOISE_CUBEMAP_SIZE_256x256);;
	TexMgr.ConvertTextureToGreyMap(TexMgr.GetTextureID("JadeNormalMap"));
	TexMgr.ConvertHeightMapToNormalMap(TexMgr.GetTextureID("JadeNormalMap"), 5.0f);


	//create font texture
	fontMgr.CreateFontFromFile("media/STXINWEI.ttf", "myFont", 24);
	fontMgr.InitDynamicTextA(0, "fps:000", 200, 100, NVECTOR4(0, 0, 0, 1.0f), 0, 0, myText_fps);
	myText_fps.SetTextColor(NVECTOR4(0, 0.3f, 1.0f, 0.5f));
	myText_fps.SetDiagonal(NVECTOR2(20, 20), NVECTOR2(150, 60));
	myText_fps.SetFont(0);

	Renderer.SetFillMode(NOISE_FILLMODE_SOLID);
	Renderer.SetCullMode(NOISE_CULLMODE_NONE);//NOISE_CULLMODE_BACK

	//------------------MESH INITIALIZATION----------------
	//Mesh1.LoadFile_STL("model/teapot7.stl");
	//Mesh1.LoadFile_OBJ("model/teapot2.obj");
	//Mesh1.LoadFile_3DS("model/box/TexturedBox.3ds");
	//Mesh1.LoadFile_3DS("model/treeScene/manyGeometry.3ds");
	//Mesh1.LoadFile_3DS("model/treeScene/treeScene3.3ds");
	//Mesh1.LoadFile_OBJ("model/cylinder.obj");
	//Mesh1.CreateSphere(5.0f, 30, 30);
	//Mesh1.CreateBox(10.0f, 10.0f, 10.0f);
	//Mesh1.CreatePlane(50.0f, 50.0f);
	Mesh1.CreateCylinder(20.0f, 30.0f,10,10);
	//Mesh1.SetPosition(0, 0, 0);
	//Mesh1.SetScale(0.2f, 0.2f, 0.2f);
	
	std::vector<N_DefaultVertex> tmpVB;
	Mesh1.GetVertexBuffer(tmpVB);
	for (auto v : tmpVB)
	{
		GraphicObjBuffer.AddLine3D(v.Pos, v.Pos + 5.0f*v.Normal, NVECTOR4(1.0f, 0, 0, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
		GraphicObjBuffer.AddLine3D(v.Pos, v.Pos + 5.0f*v.Tangent, NVECTOR4(0,0, 1.0f, 1.0f), NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));//draw the normal
	}
	
	//----------------------------------------------------------

	ICamera.SetPosition(2.0f, 0, 0);
	ICamera.SetLookAt(0, 0, 0);
	ICamera.SetViewAngle(MATH_PI / 2.5f, 1.333333333f);
	ICamera.SetViewFrustumPlane(1.0f, 500.f);
	//use bounding box of mesh to init camera pos
	NVECTOR3 AABB_MAX = Mesh1.ComputeBoundingBoxMax();
	float rotateRadius = sqrtf(AABB_MAX.x*AABB_MAX.x + AABB_MAX.z*AABB_MAX.z)*1.2f;
	float rotateY = Mesh1.ComputeBoundingBoxMax().y*1.3f;
	ICamera.SetPosition(rotateRadius*0.7f, rotateY, rotateRadius*0.7f);
	ICamera.SetLookAt(0, rotateY / 2, 0);

	Atmos.SetFogEnabled(FALSE);
	Atmos.SetFogParameter(7.0f, 8.0f, NVECTOR3(0, 0, 1.0f));
	Atmos.CreateSkyDome(4.0f, 4.0f, "Universe");
	//Atmos.CreateSkyBox(100.0f, 100.0f, 100.0f, TexMgr.GetTextureID("AtmoTexture"));

	//¡ª¡ª¡ª¡ª¡ª¡ªµÆ¹â¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
	DirLight1.mAmbientColor = NVECTOR3(0.0f,0.0f, 0.0f);
	DirLight1.mDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	DirLight1.mSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	//DirLight1.mDirection = NVECTOR3(-1.0f,1.0f, 0);
	DirLight1.mSpecularIntensity = 1.0f;
	DirLight1.mDiffuseIntensity =1.0f;
	LightMgr.CreateDynamicDirLight(DirLight1);


	N_Material Mat1;
	Mat1.mMatName = "myDefaultMaterial";
	Mat1.baseMaterial.mBaseAmbientColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	Mat1.baseMaterial.mBaseDiffuseColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.baseMaterial.mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	Mat1.baseMaterial.mSpecularSmoothLevel = 40;
	Mat1.baseMaterial.mNormalMapBumpIntensity = 0.1f;
	Mat1.baseMaterial.mEnvironmentMapTransparency = 0.05f;
	Mat1.diffuseMapName ="Jade";//"Earth");
	Mat1.normalMapName ="JadeNormalMap";
	UINT	 Mat1_ID = MatMgr.CreateMaterial(Mat1);

	//set material
	Mesh1.SetMaterial("myDefaultMaterial");

	GraphicObjBuffer.AddRectangle(NVECTOR2(340.0f, 430.0f), NVECTOR2(640.0f, 480.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), TexMgr.GetTextureID("BottomRightTitle"));

	return TRUE;
};


void MainLoop()
{
	static double incrNum = 0.0;;
	incrNum += 0.001;
	DirLight1.mDirection = NVECTOR3(sin(incrNum),-1,cos(incrNum));

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
	Renderer.RenderAtmosphere();
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
		ICamera.fps_MoveRight(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_D))
	{
		ICamera.fps_MoveRight(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_W))
	{
		ICamera.fps_MoveForward(0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_S))
	{
		ICamera.fps_MoveForward(-0.1f, FALSE);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_SPACE))
	{
		ICamera.fps_MoveUp(0.1f);
	}
	if (inputE.IsKeyPressed(NOISE_KEY_LCONTROL))
	{
		ICamera.fps_MoveUp(-0.1f);
	}

	if (inputE.IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT))
	{
		ICamera.RotateY_Yaw((float)inputE.GetMouseDiffX() / 200.0f);
		ICamera.RotateX_Pitch((float)inputE.GetMouseDiffY() / 200.0f);
	}

	//quit main loop and terminate program
	if (inputE.IsKeyPressed(NOISE_KEY_ESCAPE))
	{
		IRoot.SetMainLoopStatus(NOISE_MAINLOOP_STATUS_QUIT_LOOP);
	}

};


void Cleanup()
{
	inputE.SelfDestruction();
	NTimer.SelfDestruction();
	//fontMgr.SelfDestruction();
	//GUIMgr.SelfDestruction();

	IRoot.ReleaseAll();
	IScene.ReleaseAllChildObject();
};
