#pragma once

#include "D3D11Definitions.hpp"
#include "Util.hpp"

namespace Smol
{
	class D3D11Sampler {
	private:
		SamplerRAII sampler = nullptr;

	public:
		D3D11Sampler(Device&, const SamplerConfig&, const std::string& name);
		~D3D11Sampler() = default;

		DECLARE_MOVE_ONLY(D3D11Sampler)

		ID3D11SamplerState* get() const { return sampler.Get(); }
	};
}