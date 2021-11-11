// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	ShaderParameters.h: Shader parameter definitions.
=============================================================================*/

#pragma once

#include "CoreMinimal.h"
#include "Serialization/MemoryLayout.h"
#include "RHI.h"

class FShaderParameterMap;
class FShaderParametersMetadata;
struct FShaderCompilerEnvironment;

RENDERCORE_API void CacheUniformBufferIncludes(TMap<const TCHAR*, struct FCachedUniformBufferDeclaration>& Cache, EShaderPlatform Platform);


enum EShaderParameterFlags
{
	// no shader error if the parameter is not used
	SPF_Optional,
	// shader error if the parameter is not used
	SPF_Mandatory
};

/** A shader parameter's register binding. e.g. float1/2/3/4, can be an array, UAV */
class FShaderParameter
{
	DECLARE_EXPORTED_TYPE_LAYOUT(FShaderParameter, RENDERCORE_API, NonVirtual);
public:
	FShaderParameter()
	:	BufferIndex(0)
	,	BaseIndex(0)
	,	NumBytes(0)
	{}

	RENDERCORE_API void Bind(const FShaderParameterMap& ParameterMap,const TCHAR* ParameterName, EShaderParameterFlags Flags = SPF_Optional);
	friend RENDERCORE_API FArchive& operator<<(FArchive& Ar,FShaderParameter& P);
	bool IsBound() const { return NumBytes > 0; }
	
	inline bool IsInitialized() const 
	{ 
		return true;
	}

	uint32 GetBufferIndex() const { return BufferIndex; }
	uint32 GetBaseIndex() const { return BaseIndex; }
	uint32 GetNumBytes() const { return NumBytes; }

private:
	LAYOUT_FIELD(uint16, BufferIndex);
	LAYOUT_FIELD(uint16, BaseIndex);
	// 0 if the parameter wasn't bound
	LAYOUT_FIELD(uint16, NumBytes);
};

/** A shader resource binding (textures or samplerstates). */
class FShaderResourceParameter
{
	DECLARE_EXPORTED_TYPE_LAYOUT(FShaderResourceParameter, RENDERCORE_API, NonVirtual);
public:
	FShaderResourceParameter()
	:	BaseIndex(0)
	,	NumResources(0) 
	{}

	RENDERCORE_API void Bind(const FShaderParameterMap& ParameterMap,const TCHAR* ParameterName,EShaderParameterFlags Flags = SPF_Optional);
	friend RENDERCORE_API FArchive& operator<<(FArchive& Ar,FShaderResourceParameter& P);
	bool IsBound() const { return NumResources > 0; }

	inline bool IsInitialized() const 
	{ 
		return true;
	}

	uint32 GetBaseIndex() const { return BaseIndex; }
	uint32 GetNumResources() const { return NumResources; }

private:
	LAYOUT_FIELD(uint16, BaseIndex);
	LAYOUT_FIELD(uint16, NumResources);
};

/** A class that binds either a UAV or SRV of a resource. */
class FRWShaderParameter
{
	DECLARE_EXPORTED_TYPE_LAYOUT(FRWShaderParameter, RENDERCORE_API, NonVirtual);
public:

	void Bind(const FShaderParameterMap& ParameterMap,const TCHAR* BaseName)
	{
		SRVParameter.Bind(ParameterMap,BaseName);

		// If the shader wants to bind the parameter as a UAV, the parameter name must start with "RW"
		FString UAVName = FString(TEXT("RW")) + BaseName;
		UAVParameter.Bind(ParameterMap,*UAVName);

		// Verify that only one of the UAV or SRV parameters is accessed by the shader.
		checkf(!(SRVParameter.GetNumResources() && UAVParameter.GetNumResources()),TEXT("Shader binds SRV and UAV of the same resource: %s"),BaseName);
	}

	bool IsBound() const
	{
		return SRVParameter.IsBound() || UAVParameter.IsBound();
	}

	bool IsUAVBound() const
	{
		return UAVParameter.IsBound();
	}

	uint32 GetUAVIndex() const
	{
		return UAVParameter.GetBaseIndex();
	}

	friend FArchive& operator<<(FArchive& Ar,FRWShaderParameter& Parameter)
	{
		return Ar << Parameter.SRVParameter << Parameter.UAVParameter;
	}

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetBuffer(TRHICmdList& RHICmdList, const TShaderRHIRef& Shader, const FRWBuffer& RWBuffer) const;

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetBuffer(TRHICmdList& RHICmdList, const TShaderRHIRef& Shader, const FRWBufferStructured& RWBuffer) const;

	template<typename TShaderRHIRef, typename TRHICmdList>
	inline void SetTexture(TRHICmdList& RHICmdList, const TShaderRHIRef& Shader, FRHITexture* Texture, FRHIUnorderedAccessView* UAV) const;

	template<typename TRHICmdList>
	inline void UnsetUAV(TRHICmdList& RHICmdList, FRHIComputeShader* ComputeShader) const;

private:
	LAYOUT_FIELD(FShaderResourceParameter, SRVParameter);
	LAYOUT_FIELD(FShaderResourceParameter, UAVParameter);
};

/** Creates a shader code declaration of this struct for the given shader platform. */
extern RENDERCORE_API void CreateUniformBufferShaderDeclaration(const TCHAR* Name,const FShaderParametersMetadata& UniformBufferStruct, EShaderPlatform Platform, FString& OutDeclaration);

class FShaderUniformBufferParameter
{
	DECLARE_EXPORTED_TYPE_LAYOUT(FShaderUniformBufferParameter, RENDERCORE_API, NonVirtual);
public:
	FShaderUniformBufferParameter()
	:	BaseIndex(0xffff)
	{}

	static RENDERCORE_API void ModifyCompilationEnvironment(const TCHAR* ParameterName,const FShaderParametersMetadata& Struct,EShaderPlatform Platform,FShaderCompilerEnvironment& OutEnvironment);

	RENDERCORE_API void Bind(const FShaderParameterMap& ParameterMap,const TCHAR* ParameterName,EShaderParameterFlags Flags = SPF_Optional);

	friend FArchive& operator<<(FArchive& Ar,FShaderUniformBufferParameter& P)
	{
		P.Serialize(Ar);
		return Ar;
	}

	bool IsBound() const { return BaseIndex != 0xffff; }

	void Serialize(FArchive& Ar)
	{
		Ar << BaseIndex;
	}

	inline bool IsInitialized() const 
	{ 
		return true;
	}
	uint32 GetBaseIndex() const { ensure(IsBound()); return BaseIndex; }

private:
	LAYOUT_FIELD(uint16, BaseIndex);
};

/** A shader uniform buffer binding with a specific structure. */
template<typename TBufferStruct>
class TShaderUniformBufferParameter : public FShaderUniformBufferParameter
{
public:
	static void ModifyCompilationEnvironment(const TCHAR* ParameterName,EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
	{
		FShaderUniformBufferParameter::ModifyCompilationEnvironment(ParameterName,TBufferStruct::StaticStruct,Platform,OutEnvironment);
	}

	friend FArchive& operator<<(FArchive& Ar,TShaderUniformBufferParameter& P)
	{
		P.Serialize(Ar);
		return Ar;
	}
};

#if RHI_RAYTRACING
struct FRayTracingShaderBindingsWriter : FRayTracingShaderBindings
{
	FUniformBufferRHIRef RootUniformBuffer;

	void Set(const FShaderResourceParameter& Param, FRHITexture* Value)
	{
		if (Param.IsBound())
		{
			checkf(Param.GetNumResources() == 1, TEXT("Resource array binding is not implemented"));
			Textures[Param.GetBaseIndex()] = Value;
		}
	}

	void Set(const FShaderResourceParameter& Param, FRHIShaderResourceView* Value)
	{
		if (Param.IsBound())
		{
			checkf(Param.GetNumResources() == 1, TEXT("Resource array binding is not implemented"));
			SRVs[Param.GetBaseIndex()] = Value;
		}
	}

	void Set(const FShaderUniformBufferParameter& Param, FRHIUniformBuffer* Value)
	{
		if (Param.IsBound())
		{
			UniformBuffers[Param.GetBaseIndex()] = Value;
		}
	}

	void Set(const FShaderResourceParameter& Param, FRHIUnorderedAccessView* Value)
	{
		if (Param.IsBound())
		{
			checkf(Param.GetNumResources() == 1, TEXT("Resource array binding is not implemented"));
			UAVs[Param.GetBaseIndex()] = Value;
		}
	}

	void Set(const FShaderResourceParameter& Param, FRHISamplerState* Value)
	{
		if (Param.IsBound())
		{
			checkf(Param.GetNumResources() == 1, TEXT("Resource array binding is not implemented"));
			Samplers[Param.GetBaseIndex()] = Value;
		}
	}

	void SetTexture(uint16 BaseIndex, FRHITexture* Value)
	{
		checkSlow(BaseIndex < UE_ARRAY_COUNT(Textures));
		Textures[BaseIndex] = Value;
	}

	void SetSRV(uint16 BaseIndex, FRHIShaderResourceView* Value)
	{
		checkSlow(BaseIndex < UE_ARRAY_COUNT(SRVs));
		SRVs[BaseIndex] = Value;
	}

	void SetSampler(uint16 BaseIndex, FRHISamplerState* Value)
	{
		checkSlow(BaseIndex < UE_ARRAY_COUNT(Samplers));
		Samplers[BaseIndex] = Value;
	}

	void SetUAV(uint16 BaseIndex, FRHIUnorderedAccessView* Value)
	{
		checkSlow(BaseIndex < UE_ARRAY_COUNT(UAVs));
		UAVs[BaseIndex] = Value;
	}

	void SetUniformBuffer(uint16 BaseIndex, FRHIUniformBuffer* Value)
	{
		checkSlow(BaseIndex < UE_ARRAY_COUNT(UniformBuffers));
		UniformBuffers[BaseIndex] = Value;
	}
};
#endif //RHI_RAYTRACING
