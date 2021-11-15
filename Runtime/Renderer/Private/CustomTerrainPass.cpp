#include "CustomTerrainPass.h"
#include "ScenePrivate.h"
#include "MeshPassProcessor.inl"
#include "DeferredShadingRenderer.h"
#include "BasePassRendering.h"

// shader entry point
IMPLEMENT_SHADER_TYPE(, FCustomTerrainPassVS, TEXT("/Engine/SimpleTerrainPlane.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FCustomTerrainPassPS, TEXT("/Engine/SimpleTerrainPlane.usf"), TEXT("MainPS"), SF_Pixel);

// FMeshPassProcessor interface
FCustomTerrainPassMeshProcessor::FCustomTerrainPassMeshProcessor(const FScene* InScene
	, ERHIFeatureLevel::Type InFeatureLevel
	, const FSceneView* InViewIfDynamicMeshCommand
	, const FMeshPassProcessorRenderState& InDrawRenderState
	, FMeshPassDrawListContext* InDrawListContext)
	: FMeshPassProcessor(InScene, InFeatureLevel, InViewIfDynamicMeshCommand, InDrawListContext)
{
	PassDrawRenderState = InDrawRenderState;
}

void FCustomTerrainPassMeshProcessor::AddMeshBatch(const FMeshBatch& MeshBatch, uint64 BatchElementMask, const FPrimitiveSceneProxy* PrimitiveSceneProxy, int32 StaticMeshId)
{
	// add mesh batch for custom terrain pass

	// check fallback material, and decide the material render proxy
	const FMaterialRenderProxy* FallbackMaterialRenderProxyPtr = nullptr;
	const FMaterial& MaterialResource = MeshBatch.MaterialRenderProxy->GetMaterialWithFallback(FeatureLevel, FallbackMaterialRenderProxyPtr);
	const FMaterialRenderProxy& MaterialRenderProxy = FallbackMaterialRenderProxyPtr ? *FallbackMaterialRenderProxyPtr : *MeshBatch.MaterialRenderProxy;

	// get render state
	FMeshPassProcessorRenderState DrawRenderState(PassDrawRenderState);

	// setup pass shaders
	const FVertexFactory* VertexFactory = MeshBatch.VertexFactory;
	TMeshProcessorShaders<
		FCustomTerrainPassVS,
		FCustomTerrainPassHS,
		FCustomTerrainPassDS,
		FCustomTerrainPassPS> CustomTerrainPassShaders;

	CustomTerrainPassShaders.VertexShader = MaterialResource.GetShader<FCustomTerrainPassVS>(VertexFactory->GetType());
	CustomTerrainPassShaders.PixelShader = MaterialResource.GetShader<FCustomTerrainPassPS>(VertexFactory->GetType());

	// fill mode and cull mode
	const FMeshDrawingPolicyOverrideSettings OverrideSettings = ComputeMeshOverrideSettings(MeshBatch);
	const ERasterizerFillMode MeshFillMode = ComputeMeshFillMode(MeshBatch, MaterialResource, OverrideSettings);
	const ERasterizerCullMode MeshCullMode = ComputeMeshCullMode(MeshBatch, MaterialResource, OverrideSettings);

	// sort key, for now just use default one
	FMeshDrawCommandSortKey SortKey = FMeshDrawCommandSortKey::Default;

	// init shader element data, for now just use default one
	FMeshMaterialShaderElementData ShaderElementData;
	ShaderElementData.InitializeMeshMaterialData(ViewIfDynamicMeshCommand, PrimitiveSceneProxy, MeshBatch, StaticMeshId, true);
		
	BuildMeshDrawCommands(
		MeshBatch,
		BatchElementMask,
		PrimitiveSceneProxy,
		MaterialRenderProxy,
		MaterialResource,
		DrawRenderState,
		CustomTerrainPassShaders,
		MeshFillMode,
		MeshCullMode,
		SortKey,
		EMeshPassFeatures::Default,
		ShaderElementData);
}

// this create terrain pass processor
FMeshPassProcessor* CreateCustomTerrainPassProcessor(const FScene* Scene, const FSceneView* InViewIfDynamicMeshCommand, FMeshPassDrawListContext* InDrawListContext)
{
	// for now, just init like base pass
	// but enable depth pass
	FMeshPassProcessorRenderState PassDrawRenderState(Scene->UniformBuffers.ViewUniformBuffer);
	PassDrawRenderState.SetInstancedViewUniformBuffer(Scene->UniformBuffers.InstancedViewUniformBuffer);
	PassDrawRenderState.SetDepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite);
	PassDrawRenderState.SetDepthStencilState(TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI());
	PassDrawRenderState.SetBlendState(TStaticBlendStateWriteMask<CW_RGBA, CW_RGBA, CW_RGBA, CW_RGBA>::GetRHI());

	return new(FMemStack::Get()) FCustomTerrainPassMeshProcessor(Scene, Scene->GetFeatureLevel(), InViewIfDynamicMeshCommand, PassDrawRenderState, InDrawListContext);
}

// register custom terrain pass
FRegisterPassProcessorCreateFunction RegisterCustomTerrainPass(&CreateCustomTerrainPassProcessor, EShadingPath::Deferred, EMeshPass::CustomTerrainPass, EMeshPassFlags::MainView);

// custom terrain pass for deferred shading
BEGIN_SHADER_PARAMETER_STRUCT(FCustomTerrainPassParameters, )
	SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FOpaqueBasePassUniformParameters, BasePass)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

void FDeferredShadingSceneRenderer::RenderCustomTerrainPass(FRDGBuilder& GraphBuilder,
	const FRenderTargetBindingSlots& BasePassRenderTargets,
	FExclusiveDepthStencil::Type BasePassDepthStencilAccess,
	FRDGTextureRef ForwardScreenSpaceShadowMask)
{
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex)
	{
		FViewInfo& View = Views[ViewIndex];
		RDG_GPU_MASK_SCOPE(GraphBuilder, View.GPUMask);
		RDG_EVENT_SCOPE_CONDITIONAL(GraphBuilder, Views.Num() > 1, "View%d", ViewIndex);

		// enable depth write for custom terrain pass
		// since I've disabled it in built-in depth pass
		FMeshPassProcessorRenderState DrawRenderState(View);
		DrawRenderState.SetDepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite);
		DrawRenderState.SetDepthStencilState(TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI());
		DrawRenderState.SetBlendState(TStaticBlendStateWriteMask<CW_RGBA, CW_RGBA, CW_RGBA, CW_RGBA>::GetRHI());

		FCustomTerrainPassParameters* PassParameters = GraphBuilder.AllocParameters<FCustomTerrainPassParameters>();
		PassParameters->BasePass = CreateOpaqueBasePassUniformBuffer(GraphBuilder, View, ForwardScreenSpaceShadowMask, nullptr, ViewIndex);
		PassParameters->RenderTargets = BasePassRenderTargets;
		PassParameters->RenderTargets.DepthStencil.SetDepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite);

		if (View.ShouldRenderView())
		{
			/* BEGIN CUSTOM TERRAIN PASS */
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("CustomTerrainPass"),
				PassParameters,
				ERDGPassFlags::Raster,
				[this, &View, PassParameters](FRHICommandList& RHICmdList)
			{
				Scene->UniformBuffers.UpdateViewUniformBuffer(View);
				SetStereoViewport(RHICmdList, View, 1.0f);
				View.ParallelMeshDrawCommandPasses[EMeshPass::CustomTerrainPass].DispatchDraw(nullptr, RHICmdList);
			});
			/* END CUSTOM TERRAIN PASS */
		}
	}
}
