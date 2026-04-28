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
	{
		using enum BufferUsage;
		using enum MemoryAccess;

		const auto hasVertexUsage = has_flag(cfg.usage, VertexBuffer);
		const auto hasIndexUsage = has_flag(cfg.usage, IndexBuffer);
		const auto hasConstantUsage = has_flag(cfg.usage, ConstantBuffer);
		const auto needCPUAccess = hasVertexUsage || hasIndexUsage || hasConstantUsage || cfg.initialData != nullptr;

		const auto isGPUOnly = has_flag(cfg.access, GPUOnly);
		assert(!(isGPUOnly && needCPUAccess) && "GPU-only buffers cannot have CPU access or initial data");

		const auto isShaderResource = has_flag(cfg.usage, AllowShaderRead);
		const auto isUnorderedAccess = has_flag(cfg.usage, AllowShaderWrite);

		UINT bindFlags = 0;
		if (hasVertexUsage) bindFlags |= D3D11_BIND_VERTEX_BUFFER;
		if (hasIndexUsage) bindFlags |= D3D11_BIND_INDEX_BUFFER;
		if (hasConstantUsage) bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
		if (isShaderResource) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (isUnorderedAccess) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

		const D3D11_BUFFER_DESC desc = {
			.ByteWidth = static_cast<UINT>(cfg.size),
			.Usage = isGPUOnly ?
				D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC,
			.BindFlags = bindFlags,
			.CPUAccessFlags = isGPUOnly ?
				UINT(0) : D3D11_CPU_ACCESS_WRITE,
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
	}

	void D3D11Buffer::upload(const void* src, u32 dataSize, u32 offset) {
		assert(offset + dataSize <= size && "Upload range exceeds buffer size");
		D3D11_MAPPED_SUBRESOURCE mapped;
		context.Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

		std::memcpy(static_cast<u8*>(mapped.pData) + offset, src, dataSize);

		context.Unmap(buffer.Get(), 0);
	}

	void D3D11Buffer::download(void* dst, u32 destSize, u32 offset) {
		assert(offset + destSize <= size && "Download range exceeds buffer size");
		D3D11_MAPPED_SUBRESOURCE mapped;
		context.Map(buffer.Get(), 0, D3D11_MAP_READ, 0, &mapped);
	
		std::memcpy(dst, static_cast<u8*>(mapped.pData) + offset, destSize);
	
		context.Unmap(buffer.Get(), 0);
	}
}