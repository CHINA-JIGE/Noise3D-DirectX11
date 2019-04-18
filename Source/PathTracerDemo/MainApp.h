#pragma once

using namespace Noise3D;

class MainApp:
	public RealTimeRenderModule
{
public:

	MainApp();

	~MainApp();

	void PathTracerStartRender();

	void Init_GI();

private:

	enum MAINLOOP_STATE
	{
		PREVIEW,
		PATH_TRACING_RESULT
	};

	virtual void Callback_Mainloop() override;

	virtual void Callback_Cleanup() override;

	void Mainloop_RealTimePreview();

	void Mainloop_RenderPathTracedResult();

	void InputProcess_Preview();

	void InputProcess_PathTracerRendering();

	MAINLOOP_STATE mMainloopState;

private:

	void _InitPathTracer();

	void _InitGraphicsObjectOfPreviewRender();

	void _InitSoftShader();

	//**multi-thread
	std::thread mRenderThread;

	//**GI**
	GI::PathTracer* m_pPathTracer;
	GI::PathTracerShader_MinimalDemo mPathTracerShader_Minimal;
	GI::PathTracerShader_ReflectionDemo mPathTracerShader_ReflectionDemo;
	GI::PathTracerShader_DiffuseDemo mPathTracerShader_DiffuseDemo;
	GI::PathTracerShader_RefractionDemo mPathTracerShader_RefractionDemo;
	GI::PathTracerShader_AreaLightingDemo mPathTracerShader_AreaLightingDemo;
	Texture2D* m_pPathTracerRenderTarget;
	GraphicObject* m_pGraphicObj_ResultPreview;
	float mTotalPathTracerRenderTime;
};