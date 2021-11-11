#pragma once
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

// shader parameters
BEGIN_SHADER_PARAMETER_STRUCT(FSceneInverseParameters, )
	SHADER_PARAMETER_STRUCT(FScreenPassTextureInput, Input)
	RENDER_TARGET_BINDING_SLOTS()	// OMSetRenderTarget
END_SHADER_PARAMETER_STRUCT()

// class for vertex shader
class FSceneInverseVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSceneInverseVS);
	SHADER_USE_PARAMETER_STRUCT_WITH_LEGACY_BASE(FSceneInverseVS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	using FParameters = FSceneInverseParameters;
	
};

// class for pixel shader
class FSceneInversePS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FSceneInversePS);
	SHADER_USE_PARAMETER_STRUCT(FSceneInversePS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	using FParameters = FSceneInverseParameters;
};