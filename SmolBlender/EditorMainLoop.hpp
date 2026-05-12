#pragma once

#include <memory>

#include "MainLoop.hpp"
#include "Util.hpp"

#include "D3D11Device.hpp"

namespace Smol {
	class EditorMainLoop final: public MainLoop {
	private:
		D3D11Device device;
		D3D11Swapchain swapchain;
		D3D11CommandList cmdList;

		D3D11GraphicsPipelineState pipeline;

		D3D11Buffer vertexBuffer;

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