#include "SceneInverse.h"

IMPLEMENT_SHADER_TYPE(, FSceneInverseVS, TEXT("/Engine/SceneInverse.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FSceneInversePS, TEXT("/Engine/SceneInverse.usf"), TEXT("MainPS"), SF_Pixel);