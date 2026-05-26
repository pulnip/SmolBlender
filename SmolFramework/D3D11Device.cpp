#include <stdexcept>
#include <print>
#include "D3D11Device.hpp"

#define CHECK_FEATURE(FEAT, var) \
    device.CheckFeatureSupport(FEAT, &var, sizeof(var))

namespace
{
	inline std::string WideToUtf8(const wchar_t* w) {
		if (!w || !*w) return {};
		int len = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);

		std::string out(static_cast<size_t>(len - 1), '\0');
		WideCharToMultiByte(CP_UTF8, 0, w, -1, out.data(), len, nullptr, nullptr);

		return out;
	}

	void printInfo(IDXGIAdapter1& adapter, ID3D11Device& device) {
		DXGI_ADAPTER_DESC1 desc;
		adapter.GetDesc1(&desc);

		std::println(
			"Adapter={}\n"
			"Vendor=0x{:04X}, Device=0x{:04X}, Rev={}\n"
			"VRAM={}MiB, Shared Sysmem={}MiB\n"
			"LUID={:08X}-{:08X}",
			WideToUtf8(desc.Description),
			desc.VendorId, desc.DeviceId, desc.Revision,
			desc.DedicatedVideoMemory >> 20, desc.DedicatedSystemMemory >> 20,
			desc.AdapterLuid.HighPart, desc.AdapterLuid.LowPart
		);

		D3D11_FEATURE_DATA_THREADING threading{};
		CHECK_FEATURE(D3D11_FEATURE_THREADING, threading);
		std::println(
			"Threading: ConcurrentCreates={}, CommandLists={}",
			(bool)threading.DriverConcurrentCreates,
			(bool)threading.DriverCommandLists
		);

		D3D11_FEATURE_DATA_DOUBLES doubles{};
		CHECK_FEATURE(D3D11_FEATURE_DOUBLES, doubles);
		std::println(
			"FP64 in shaders: {}",
			(bool)doubles.DoublePrecisionFloatShaderOps
		);

		D3D11_FEATURE_DATA_D3D11_OPTIONS opts{};
		CHECK_FEATURE(D3D11_FEATURE_D3D11_OPTIONS, opts);
		std::println(
			"OM LogicOps: {}, UAVOnlyForcedSampleCount: {}, ConstBufOffset: {}",
			(bool)opts.OutputMergerLogicOp,
			(bool)opts.UAVOnlyRenderingForcedSampleCount,
			(bool)opts.ConstantBufferOffsetting
		);

		D3D11_FEATURE_DATA_D3D11_OPTIONS1 opts1{};
		CHECK_FEATURE(D3D11_FEATURE_D3D11_OPTIONS1, opts1);
		std::println("TiledResources Tier: {}, MinMaxFilter: {}, MapDefaultBuf: {}",
			(int)opts1.TiledResourcesTier,
			(int)opts1.MinMaxFiltering,
			(bool)opts1.MapOnDefaultBuffers
		);

		D3D11_FEATURE_DATA_D3D11_OPTIONS2 opts2{};
		CHECK_FEATURE(D3D11_FEATURE_D3D11_OPTIONS2, opts2);
		std::println("UMA: {}, StandardSwizzle: {}, ConservativeRaster Tier: {}, ROVs: {}, TypedUAVLoadExt: {}",
			(bool)opts2.UnifiedMemoryArchitecture,
			(bool)opts2.StandardSwizzle,
			(int)opts2.ConservativeRasterizationTier,
			(bool)opts2.ROVsSupported,
			(bool)opts2.TypedUAVLoadAdditionalFormats
		);

		D3D11_FEATURE_DATA_ARCHITECTURE_INFO arch{};
		CHECK_FEATURE(D3D11_FEATURE_ARCHITECTURE_INFO, arch);
		std::println(
			"TileBasedDeferredRenderer: {}",
			(bool)arch.TileBasedDeferredRenderer
		);

		D3D11_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpuVA{};
		CHECK_FEATURE(D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, gpuVA);
		std::println(
			"VA bits: per-resource={}, per-process={}",
			gpuVA.MaxGPUVirtualAddressBitsPerResource,
			gpuVA.MaxGPUVirtualAddressBitsPerProcess
		);

		auto reportFormat = [&device](DXGI_FORMAT fmt, const char* name) {
			// MSAA support
			std::print("{:<8} MSAA: ", name);
			for (UINT s : {1u, 2u, 4u, 8u, 16u}) {
				UINT q = 0;
				device.CheckMultisampleQualityLevels(fmt, s, &q);
				std::print("{}x(q={}) ", s, q);
			}
			std::println("");

			// Format support
			UINT s1 = 0;
			device.CheckFormatSupport(fmt, &s1);
			std::println("  Tex2D={} RT={} DS={} UAV={} Blendable={} Mip={}",
				bool(s1 & D3D11_FORMAT_SUPPORT_TEXTURE2D),
				bool(s1 & D3D11_FORMAT_SUPPORT_RENDER_TARGET),
				bool(s1 & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL),
				bool(s1 & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW),
				bool(s1 & D3D11_FORMAT_SUPPORT_BLENDABLE),
				bool(s1 & D3D11_FORMAT_SUPPORT_MIP)
			);
		};
		reportFormat(DXGI_FORMAT_R8G8B8A8_UNORM, "RGBA8");
		reportFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, "RGBA16F");
		reportFormat(DXGI_FORMAT_R10G10B10A2_UNORM, "RGB10A2");
		reportFormat(DXGI_FORMAT_D32_FLOAT, "D32F");
		reportFormat(DXGI_FORMAT_D24_UNORM_S8_UINT, "D24S8");
	}
}

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

		UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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
#if defined(_DEBUG) || !defined(NDEBUG)
		printInfo(*selectedAdapter.Get(), *device.Get());
#endif
	}

	D3D11CommandList D3D11Device::createCommandList() {
		return D3D11CommandList(*device.Get(), *context.Get());
	}

	D3D11Swapchain D3D11Device::createSwapchain(const SwapchainConfig& cfg, std::string_view name) {
		return D3D11Swapchain(*device.Get(), *factory.Get(), cfg, name);
	}

	D3D11GraphicsPipelineState D3D11Device::createPipelineState(const GraphicsPipelineConfig& cfg) {
		return D3D11GraphicsPipelineState(*device.Get(), cfg);
	}

	D3D11Buffer D3D11Device::createBuffer(const BufferConfig& cfg, std::string_view name) {
		return D3D11Buffer(*device.Get(), *context.Get(), cfg, name);
	}

	D3D11Texture D3D11Device::createTexture(const TextureConfig& cfg, std::string_view name) {
		return D3D11Texture(*device.Get(), *context.Get(), cfg, name);
	}

	D3D11Sampler D3D11Device::createSampler(const SamplerConfig& cfg, std::string_view name) {
		return D3D11Sampler(*device.Get(), cfg, name);
	}

	void D3D11Device::submit(D3D11CommandList&, D3D11Swapchain* swapchain) {
		if (swapchain != nullptr) swapchain->present();
	}
}