#pragma once

#include "D3D11Definitions.hpp"

namespace Smol
{
	class D3D11GraphicsPipelineState {
	private:
		InputLayoutRAII inputLayout = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		VertexShaderRAII vertexShader = nullptr;
		RasterizerStateRAII rasterizerState = nullptr;
		PixelShaderRAII pixelShader = nullptr;
		DepthStencilStateRAII depthStencilState = nullptr;
		BlendStateRAII blendState = nullptr;

	public:
		D3D11GraphicsPipelineState(Device&, const GraphicsPipelineConfig&);
		~D3D11GraphicsPipelineState() = default;

		void bind(DeviceContext&) const;
	};

	// TODO
	class D3D11ComputePipelineState {

	};
}