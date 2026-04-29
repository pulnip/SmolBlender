#pragma once

#include "D3D11Definitions.hpp"
#include "Util.hpp"

namespace Smol
{
	class D3D11Buffer {
	private:
		BufferRAII buffer = nullptr;
		u32 size = 0;

		DeviceContext& context;

		SRVRAII srv = nullptr;
		UAVRAII uav = nullptr;

	public:
		D3D11Buffer(Device&, DeviceContext&, const BufferConfig&, const std::string& name);
		~D3D11Buffer() = default;

		DECLARE_MOVE_ONLY(D3D11Buffer)

		void upload(const void* src, u32 dataSize, u32 offset = 0);
		void download(void* dst, u32 destSize, u32 offset = 0);

		Buffer* get() const { return buffer.Get(); }
		SRV* getSRV() const { return srv.Get(); }
		UAV* getUAV() const { return uav.Get(); }
	};
}