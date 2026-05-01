#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include "D3D11Buffer.hpp"
#include "D3D11CommandList.hpp"
#include "D3D11PipelineState.hpp"
#include "D3D11Sampler.hpp"
#include "D3D11Swapchain.hpp"
#include "D3D11Texture.hpp"

namespace Smol
{
	void D3D11CommandList::beginRenderPass(
		std::span<const D3D11Texture*> renderTargets,
		const D3D11Texture* depthTarget,
		const ClearColor& clearColor,
		const ClearDepthStencil& clearDepthStencil,
		LoadAction loadAction,
		StoreAction storeAction
	) {
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(!inRenderPass && "Already in a render pass. Did you call CommandList::endRenderPass()?");

		RTV* rtvs[8];
		for (size_t i = 0; i < renderTargets.size(); ++i)
			rtvs[i] = renderTargets[i]->getRTV();

		DSV* dsv = depthTarget != nullptr ? depthTarget->getDSV() : nullptr;

		beginRenderPass(
			rtvs,
			dsv,
			clearColor,
			clearDepthStencil,
			loadAction,
			storeAction
		);

		inRenderPass = true;
	}

	void D3D11CommandList::beginRenderPass(
		const D3D11Swapchain& swapchain,
		const D3D11Texture* ds,
		const ClearColor& clearColor,
		const ClearDepthStencil& clearDepthStencil,
		LoadAction loadAction,
		StoreAction storeAction
	) {
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(!inRenderPass && "Already in a render pass. Did you call CommandList::endRenderPass()?");

		RTV* rtvs[] = { swapchain.getRTV() };
		DSV* dsv = ds != nullptr ? ds->getDSV() : nullptr;

		beginRenderPass(
			rtvs,
			dsv,
			clearColor,
			clearDepthStencil,
			loadAction,
			storeAction
		);

		inRenderPass = true;
	}

	void D3D11CommandList::endRenderPass() {
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(inRenderPass && "Not in a render pass. Did you call CommandList::beginRenderPass()?");
		

#if defined(_DEBUG) || !defined(NDEBUG)
		// clean-up srv binding for suppress data hazard warning.
		static ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

		if (maxBindedVSSRV > 0)
			context.VSSetShaderResources(0, maxBindedVSSRV, nullSRVs);
		if (maxBindedPSSRV > 0)
			context.PSSetShaderResources(0, maxBindedPSSRV, nullSRVs);
		if (maxBindedCSSRV > 0)
			context.CSSetShaderResources(0, maxBindedCSSRV, nullSRVs);

		maxBindedVSSRV = 0;
		maxBindedPSSRV = 0;
		maxBindedCSSRV = 0;

		// Unbind render targets to avoid warnings about resources being bound while being used.
		context.OMSetRenderTargets(0, nullptr, nullptr);
#endif

		inRenderPass = false;
	}

	void D3D11CommandList::setPipelineState(
		const D3D11GraphicsPipelineState& pso
	) {
		pso.bind(context);
	}

	void D3D11CommandList::setVertexBuffer(
		const D3D11Buffer& buffer,
		u32 slot,
		u32 stride,
		u32 offset
	) {
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(inRenderPass && "Not in a render pass. Did you call CommandList::beginRenderPass()?");
	
		Buffer* const buffers[] = { buffer.get() };
		context.IASetVertexBuffers(
			slot,
			1,
			buffers,
			&stride,
			&offset
		);
	}

	void D3D11CommandList::setIndexBuffer(
		const D3D11Buffer& buffer,
		DXGI_FORMAT format,
		u32 offset
	) {
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(inRenderPass && "Not in a render pass. Did you call CommandList::beginRenderPass()?");
	
		context.IASetIndexBuffer(
			buffer.get(),
			format,
			offset
		);
	}

	void D3D11CommandList::setConstantBuffer(
		const D3D11Buffer& buffer,
		u32 slot,
		ShaderStage stage,
		u32 offset
	) {
		using enum ShaderStage;
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(inRenderPass && "Not in a render pass. Did you call CommandList::beginRenderPass()?");
	
		Buffer* const buffers[] = { buffer.get() };
		switch (stage) {
		case VertexShader:
			context.VSSetConstantBuffers(slot, 1, buffers);
			break;
		case PixelShader:
			context.PSSetConstantBuffers(slot, 1, buffers);
			break;
		case ComputeShader:
			context.CSSetConstantBuffers(slot, 1, buffers);
			break;
		default:
			std::unreachable();
		}
	}

	void D3D11CommandList::setTexture(
		const D3D11Texture& texture,
		u32 slot,
		ShaderStage stage
	) {
		using enum ShaderStage;
		assert(isRecording && "Did you call CommandList::begin()?");
		assert(inRenderPass && "Not in a render pass. Did you call CommandList::beginRenderPass()?");

		SRV* const views[] = { texture.getSRV() };
		switch (stage) {
		case VertexShader:
#if defined(_DEBUG) || !defined(NDEBUG)
			maxBindedVSSRV = std::max(maxBindedVSSRV, slot + 1);
#endif
			context.VSSetShaderResources(slot, 1, views);
			break;
		case PixelShader:
#if defined(_DEBUG) || !defined(NDEBUG)
			maxBindedPSSRV = std::max(maxBindedPSSRV, slot + 1);
#endif
			context.PSSetShaderResources(slot, 1, views);
			break;
		case ComputeShader:
#if defined(_DEBUG) || !defined(NDEBUG)
			maxBindedCSSRV = std::max(maxBindedCSSRV, slot + 1);
#endif
			context.CSSetShaderResources(slot, 1, views);
			break;
		default:
			std::unreachable();
		}
	}

	void D3D11CommandList::setSampler(
		const D3D11Sampler& sampler,
		u32 slot,
		ShaderStage stage
	) {
		using enum ShaderStage;
		auto s = sampler.get();

		switch (stage) {
		case VertexShader:
			context.VSSetSamplers(slot, 1, &s);
			break;
		case PixelShader:
			context.PSSetSamplers(slot, 1, &s);
			break;
		case ComputeShader:
			context.CSSetSamplers(slot, 1, &s);
			break;
		default:
			std::unreachable();
		}
	}

	void D3D11CommandList::setViewport(
		const D3D11_VIEWPORT& viewport
	) {
		context.RSSetViewports(1, &viewport);
	}

	void D3D11CommandList::setScissorRect(
		const D3D11_RECT& scissor
	) {
		context.RSSetScissorRects(1, &scissor);
	}

	void D3D11CommandList::draw(
		u32 vertexCount,
		u32 instanceCount,
		u32 startVertex,
		u32 startInstance
	) {
		if (instanceCount > 1)
			context.DrawInstanced(
				vertexCount,
				instanceCount,
				startVertex,
				startInstance
			);
		else
			context.Draw(vertexCount, startVertex);
	}

	void D3D11CommandList::drawIndexed(
		u32 indexCount,
		u32 instanceCount,
		u32 startIndex,
		i32 baseVertex,
		u32 startInstance
	) {
		if (instanceCount > 1)
			context.DrawIndexedInstanced(
				indexCount,
				instanceCount,
				startIndex,
				baseVertex,
				startInstance
			);
		else
			context.DrawIndexed(
				indexCount,
				startIndex,
				baseVertex
			);
	}

	void D3D11CommandList::beginRenderPass(
		std::span<RTV*> rtvs,
		DSV* dsv,
		const ClearColor& clearColor,
		const ClearDepthStencil& clearDepthStencil,
		LoadAction loadAction,
		StoreAction
	) {
		context.OMSetRenderTargets(static_cast<UINT>(rtvs.size()), rtvs.data(), dsv);

		if (loadAction == LoadAction::Clear) {
			for (size_t i = 0; i < rtvs.size(); ++i)
				context.ClearRenderTargetView(rtvs[i], clearColor.v);

			if (dsv != nullptr)
				context.ClearDepthStencilView(dsv,
					D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
					clearDepthStencil.depth, clearDepthStencil.stencil
				);
		}
	}

	void D3D11CommandList::waitUntilCompleted() {
		assert(!isRecording && "Did you call CommandList::begin()?");

		// D3D11 executes commands immediately, so we can just flush the context.
		context.Flush();
	}
}