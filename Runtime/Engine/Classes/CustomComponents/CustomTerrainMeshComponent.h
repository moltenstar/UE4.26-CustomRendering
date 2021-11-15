
#pragma once
#include "CustomTerrainMeshComponent.generated.h"

class UStaticMeshComponent;
class FStaticMeshSceneProxy;
class UCustomTerrainMeshComponent;

class FCustomTerrainMeshSceneProxy : public FStaticMeshSceneProxy
{
public:

	// simply inherits from static mesh scene proxy
	FCustomTerrainMeshSceneProxy(UCustomTerrainMeshComponent* InComponent)
	: FStaticMeshSceneProxy(Cast<UStaticMeshComponent>(InComponent), false)
	{
		bCustomTerrainPass = true;
	}
};

UCLASS(Blueprintable, ClassGroup=(Rendering, Common), hidecategories=(Object,Activation,"Components|Activation"), ShowCategories=(Mobility), editinlinenew, meta=(BlueprintSpawnableComponent))
class UCustomTerrainMeshComponent : public UStaticMeshComponent
{
public:
	GENERATED_BODY()

	// create proxy for custom terrain mesh component
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override
	{
		// null static mesh check
		if (GetStaticMesh() == nullptr || GetStaticMesh()->RenderData == nullptr)
		{
			return nullptr;
		}

		// LOD check
		const FStaticMeshLODResourcesArray& LODResources = GetStaticMesh()->RenderData->LODResources;
		if (LODResources.Num() == 0	|| LODResources[FMath::Clamp<int32>(GetStaticMesh()->MinLOD.Default, 0, LODResources.Num()-1)].VertexBuffers.StaticMeshVertexBuffer.GetNumVertices() == 0)
		{
			return nullptr;
		}
		LLM_SCOPE(ELLMTag::StaticMesh);

		FCustomTerrainMeshSceneProxy* Proxy = ::new FCustomTerrainMeshSceneProxy(this);
		return Proxy;
	}
};