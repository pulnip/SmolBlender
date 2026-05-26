#pragma once

#include "D3D11Definitions.hpp"

namespace Smol
{
	class D3D11GraphicsPipelineState {
	private:
		// (optional) Input Assembler
		InputLayoutRAII inputLayout = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		// Vertex Shader - Rasterizer State - Pixel Shader
		VertexShaderRAII vertexShader = nullptr;
		RasterizerStateRAII rasterizerState = nullptr;
		PixelShaderRAII pixelShader = nullptr;

		// (optional) Output Merger
		DepthStencilStateRAII depthStencilState = nullptr;
		BlendStateRAII blendState = nullptr;

	public:
		D3D11GraphicsPipelineState();
		D3D11GraphicsPipelineState(Device&, const GraphicsPipelineConfig&);
		~D3D11GraphicsPipelineState() = default;

		DECLARE_MOVE_ONLY(D3D11GraphicsPipelineState)

		void bind(DeviceContext&) const;
	};

	// TODO
	class D3D11ComputePipelineState {

	};
}