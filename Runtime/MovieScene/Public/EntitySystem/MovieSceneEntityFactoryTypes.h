// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/ArrayView.h"

#include "EntitySystem/MovieSceneEntityIDs.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"

class UMovieSceneEntitySystemLinker;


namespace UE
{
namespace MovieScene
{

struct FEntityRange;

struct FChildEntityFactory
{
	virtual ~FChildEntityFactory(){}

	int32 Num() const;

	int32 GetCurrentIndex() const;

	void Apply(UMovieSceneEntitySystemLinker* Linker, const FEntityAllocation* ParentAllocation);
	
	void Add(int32 EntityIndex)
	{
		ParentEntityOffsets.Add(EntityIndex);
	}

	virtual void GenerateDerivedType(FComponentMask& OutNewEntityType)
	{}

	virtual void PostInitialize(UMovieSceneEntitySystemLinker* Linker)
	{}

	virtual void InitializeAllocation(UMovieSceneEntitySystemLinker* Linker, const FComponentMask& ParentType, const FComponentMask& ChildType, const FEntityAllocation* ParentAllocation, TArrayView<const int32> ParentAllocationOffsets, const FEntityRange& InChildEntityRange)
	{}

protected:
	TArrayView<const int32> CurrentEntityOffsets;
	TArray<int32> ParentEntityOffsets;
};



struct FChildEntityInitializer
{
	virtual ~FChildEntityInitializer(){}

	bool IsRelevant(const FComponentMask& InParentType, const FComponentMask& InChildType) const
	{
		// Entity initializers with no parent component are always valid
		const bool bHasParentComponent = !ParentComponent || InParentType.Contains(ParentComponent);
		const bool bHasChildComponent  =  ChildComponent  && InChildType.Contains(ChildComponent);

		return bHasParentComponent && bHasChildComponent;
	}

	FComponentTypeID GetParentComponent() const
	{
		return ParentComponent;
	}

	FComponentTypeID GetChildComponent() const
	{
		return ChildComponent;
	}

	virtual void Run(const FEntityRange&, const FEntityAllocation*, TArrayView<const int32>) = 0;

protected:

	FComponentTypeID ParentComponent, ChildComponent;

	explicit FChildEntityInitializer(FComponentTypeID InParentComponent, FComponentTypeID InChildComponent)
		: ParentComponent(InParentComponent)
		, ChildComponent(InChildComponent)
	{}
};


struct FMutualEntityInitializer
{
	virtual ~FMutualEntityInitializer(){}

	bool IsRelevant(const FComponentMask& InType) const
	{
		return InType.Contains(ComponentA) && InType.Contains(ComponentB);
	}

	FComponentTypeID GetComponentA() const
	{
		return ComponentA;
	}

	FComponentTypeID GetComponentB() const
	{
		return ComponentB;
	}

	virtual void Run(const FEntityRange& Range) = 0;

protected:

	FComponentTypeID ComponentA, ComponentB;

	explicit FMutualEntityInitializer(FComponentTypeID InComponentA, FComponentTypeID InComponentB)
		: ComponentA(InComponentA), ComponentB(InComponentB)
	{
		check(ComponentA && ComponentB);
	}
};


}	// using namespace MovieScene
}	// using namespace UE