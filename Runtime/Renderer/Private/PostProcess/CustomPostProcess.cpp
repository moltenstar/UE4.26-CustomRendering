#include "CustomPostProcess.h"
#include "ShaderClass/SceneInverse.h"

static TAutoConsoleVariable<bool> CVarSceneInverseTest(
	TEXT("r.PostProcessing.SceneInverseEnabled"),
	0,
	TEXT("Custom Scene Inverse Shader"),
	ECVF_RenderThreadSafe
);

FScreenPassTexture AddSceneInverse(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FScreenPassTexture& Input)
{
	bool SceneInverseTestEnabled = CVarSceneInverseTest.GetValueOnAnyThread();
	if (!SceneInverseTestEnabled)
	{
		return Input;
	}
	
	// create output
	FScreenPassRenderTarget Output;
	if (!Output.IsValid())
	{
		Output = FScreenPassRenderTarget::CreateFromInput(GraphBuilder, Input, View.GetOverwriteLoadAction(), TEXT("RenderSceneInverse"));
	}

	// setup shader parameters
	FSceneInverseParameters* sceneInverseParameters = GraphBuilder.AllocParameters<FSceneInverseParameters>();

	// this is how we bind render target
	sceneInverseParameters->RenderTargets[0] = Output.GetRenderTargetBinding();

	// get a linear sampler and the input screen pass texture
	FRHISamplerState* BilinearClampSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	sceneInverseParameters->Input = GetScreenPassTextureInput(Input, BilinearClampSampler);

	const FScreenPassTextureViewport InputViewport(Input);
	const FScreenPassTextureViewport OutputViewport(Output);
	
	// Get the actual shader instances off the ShaderMap
	auto ShaderMap = GetGlobalShaderMap(EShaderPlatform::SP_PCD3D_SM5);
	TShaderMapRef<FSceneInverseVS> SceneInverseVS(ShaderMap);
	TShaderMapRef<FSceneInversePS> SceneInversePS(ShaderMap);
	
	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("Render Scene Inverse"),
		View,
		OutputViewport,
		InputViewport,
		FScreenPassPipelineState(SceneInverseVS, SceneInversePS),
		sceneInverseParameters,
		EScreenPassDrawFlags::AllowHMDHiddenAreaMask,
		[SceneInverseVS, SceneInversePS, sceneInverseParameters](FRHICommandList& RHICmdList)
		{
			SetShaderParameters(RHICmdList, SceneInverseVS, SceneInverseVS.GetVertexShader(), *sceneInverseParameters);
			SetShaderParameters(RHICmdList, SceneInversePS, SceneInversePS.GetPixelShader(), *sceneInverseParameters);
		});

	return MoveTemp(Output);
}
