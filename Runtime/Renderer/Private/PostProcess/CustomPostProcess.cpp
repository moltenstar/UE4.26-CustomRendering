#include "CustomPostProcess.h"
#include "CustomSceneInverse.h"

static TAutoConsoleVariable<bool> CVarCustomSceneInverseTest(
	TEXT("r.PostProcessing.CustomSceneInverseEnabled"),
	0,
	TEXT("Custom Scene Inverse Shader"),
	ECVF_RenderThreadSafe
);

FScreenPassTexture AddCustomSceneInverse(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FScreenPassTexture& Input)
{
	bool CustomSceneInverseTestEnabled = CVarCustomSceneInverseTest.GetValueOnAnyThread();
	if (!CustomSceneInverseTestEnabled)
	{
		return Input;
	}
	
	// create output
	FScreenPassRenderTarget Output;
	if (!Output.IsValid())
	{
		Output = FScreenPassRenderTarget::CreateFromInput(GraphBuilder, Input, View.GetOverwriteLoadAction(), TEXT("RenderCustomSceneInverse"));
	}

	// setup shader parameters
	FCustomSceneInverseParameters* CustomSceneInverseParameters = GraphBuilder.AllocParameters<FCustomSceneInverseParameters>();

	// this is how we bind render target
	CustomSceneInverseParameters->RenderTargets[0] = Output.GetRenderTargetBinding();

	// get a linear sampler and the input screen pass texture
	FRHISamplerState* BilinearClampSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	CustomSceneInverseParameters->Input = GetScreenPassTextureInput(Input, BilinearClampSampler);

	const FScreenPassTextureViewport InputViewport(Input);
	const FScreenPassTextureViewport OutputViewport(Output);
	
	// Get the actual shader instances off the ShaderMap
	auto ShaderMap = GetGlobalShaderMap(EShaderPlatform::SP_PCD3D_SM5);
	TShaderMapRef<FCustomSceneInverseVS> CustomSceneInverseVS(ShaderMap);
	TShaderMapRef<FCustomSceneInversePS> CustomSceneInversePS(ShaderMap);
	
	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("Render Scene Inverse"),
		View,
		OutputViewport,
		InputViewport,
		FScreenPassPipelineState(CustomSceneInverseVS, CustomSceneInversePS),
		CustomSceneInverseParameters,
		EScreenPassDrawFlags::AllowHMDHiddenAreaMask,
		[CustomSceneInverseVS, CustomSceneInversePS, CustomSceneInverseParameters](FRHICommandList& RHICmdList)
		{
			SetShaderParameters(RHICmdList, CustomSceneInverseVS, CustomSceneInverseVS.GetVertexShader(), *CustomSceneInverseParameters);
			SetShaderParameters(RHICmdList, CustomSceneInversePS, CustomSceneInversePS.GetPixelShader(), *CustomSceneInverseParameters);
		});

	return MoveTemp(Output);
}
