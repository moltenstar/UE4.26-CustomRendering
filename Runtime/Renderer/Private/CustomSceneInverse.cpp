#include "CustomSceneInverse.h"

IMPLEMENT_SHADER_TYPE(, FCustomSceneInverseVS, TEXT("/Engine/SceneInverse.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FCustomSceneInversePS, TEXT("/Engine/SceneInverse.usf"), TEXT("MainPS"), SF_Pixel);