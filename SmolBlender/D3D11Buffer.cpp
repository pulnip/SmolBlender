#include <cassert>
#include <stdexcept>
#include "D3D11Buffer.hpp"
#include "Util.hpp"

namespace Smol
{
	D3D11Buffer::D3D11Buffer(
		Device& device,
		DeviceContext& context,
		const BufferConfig& cfg,
		const std::string& name
	)
		: context(context)
		, size(cfg.size)
	{
		using enum BufferUsage;
		using enum MemoryAccess;

		const auto hasVertexUsage = has_flag(cfg.usage, VertexBuffer);
		const auto hasIndexUsage = has_flag(cfg.usage, IndexBuffer);
		const auto hasConstantUsage = has_flag(cfg.usage, ConstantBuffer);
		const auto isShaderResource = has_flag(cfg.usage, AllowShaderRead);
		const auto isUnorderedAccess = has_flag(cfg.usage, AllowShaderWrite);

		const auto isCPUWrite = (cfg.access == CPUWrite);
		const auto isCPURead = (cfg.access == CPURead);

		UINT bindFlags = 0;
		if (hasVertexUsage) bindFlags |= D3D11_BIND_VERTEX_BUFFER;
		if (hasIndexUsage) bindFlags |= D3D11_BIND_INDEX_BUFFER;
		if (hasConstantUsage) bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
		if (isShaderResource) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (isUnorderedAccess) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		// DYNAMIC+UAV is invalid in D3D11;
		assert(!(isCPUWrite && isUnorderedAccess));

		const D3D11_BUFFER_DESC desc = {
			.ByteWidth = static_cast<UINT>(cfg.size),
			.Usage = isCPUWrite ?
				D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
			.BindFlags = bindFlags,
			.CPUAccessFlags = isCPUWrite ?
				D3D11_CPU_ACCESS_WRITE : UINT(0),
			.MiscFlags = 0,
			.StructureByteStride = 0
		};

		const D3D11_SUBRESOURCE_DATA initData{
				.pSysMem = cfg.initialData
		};

		if (FAILED(device.CreateBuffer(
			&desc,
			cfg.initialData != nullptr ? &initData : nullptr,
			&buffer
		))) {
			throw std::runtime_error("Failed to create D3D11 buffer");
		}

#if defined(_DEBUG) || !defined(NDEBUG)
		if (!name.empty()) {
			buffer->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				static_cast<UINT>(name.length()),
				name.c_str()
			);
		}
#endif

		if (isShaderResource) {
			const D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = DXGI_FORMAT_R32_FLOAT,
				.ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
				.Buffer = {
					.FirstElement = 0,
					.NumElements = static_cast<UINT>(cfg.size / sizeof(float)),
				}
			};
			device.CreateShaderResourceView(buffer.Get(), &srvDesc, &srv);
		}
		if(isUnorderedAccess) {
			const D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{
				.Format = DXGI_FORMAT_R32_FLOAT,
				.ViewDimension = D3D11_UAV_DIMENSION_BUFFER,
				.Buffer = {
					.FirstElement = 0,
					.NumElements = static_cast<UINT>(cfg.size / sizeof(float)),
					.Flags = 0
				}
			};
			device.CreateUnorderedAccessView(buffer.Get(), &uavDesc, &uav);
		}

		// Create a staging buffer for CPU readback when access == CPURead
		if (isCPURead) {
			D3D11_BUFFER_DESC stagingDesc{
				.ByteWidth = static_cast<UINT>(cfg.size),
				.Usage = D3D11_USAGE_STAGING,
				.BindFlags = 0,
				.CPUAccessFlags = D3D11_CPU_ACCESS_READ,
				.MiscFlags = 0,
				.StructureByteStride = 0
			};
			if (FAILED(device.CreateBuffer(&stagingDesc, nullptr, &stagingBuffer))) {
				throw std::runtime_error("Failed to create staging buffer for readback");
			}
		}
	}

	void D3D11Buffer::upload(const void* src, u32 dataSize, u32 offset) {
		assert(offset + dataSize <= size && "Upload range exceeds buffer size");
		D3D11_MAPPED_SUBRESOURCE mapped;
		context.Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

		std::memcpy(static_cast<u8*>(mapped.pData) + offset, src, dataSize);

		context.Unmap(buffer.Get(), 0);
	}

	void D3D11Buffer::download(void* dst, u32 dstSize, u32 offset) {
		assert(offset + dstSize <= size && "Download range exceeds buffer size");
		assert(stagingBuffer && "requires MemoryAccess::CPURead");

		// Copy GPU buffer ¡æ staging, then Map staging for CPU read
		const D3D11_BOX srcBox{
			.left = offset,
			.top = 0,
			.front = 0,
			.right = offset + dstSize,
			.bottom = 1,
			.back = 1
		};
		context.CopySubresourceRegion(
			stagingBuffer.Get(), 0,   // dst resource, dst subresource
			0, 0, 0,                   // dst x, y, z.
			buffer.Get(), 0,           // src resource, src subresource
			&srcBox
		);

		D3D11_MAPPED_SUBRESOURCE mapped;
		context.Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mapped);
	
		std::memcpy(dst, mapped.pData, dstSize);
	
		context.Unmap(stagingBuffer.Get(), 0);
	}
}