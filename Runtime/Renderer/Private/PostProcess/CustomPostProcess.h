#pragma once
#include "PostProcessing.h"

FScreenPassTexture AddSceneInverse(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FScreenPassTexture& Input);
