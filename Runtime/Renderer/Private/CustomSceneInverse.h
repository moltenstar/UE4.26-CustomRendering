#pragma once
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "ScreenPass.h"

// shader parameters
BEGIN_SHADER_PARAMETER_STRUCT(FCustomSceneInverseParameters, )
	SHADER_PARAMETER_STRUCT(FScreenPassTextureInput, Input)
	RENDER_TARGET_BINDING_SLOTS()	// OMSetRenderTarget
END_SHADER_PARAMETER_STRUCT()

// class for vertex shader
class FCustomSceneInverseVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FCustomSceneInverseVS);
	SHADER_USE_PARAMETER_STRUCT_WITH_LEGACY_BASE(FCustomSceneInverseVS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	using FParameters = FCustomSceneInverseParameters;
	
};

// class for pixel shader
class FCustomSceneInversePS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCustomSceneInversePS);
	SHADER_USE_PARAMETER_STRUCT(FCustomSceneInversePS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	using FParameters = FCustomSceneInverseParameters;
};