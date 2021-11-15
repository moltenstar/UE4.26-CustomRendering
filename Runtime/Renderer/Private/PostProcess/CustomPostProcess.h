#pragma once
#include "PostProcessing.h"

FScreenPassTexture AddCustomSceneInverse(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FScreenPassTexture& Input);
