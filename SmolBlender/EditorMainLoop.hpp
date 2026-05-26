#pragma once

#include <memory>
#include <vector>

#include "LinearAlgebra.hpp"
#include "MainLoop.hpp"
#include "Util.hpp"
#include "Vertex.hpp"

#include "D3D11Device.hpp"

namespace Smol {
	class EditorMainLoop final: public MainLoop {
	private:
		D3D11Device device;
		D3D11Swapchain swapchain;
		D3D11CommandList cmdList;

		D3D11GraphicsPipelineState pipeline;

		std::vector<Vertex1> vertices;
		D3D11Buffer vertexBuffer;

		Vec4 rotation = unitQuat();
		D3D11Buffer constantBuffer;

	public:
		EditorMainLoop(const SwapchainConfig&);
		~EditorMainLoop() = default;

		DECLARE_PINNED(EditorMainLoop)

		void initialize() override;

		void processInput() override;
		bool update(float deltaTime, float totalTime) override;
		void render() override;

		void finalize() override;
	};
}