#include <stdexcept>
#include "D3D11Sampler.hpp"

namespace Smol
{
	D3D11Sampler::D3D11Sampler(
		Device& device,
		const SamplerConfig& cfg,
		const std::string& name
	)
	{
		const D3D11_SAMPLER_DESC desc = {
			.Filter = cfg.filter,
			.AddressU = cfg.address,
			.AddressV = cfg.address,
			.AddressW = cfg.address,
			.MipLODBias = 0.0f,
			.MaxAnisotropy = 1,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.BorderColor = { 0, 0, 0, 0 },
			.MinLOD = 0,
			.MaxLOD = D3D11_FLOAT32_MAX
		};
		if (FAILED(device.CreateSamplerState(&desc, &sampler))) {
			throw std::runtime_error("Failed to create D3D11 Sampler");
		}

#if defined(_DEBUG) || !defined(NDEBUG)
		if (!name.empty()) {
			sampler->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				static_cast<UINT>(name.length()),
				name.c_str()
			);
		}
#endif
	}
}