# UE4.26-Custom Rendering
Customize shader in UE4.

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
