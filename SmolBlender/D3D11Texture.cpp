#include <stdexcept>
#include <utility>
#include "D3D11Texture.hpp"

namespace Smol
{
	static constexpr UINT getBytesPerPixel(DXGI_FORMAT format) {
		switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return 4;
		default:
			// TODO.
			std::unreachable();
		}
	}

	D3D11Texture::D3D11Texture(
		Device& device,
		DeviceContext& context,
		const TextureConfig& cfg,
		const std::string& name
	)
		: context(context)
		, width(cfg.width), height(cfg.height)
		, format(cfg.format)
	{
		using enum TextureUsage;

		const auto isShaderResource = has_flag(cfg.usage, AllowShaderRead);
		const auto isRenderTarget = has_flag(cfg.usage, AllowRenderTarget);
		const auto isDepthTarget = has_flag(cfg.usage, AllowDepthStencil);
		const auto isUnorderedAccess = has_flag(cfg.usage, AllowShaderWrite);

		UINT bindFlags = 0;
		if (isShaderResource) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (isRenderTarget) bindFlags |= D3D11_BIND_RENDER_TARGET;
		if (isDepthTarget) bindFlags |= D3D11_BIND_DEPTH_STENCIL;
		if (isUnorderedAccess) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		bool needsGPUOnly = isShaderResource || isRenderTarget ||
			isDepthTarget || isUnorderedAccess;

		const D3D11_TEXTURE2D_DESC desc{
			.Width = cfg.width,
			.Height = cfg.height,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = cfg.format,
			// No MSAA
			.SampleDesc = {1, 0},
			.Usage = needsGPUOnly ?
				D3D11_USAGE_DEFAULT : D3D11_USAGE_STAGING,
			.BindFlags = bindFlags,
			.CPUAccessFlags = needsGPUOnly ?
				UINT(0) : D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0
		};
		const D3D11_SUBRESOURCE_DATA initData{
			.pSysMem = cfg.initialData,
			.SysMemPitch = cfg.width * getBytesPerPixel(cfg.format),
			.SysMemSlicePitch = 0
		};

		if (FAILED(device.CreateTexture2D(
			&desc,
			cfg.initialData != nullptr ? &initData : nullptr,
			&texture
		))) {
			throw std::runtime_error("Failed to create D3D11 texture");
		}

#if defined(_DEBUG) || !defined(NDEBUG)
		if (!name.empty()) {
			texture->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				static_cast<UINT>(name.length()),
				name.c_str()
			);
		}
#endif

		if (isShaderResource) {
			const D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = format,
				.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
				.Texture2D = {
					.MostDetailedMip = 0,
					.MipLevels = 1,
				}
			};
			device.CreateShaderResourceView(
				texture.Get(),
				&srvDesc,
				&srv
			);
		}
		if (isRenderTarget) {
			const D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{
				.Format = format,
				.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
				.Texture2D = {
					.MipSlice = 0,
				}
			};
			device.CreateRenderTargetView(
				texture.Get(),
				&rtvDesc,
				&rtv
			);
		}
		if(isUnorderedAccess) {
			const D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{
				.Format = format,
				.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D,
				.Texture2D = {
					.MipSlice = 0,
				}
			};
			device.CreateUnorderedAccessView(
				texture.Get(),
				&uavDesc,
				&uav
			);
		}
		if(isDepthTarget) {
			const D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{
				.Format = format,
				.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
				.Texture2D = {
					.MipSlice = 0,
				}
			};
			device.CreateDepthStencilView(
				texture.Get(),
				&dsvDesc,
				&dsv
			);
		}
	}
}