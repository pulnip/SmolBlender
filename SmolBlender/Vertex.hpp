#pragma once

#include <array>
#include <d3d11.h>

namespace Smol
{
	struct Vertex1 {
		float position[2];
		float color[4];
	};

	constexpr std::array VERTEX1_INPUT_LAYOUT = {
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT, // float2 (8 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "COLOR",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, // float4 (16 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 8,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		}
	};

	struct Vertex2 {
		float position[3];
		float normal[3];
		float texcoord[2];
		float tangent[4];
	};

	constexpr std::array VERTEX2_INPUT_LAYOUT = {
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, // float3 (12 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT, // float3 (12 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 12,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT, // float2 (8 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 24,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
		D3D11_INPUT_ELEMENT_DESC{
			.SemanticName = "TANGENT",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT, // float4 (16 bytes)
			.InputSlot = 0,
			.AlignedByteOffset = 32,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		}
	};
}
