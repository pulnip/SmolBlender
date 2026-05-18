#include <array>
#include <cmath>
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
		auto x = 0.8f * std::sqrtf(3)/2;
		std::array vertices = {
			Vertex1{.position = {0.0f,  0.8f}, .color = {1, 0, 0, 1}},
			Vertex1{.position = {  -x, -0.4f}, .color = {0, 1, 0, 1}},
			Vertex1{.position = {   x, -0.4f}, .color = {0, 0, 1, 1}}
		};
		
		vertexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Vertex1) * vertices.size(),
			.usage = BufferUsage::VertexBuffer,
			.initialData = vertices.data()
		}, "Vertex Buffer");
		constantBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(float) * mat.size(),
			.usage = BufferUsage::ConstantBuffer,
			.access = MemoryAccess::CPUWrite
		}, "Constant Buffer");
	}

	void EditorMainLoop::processInput() {
		const auto mouse = OS_.getMouse();

		bool isMouseMoved = mouse.dx != 0 || mouse.dy != 0;
		if (mouse.midDown && isMouseMoved) {
			printf("asd");
		}
	}

	void EditorMainLoop::fillMatFromTheta(float theta) {
		float cost = std::cos(theta), sint = std::sin(theta);

		mat = {
			 cost, sint, 0.0f, 0.0f,
			-sint, cost, 0.0f, 0.0f,
			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	bool EditorMainLoop::update(float deltaTime, float totalTime) {
		fillMatFromTheta(totalTime);

		return true;
	}

	void EditorMainLoop::render() {
		ClearColor clearColor{ 0, 0, 0, 1 };

		cmdList.begin();
		cmdList.beginRenderPass(
			swapchain,
			&clearColor
		);

		auto matSize = static_cast<u32>(sizeof(float) * mat.size());
		constantBuffer.upload(mat.data(), matSize);

		cmdList.setPipelineState(pipeline);

		cmdList.setViewport(D3D11_VIEWPORT{
			.TopLeftX = 0, .TopLeftY = 0,
			.Width = static_cast<FLOAT>(OS_.getWidth()), .Height = static_cast<FLOAT>(OS_.getHeight()),
			.MinDepth = 0, .MaxDepth = 1
			}
		);

		cmdList.setConstantBuffer(constantBuffer, 0, ShaderStage::VertexShader);

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