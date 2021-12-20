# UE4.26-Custom Rendering
Customize rendering in UE4.

Basically I'm trying to customize shaders in native way. <br>
We can do 99% application in material editor no doubtly. <br>
But as programmers, we always enjoy that old-school text based shader. Don't we? <br>

# How to test this?
This repo doesn't clone whole UE source code. So, copy Shaders/ to Engine/. <br>
And copy Runtime/ to  Engine/Source/. <br>
Just copy paste them. (Of course you need to checkout whole engine source code from official github first.) <br>

### Hello World Global Shader (2021-10-08)
+ Add SceneInverse.usf/.h/.cpp. Which defines a simple scene color inverse shader.
+ Add CustomPostProcess.h/.cpp. Which utlitzes the shader we create.
+ Call CustomPostProcess::AddSceneInverse() in PostProcessing.cpp. I put it at the end of UE's post processing effects.
<br>But before the visualization/debug passes.
+ Use r.PostProcessing.SceneInverseEnabled for debugging this.
+ Implementation detail: https://thegraphicguysquall.wordpress.com/2021/10/09/customizing-shader-in-ue4-global-shader/
![alt text](https://i.imgur.com/sd6oYEw.jpg)

### Hello World Deferred Mesh Pass (2021-11-15)
+ Add SimpleTerrainPlane.usf, which is the hello world mesh pass shader.
+ SceneVisibility.cpp, call DrawCommandPacket.AddCommandsForMesh() for my custom terrain pass. Also exclude my pass from depth pre pass.
+ PrimitiveSceneProxy.h/.cpp, add bCustomTerrainPass flag for differentaite my custom pass with UE's pass.
+ Add CustomTerrainMeshComponent.h, only meshes (actors) use this component will be rendered in my custom pass.
+ BaseParrRendering.cpp, call RenderCustomTerrainPass() before UE's RenderBasePassInternal().
+ DeferredShadingRenderer.h, define RenderCustomTerrainPass().
+ CustomTerrainPass.h/.cpp, shader class for SimpleTerrainPlane.usf. And FMeshPassProcessor inheritance. AddMeshBatch() and RenderCustomTerrainPass() are implemented here.
+ Implementation detail: https://thegraphicguysquall.wordpress.com/2021/11/15/unreal-4-custom-mesh-pass-in-deferred-shading/
![alt text](https://thegraphicguysquall.files.wordpress.com/2021/11/noname2.jpg)
![alt text](https://thegraphicguysquall.files.wordpress.com/2021/11/noname.jpg)

### Play With Material Inputs (2021-12-20)
+ SimpleTerrainPlane.usf: Use FPixelMaterialInputs struct instead of test color. So that it can react to Material Graph input.
+ Runtime\Engine\Public\SceneTypes.h: Add MP_JustATestColor enum.
+ Runtime\Engine\Private\Materials\MaterialShared.cpp: Add JustATestColor usages.
+ Runtime\Engine\Private\Materials\MaterialExpressions.cpp: Add JustATestColor usages.
+ Runtime\Engine\Private\Materials\Material.cpp: Add JustATestColor usages.
+ Runtime\Engine\Private\Materials\HLSLMaterialTranslator.cpp: Add JustATestColor usages.
+ Runtime\Engine\Classes\Materials\MaterialExpressionMakeMaterialAttributes.h: Add JustATestColor usages.
+ Runtime\Engine\Classes\Materials\Material.h: Add JustATestColor usages.
+ Editor\UnrealEd\Private\MaterialGraph.cpp: Add JustATestColor usages.
+ Implementation detail: https://thegraphicguysquall.wordpress.com/2021/12/21/unreal-4-play-with-material-inputs/
![alt text](https://thegraphicguysquall.files.wordpress.com/2021/12/noname5.jpg)
![alt text](https://thegraphicguysquall.files.wordpress.com/2021/12/noname4-1.jpg)
