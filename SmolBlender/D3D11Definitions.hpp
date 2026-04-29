#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "Util.hpp"

namespace Smol
{
	using Device = ID3D11Device;
	using DeviceContext = ID3D11DeviceContext;
	using Factory = IDXGIFactory2;
	using Adapter = IDXGIAdapter1;
	using Swapchain = IDXGISwapChain1;

	// Pipeline state types
	// Input Assembler Stage
	using InputLayout = ID3D11InputLayout;
	// Vertex Shader Stage
	using VertexShader = ID3D11VertexShader;
	// Rasterizer Stage
	using RasterizerState = ID3D11RasterizerState;
	// Pixel Shader Stage
	using PixelShader = ID3D11PixelShader;
	// Output Merger Stage
	using DepthStencilState = ID3D11DepthStencilState;
	using BlendState = ID3D11BlendState;

	// Resource types
	using Buffer = ID3D11Buffer;
	using Texture = ID3D11Texture2D;
	using Sampler = ID3D11SamplerState;

	// View types
	using SRV = ID3D11ShaderResourceView;
	using RTV = ID3D11RenderTargetView;
	using UAV = ID3D11UnorderedAccessView;
	using DSV = ID3D11DepthStencilView;

	// RAII wrappers for COM interfaces
	template<typename T>
	using RAII = Microsoft::WRL::ComPtr<T>;

	using DeviceRAII = RAII<Device>;
	using DeviceContextRAII = RAII<DeviceContext>;
	using FactoryRAII = RAII<Factory>;
	using AdapterRAII = RAII<Adapter>;
	using SwapchainRAII = RAII<Swapchain>;

	using InputLayoutRAII = RAII<InputLayout>;
	using RasterizerStateRAII = RAII<RasterizerState>;
	using DepthStencilStateRAII = RAII<DepthStencilState>;
	using BlendStateRAII = RAII<BlendState>;
	using VertexShaderRAII = RAII<VertexShader>;
	using PixelShaderRAII = RAII<PixelShader>;

	using BufferRAII = RAII<Buffer>;
	using TextureRAII = RAII<Texture>;
	using SamplerRAII = RAII<Sampler>;

	using SRVRAII = RAII<SRV>;
	using RTVRAII = RAII<RTV>;
	using UAVRAII = RAII<UAV>;
	using DSVRAII = RAII<DSV>;

	constexpr auto RHI_FRAMES_IN_FLIGHT = 3;

	struct SwapchainConfig {
		HWND hwnd = nullptr;
		
		// buffer configuration
		u32 width = 800, height = 600;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM; // Default format

		u32 bufferCount = RHI_FRAMES_IN_FLIGHT;     // Triple buffering
		bool vsync = true;                               // VSync enabled by default
		bool allowTearing = false;                       // Variable refresh rate

#if defined(_DEBUG) || !defined(NDEBUG)
		std::string debugName = "Swapchain";
#endif
	};

	struct Vertex {
		float position[3];
		float normal[3];
		float texcoord[2];
		float tangent[4];
	};

	constexpr std::array DEFAULT_INPUT_LAYOUT = {
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, // float3 (12 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, // float3 (12 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 12,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT, // float2 (8 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 24,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "TANGENT",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, // float4 (16 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 32,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		}
	};

	constexpr D3D11_RASTERIZER_DESC DEFAULT_RASTERIZER_DESC{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_BACK,
		.FrontCounterClockwise = TRUE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.0f,
		.SlopeScaledDepthBias = 0.0f,
		.DepthClipEnable = TRUE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE
	};

	constexpr D3D11_DEPTH_STENCIL_DESC DEFAULT_DEPTH_STENCIL_DESC{
		// Depth test configuration
		.DepthEnable = TRUE,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS,
		// Stencil test configuration
		.StencilEnable = FALSE,
		.StencilReadMask = 0xFF,
		.StencilWriteMask = 0xFF,
		.FrontFace = {
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS
		},
		.BackFace = {
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS
		}
	};

	constexpr D3D11_BLEND_DESC DEFAULT_BLEND_DESC{
		.AlphaToCoverageEnable = FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget = {
			{
				.BlendEnable = FALSE,
				.SrcBlend = D3D11_BLEND_ONE,
				.DestBlend = D3D11_BLEND_ZERO,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
			}
		}
	};

	struct GraphicsPipelineConfig {
		std::optional<std::span<const D3D11_INPUT_ELEMENT_DESC>> inputElementDescs = std::nullopt;
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		std::filesystem::path vertexShaderPath;
		std::string vertexShaderEntryPoint = "vs_main";
		D3D11_RASTERIZER_DESC rasterizerState = DEFAULT_RASTERIZER_DESC;
		std::filesystem::path pixelShaderPath;
		std::string pixelShaderEntryPoint = "ps_main";
		std::optional<D3D11_DEPTH_STENCIL_DESC> depthStencilState = std::nullopt;
		std::optional<D3D11_BLEND_DESC> blendState = std::nullopt;
		DXGI_FORMAT renderTargetFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
		UINT numRenderTargets = 1;
	};

	enum class MemoryAccess : u8 {
		GPUOnly = 0,
		CPURead = 1,
		CPUWrite = 2
	};

	enum class BufferUsage : u8 {
		None = 0,
		// fixed binding
		VertexBuffer = 1 << 0,
		IndexBuffer = 1 << 1,
		ConstantBuffer = 1 << 2,
		// view capability
		AllowShaderRead = 1 << 4,
		AllowShaderWrite = 1 << 5
	};

	struct BufferConfig {
		size_t size = 0;
		BufferUsage usage = BufferUsage::None;
		MemoryAccess access = MemoryAccess::GPUOnly;

		void* initialData = nullptr;
	};

	enum class TextureUsage : u8{
		None = 0,
		AllowShaderRead = 1 << 0,
		AllowRenderTarget = 1 << 1,
		AllowDepthStencil = 1 << 2,
		AllowShaderWrite = 1 << 3,
	};

	struct TextureConfig {
		u32 width = 0, height = 0;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureUsage usage = TextureUsage::None;

		void* initialData = nullptr;
	};

	struct SamplerConfig {
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		D3D11_TEXTURE_ADDRESS_MODE address = D3D11_TEXTURE_ADDRESS_WRAP;
	};
	constexpr SamplerConfig LINEAR_WRAP_SAMPLER_CONFIG{
		.filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.address = D3D11_TEXTURE_ADDRESS_WRAP
	};
	constexpr SamplerConfig NEAREST_WRAP_SAMPLER_CONFIG{
		.filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.address = D3D11_TEXTURE_ADDRESS_WRAP
	};
}
