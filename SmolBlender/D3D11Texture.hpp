#pragma once

#include "D3D11Definitions.hpp"
#include "Util.hpp"

namespace Smol
{
	class D3D11Texture {
	private:
		TextureRAII texture = nullptr;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		u32 width = 0, height = 0;

		DeviceContext& context;

		SRVRAII srv = nullptr;
		RTVRAII rtv = nullptr;
		UAVRAII uav = nullptr;
		DSVRAII dsv = nullptr;

	public:
		D3D11Texture(Device&, DeviceContext&, const TextureConfig&, const std::string& name);
		~D3D11Texture() = default;

		Texture* get() const { return texture.Get(); }
		SRV* getSRV() const { return srv.Get(); }
		RTV* getRTV() const { return rtv.Get(); }
		UAV* getUAV() const { return uav.Get(); }
		DSV* getDSV() const { return dsv.Get(); }
	};
}