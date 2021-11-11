
#pragma once
#include "CustomTerrainMeshComponent.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable, ClassGroup=(Rendering, Common), hidecategories=(Object,Activation,"Components|Activation"), ShowCategories=(Mobility), editinlinenew, meta=(BlueprintSpawnableComponent))
class UCustomTerrainMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
};