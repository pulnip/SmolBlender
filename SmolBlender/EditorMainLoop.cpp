#include "EditorMainLoop.hpp"

#include "D3D11Device.hpp"
#include "OS.hpp"

namespace Smol
{
	EditorMainLoop::EditorMainLoop(const SwapchainConfig& cfg)
		: device()
		, swapchain(device.createSwapchain(cfg))
		, cmdList(device.createCommandList()) {}

	void EditorMainLoop::initialize() {
		pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.vertexShaderPath = L"vs.hlsl",
			.vertexShaderEntryPoint = "vs_main",
			.pixelShaderPath = L"ps.hlsl",
			.pixelShaderEntryPoint = "ps_main"
		});
	}

	bool EditorMainLoop::update(float deltaTime, float totalTime) {
		cmdList.begin();
		cmdList.beginRenderPass(swapchain);

		cmdList.setPipelineState(pipeline);

		cmdList.setViewport(D3D11_VIEWPORT{
			.TopLeftX = 0, .TopLeftY = 0,
			.Width = static_cast<FLOAT>(OS_.getWidth()), .Height = static_cast<FLOAT>(OS_.getHeight()),
			.MinDepth = 0, .MaxDepth = 1
			});

		cmdList.draw(3, 1);

		cmdList.endRenderPass();
		cmdList.close();

		device.submit(cmdList, &swapchain);

		return true;
	}

	void EditorMainLoop::finalize() {

	}
}