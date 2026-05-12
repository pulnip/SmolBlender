#include <array>
#include "EditorMainLoop.hpp"

#include "D3D11Device.hpp"
#include "OS.hpp"
#include "Vertex.hpp"

namespace Smol
{
	EditorMainLoop::EditorMainLoop(const SwapchainConfig& cfg)
		: device()
		, swapchain(device.createSwapchain(cfg))
		, cmdList(device.createCommandList()) {}

	void EditorMainLoop::initialize() {
		pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.inputElementDescs = VERTEX1_INPUT_LAYOUT,
			.vertexShaderPath = L"vs.hlsl",
			.vertexShaderEntryPoint = "vs_main1",
			.pixelShaderPath = L"ps.hlsl",
			.pixelShaderEntryPoint = "ps_main"
		});

		using enum BufferUsage;

		// Notice! Counter-Clockwise for front face
		std::array vertices = {
			Vertex1{.position = {-0.6f,  0.8f}, .color = {1, 0, 0, 1}},
			Vertex1{.position = { 0.0f, -0.8f}, .color = {0, 1, 0, 1}},
			Vertex1{.position = { 0.6f,  0.8f}, .color = {0, 0, 1, 1}}
		};
		
		vertexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Vertex1) * vertices.size(),
			.usage = BufferUsage::VertexBuffer,
			.initialData = vertices.data()
		}, "Vertex Buffer");
	}

	void EditorMainLoop::processInput() {

	}

	bool EditorMainLoop::update(float deltaTime, float totalTime) {
		return true;
	}

	void EditorMainLoop::render() {
		cmdList.begin();
		cmdList.beginRenderPass(swapchain);

		cmdList.setPipelineState(pipeline);

		cmdList.setViewport(D3D11_VIEWPORT{
			.TopLeftX = 0, .TopLeftY = 0,
			.Width = static_cast<FLOAT>(OS_.getWidth()), .Height = static_cast<FLOAT>(OS_.getHeight()),
			.MinDepth = 0, .MaxDepth = 1
			}
		);

		cmdList.setVertexBuffer(vertexBuffer, 0, sizeof(Vertex1));
		cmdList.draw(3);

		cmdList.endRenderPass();
		cmdList.close();

		// Equivalent to swapchain.present();
		device.submit(cmdList, &swapchain);
	}

	void EditorMainLoop::finalize() {

	}
}