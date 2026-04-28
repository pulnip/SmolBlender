#pragma once

#include <cassert>
#include <span>
#include "D3D11Definitions.hpp"
#include "fwd.hpp"
#include "Util.hpp"

namespace Smol
{
	enum class ShaderStage {
		VertexShader,
		PixelShader,
		ComputeShader,
	};

	enum class LoadAction {
		Load,
		Clear,
		DontCare
	};
	
	enum class StoreAction {
		Store,
		DontCare
	};

	struct ClearColor { float v[4] = { 0, 0, 0, 1 }; };
	struct ClearDepthStencil { float depth = 1.0f; UINT8 stencil = 0; };

	// Actually, D3D11 doesn't have a command list concept like D3D12.
	// But for understanding and encapsulating command recording functionality.
	class D3D11CommandList {
	private:
		// Note: Immediate context.
		DeviceContext& context;
		// simulate command recording
		bool isRecording = false;
		bool inRenderPass = false;
#if defined(_DEBUG) || !defined(NDEBUG)
		u32 maxBindedVSSRV = 0;
		u32 maxBindedPSSRV = 0;
		u32 maxBindedCSSRV = 0;
#endif

	public:
		D3D11CommandList(Device&, DeviceContext& ctx)
			: context(ctx) {}

		void begin() {
			assert(!isRecording && "Did you call CommandList::close()?");
			isRecording = true;
		}

		void close() {
			assert(isRecording && "Did you call CommandList::begin()?");
			isRecording = false;
		}

		void beginRenderPass(
			std::span<const D3D11Texture*> rts,
			const D3D11Texture* ds = nullptr,
			LoadAction loadAction = LoadAction::Clear,
			StoreAction storeAction = StoreAction::Store,
			const ClearColor& clearColor = {},
			const ClearDepthStencil& clearDepthStencil = {}
		);
		void beginRenderPass(
			const D3D11Swapchain&,
			const D3D11Texture* ds = nullptr,
			LoadAction loadAction = LoadAction::Load,
			StoreAction storeAction = StoreAction::Store,
			const ClearColor& clearColor = {},
			const ClearDepthStencil& clearDepthStencil = {}
		);
		void endRenderPass();

		void setVertexBuffer(
			const D3D11Buffer&,
			u32 slot,
			u32 stride = sizeof(Vertex),
			u32 offset = 0
		);
		void setIndexBuffer(
			const D3D11Buffer&,
			DXGI_FORMAT format = DXGI_FORMAT_R32_UINT,
			u32 offset = 0
		);

		void setPipelineState(
			const D3D11GraphicsPipelineState&
		);

		void setConstantBuffer(
			const D3D11Buffer&,
			u32 slot,
			ShaderStage stage,
			u32 offset = 0
		);
		void setTexture(
			const D3D11Texture&,
			u32 slot,
			ShaderStage stage
		);
		void setSampler(
			const D3D11Sampler&,
			u32 slot,
			ShaderStage stage
		);

		void setViewport(const D3D11_VIEWPORT&);
		void setScissorRect(const D3D11_RECT&);

		void draw(
			u32 vertexCount,
			u32 instanceCount = 1,
			u32 startVertex = 0,
			u32 startInstance = 0
		);
		void drawIndexed(
			u32 indexCount,
			u32 instanceCount = 1,
			u32 startIndex = 0,
			i32 baseVertex = 0,
			u32 startInstance = 0
		);

		void waitUntilCompleted();

	private:
		void beginRenderPass(
			std::span<RTV*> rtvs,
			DSV* dsv,
			LoadAction loadAction,
			StoreAction storeAction,
			const ClearColor& clearColor = {},
			const ClearDepthStencil& clearDepthStencil = {}
		);
	};
}