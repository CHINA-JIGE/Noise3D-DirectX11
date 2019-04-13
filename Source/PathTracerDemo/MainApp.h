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

	void _InitAreaLight();

	void _InitSoftShader();

	//**multi-thread
	std::thread mRenderThread;

	//**GI**
	GI::PathTracer* m_pPathTracer;
	GI::PathTracerShader_Minimal mPathTracerShader_Minimal;
	GI::PathTracerShader_Sky mPathTracerShader_Sky;
	GI::PathTracerShader_Diffuse mPathTracerShader_DiffuseDemo;
	Texture2D* m_pPathTracerRenderTarget;
	GraphicObject* m_pGraphicObj_ResultPreview;
	float mTotalPathTracerRenderTime;
};