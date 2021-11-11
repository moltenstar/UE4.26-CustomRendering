// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimBoneCompressionSettings.generated.h"

class UAnimBoneCompressionCodec;
struct FCompressibleAnimData;
struct FCompressibleAnimDataResult;

/*
 * This object is used to wrap a bone compression codec. It allows a clean integration in the editor by avoiding the need
 * to create asset types and factory wrappers for every codec.
 */
UCLASS(hidecategories = Object)
class ENGINE_API UAnimBoneCompressionSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	/** A list of animation bone compression codecs to try. Empty entries are ignored but the array cannot be empty. */
	UPROPERTY(Category = Compression, Instanced, EditAnywhere, meta = (NoElementDuplicate))
	TArray<UAnimBoneCompressionCodec*> Codecs;

#if WITH_EDITORONLY_DATA
	/** When compressing, the best codec below this error threshold will be used. */
	UPROPERTY(Category = Compression, EditAnywhere, meta = (ClampMin = "0"))
	float ErrorThreshold;

	/** Any codec (even one that increases the size) with a lower error will be used until it falls below the threshold. */
	UPROPERTY(Category = Compression, EditAnywhere)
	bool bForceBelowThreshold;
#endif

	/** Allow us to convert DDC serialized path back into codec object */
	UAnimBoneCompressionCodec* GetCodec(const FString& DDCHandle);

	//////////////////////////////////////////////////////////////////////////

#if WITH_EDITORONLY_DATA
	// UObject overrides
	virtual void GetPreloadDependencies(TArray<UObject*>& OutDeps) override;

	/** Returns whether or not we can use these settings to compress. */
	bool AreSettingsValid() const;

	/*
	 * Compresses the animation bones inside the supplied sequence.
	 * The resultant compressed data is applied to the OutCompressedData structure.
	 */
	bool Compress(const FCompressibleAnimData& AnimSeq, FCompressibleAnimDataResult& OutCompressedData) const;

	/** Generates a DDC key that takes into account the current settings and selected codec. */
	void PopulateDDCKey(FArchive& Ar);
#endif
};
