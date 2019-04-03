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

	void Mainloop_RealTimePreview();

	void Mainloop_RenderPathTracedResult();

	void InputProcess_Preview();

	MAINLOOP_STATE mMainloopState;

private:

	void _InitPathTracer();

	void _InitPathTracingSceneObject();

	void _InitGraphicsObjectOfPreviewRender();

	void _InitAreaLight();

	//**GI**
	const uint32_t c_PathTracerBufferWidth;
	const uint32_t c_PathTracerBufferHeight;
	GI::PathTracer* m_pPathTracer;
	GI::PathTracerShader_Minimal mPathTracerShader_Minimal;
	Texture2D* m_pPathTracerRenderTarget;
	GraphicObject* m_pGraphicObj_ResultPreview;
	float mTotalPathTracerRenderTime;
};