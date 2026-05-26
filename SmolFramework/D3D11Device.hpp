#pragma once

#include <string_view>
#include "D3D11Definitions.hpp"
#include "D3D11Buffer.hpp"
#include "D3D11CommandList.hpp"
#include "D3D11PipelineState.hpp"
#include "D3D11Sampler.hpp"
#include "D3D11Swapchain.hpp"
#include "D3D11Texture.hpp"

namespace Smol
{
	class D3D11Device {
	private:
		FactoryRAII factory = nullptr;
		DeviceRAII device = nullptr;
		DeviceContextRAII context = nullptr;

	public:
		D3D11Device();
		~D3D11Device() = default;
		
		DECLARE_MOVE_ONLY(D3D11Device)

		D3D11CommandList createCommandList();
		D3D11Swapchain createSwapchain(const SwapchainConfig&, std::string_view name = {});

		D3D11GraphicsPipelineState createPipelineState(const GraphicsPipelineConfig&);

		D3D11Buffer createBuffer(const BufferConfig&, std::string_view name = {});
		D3D11Texture createTexture(const TextureConfig&, std::string_view name = {});
		D3D11Sampler createSampler(const SamplerConfig&, std::string_view name = {});

		void submit(D3D11CommandList&, D3D11Swapchain* swapchain = nullptr);
	};
}