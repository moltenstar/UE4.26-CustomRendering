#pragma once
#include "MeshMaterialShader.h"
#include "MeshPassProcessor.h"

// vertex shader class for CustomTerrainPass
class FCustomTerrainPassVS : public FMeshMaterialShader
{
public:
	DECLARE_SHADER_TYPE(FCustomTerrainPassVS, MeshMaterial);

	static bool ShouldCompilePermutation(const FMeshMaterialShaderPermutationParameters& Parameters)
	{
		// Compile if supported by the hardware.
		const bool bIsFeatureSupported = IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);

		return bIsFeatureSupported && FMeshMaterialShader::ShouldCompilePermutation(Parameters);
	}

	FCustomTerrainPassVS() = default;
	FCustomTerrainPassVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshMaterialShader(Initializer)
	{}
};

// pixel shader class for CustomTerrainPass
class FCustomTerrainPassPS : public FMeshMaterialShader
{
public:
	DECLARE_SHADER_TYPE(FCustomTerrainPassPS, MeshMaterial);

	static bool ShouldCompilePermutation(const FMeshMaterialShaderPermutationParameters& Parameters)
	{
		// Compile if supported by the hardware.
		const bool bIsFeatureSupported = IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);

		return bIsFeatureSupported && FMeshMaterialShader::ShouldCompilePermutation(Parameters);
	}

	FCustomTerrainPassPS() = default;
	FCustomTerrainPassPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshMaterialShader(Initializer)
	{}
};

// hull shader class for CustomTerrainPass
class FCustomTerrainPassHS : public FMeshMaterialShader
{
public:
	DECLARE_TYPE_LAYOUT(FCustomTerrainPassHS, NonVirtual);

	static bool ShouldCompilePermutation(const FMeshMaterialShaderPermutationParameters& Parameters)
	{
		if (!RHISupportsTessellation(Parameters.Platform))
		{
			return false;
		}

		if (Parameters.VertexFactoryType && !Parameters.VertexFactoryType->SupportsTessellationShaders())
		{
			// VF can opt out of tessellation
			return false;	
		}

		if (Parameters.MaterialParameters.TessellationMode == MTM_NoTessellation)
		{
			// Material controls use of tessellation
			return false;	
		}

		return true;
	}

	FCustomTerrainPassHS() = default;
	FCustomTerrainPassHS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshMaterialShader(Initializer)
	{}
};

// domain shader class for CustomTerrainPass
class FCustomTerrainPassDS : public FMeshMaterialShader
{
public:
	DECLARE_TYPE_LAYOUT(FCustomTerrainPassDS, NonVirtual);

	static bool ShouldCompilePermutation(const FMeshMaterialShaderPermutationParameters& Parameters)
	{
		if (!RHISupportsTessellation(Parameters.Platform))
		{
			return false;
		}

		if (Parameters.VertexFactoryType && !Parameters.VertexFactoryType->SupportsTessellationShaders())
		{
			// VF can opt out of tessellation
			return false;
		}

		if (Parameters.MaterialParameters.TessellationMode == MTM_NoTessellation)
		{
			// Material controls use of tessellation
			return false;
		}

		return true;
	}

	FCustomTerrainPassDS() = default;
	FCustomTerrainPassDS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshMaterialShader(Initializer)
	{}
};

// class for custom terrain pass
class FCustomTerrainPassMeshProcessor : public FMeshPassProcessor
{
public:
	FCustomTerrainPassMeshProcessor(const FScene* InScene
		, ERHIFeatureLevel::Type InFeatureLevel
		, const FSceneView* InViewIfDynamicMeshCommand
		, const FMeshPassProcessorRenderState& InDrawRenderState
		, FMeshPassDrawListContext* InDrawListContext);

	virtual void AddMeshBatch(const FMeshBatch& MeshBatch, uint64 BatchElementMask, const FPrimitiveSceneProxy* PrimitiveSceneProxy, int32 StaticMeshId) override;

	FMeshPassProcessorRenderState PassDrawRenderState;
};