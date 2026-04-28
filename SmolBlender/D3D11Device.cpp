#include <stdexcept>
#include "D3D11Device.hpp"

namespace Smol
{
	D3D11Device::D3D11Device(){
		UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG) || !defined(NDEBUG)
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		if(FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
			throw std::runtime_error("Failed to create DXGI Factory");
		}

		AdapterRAII adapter;
		AdapterRAII selectedAdapter;
		SIZE_T maxDedicatedVideoMemory = 0;

		for(UINT i=0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// Check if adapter supports D3D11
			if (SUCCEEDED(D3D11CreateDevice(
				adapter.Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				0,
				nullptr, 0,
				D3D11_SDK_VERSION,
				nullptr, nullptr, nullptr
			))) {
				if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory) {
					maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
					selectedAdapter = adapter;
				}
			}
		}

		if(selectedAdapter == nullptr) {
			throw std::runtime_error("No suitable GPU found");
		}

		UINT deviceFlags = 0;
#if defined(_DEBUG) || !defined(NDEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		}, actualLevel;

		if(FAILED(D3D11CreateDevice(
			selectedAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			deviceFlags,
			featureLevels, _countof(featureLevels),
			D3D11_SDK_VERSION,
			&device, &actualLevel, &context
		))) {
			throw std::runtime_error("Failed to create D3D11 Device");
		}

		device->GetImmediateContext(&context);
	}

	D3D11CommandList D3D11Device::createCommandList() {
		return D3D11CommandList(*device.Get(), *context.Get());
	}

	D3D11Swapchain D3D11Device::createSwapchain(const SwapchainConfig& cfg) {
		return D3D11Swapchain(*device.Get(), *factory.Get(), cfg);
	}

	D3D11GraphicsPipelineState D3D11Device::createPipelineState(const GraphicsPipelineConfig& cfg) {
		return D3D11GraphicsPipelineState(*device.Get(), cfg);
	}

	D3D11Buffer D3D11Device::createBuffer(const BufferConfig& cfg, const std::string& name) {
		return D3D11Buffer(*device.Get(), *context.Get(), cfg, name);
	}

	D3D11Texture D3D11Device::createTexture(const TextureConfig& cfg, const std::string& name) {
		return D3D11Texture(*device.Get(), *context.Get(), cfg, name);
	}

	D3D11Sampler D3D11Device::createSampler(const SamplerConfig& cfg, const std::string& name) {
		return D3D11Sampler(*device.Get(), cfg, name);
	}

	void D3D11Device::submit(D3D11CommandList& cmdList, D3D11Swapchain* swapchain) {
		if (swapchain != nullptr) swapchain->present();
	}
}