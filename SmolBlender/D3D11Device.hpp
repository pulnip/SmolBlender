#pragma once

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

		D3D11CommandList createCommandList();
		D3D11Swapchain createSwapchain(const SwapchainConfig&);

		D3D11GraphicsPipelineState createPipelineState(const GraphicsPipelineConfig&);

		D3D11Buffer createBuffer(const BufferConfig&, const std::string& name);
		D3D11Texture createTexture(const TextureConfig&, const std::string& name);
		D3D11Sampler createSampler(const SamplerConfig&, const std::string& name);

		void submit(D3D11CommandList&, D3D11Swapchain* swapchain = nullptr);
	};
}