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
		D3D11_RASTERIZER_DESC rasterizerDesc = DEFAULT_RASTERIZER_DESC;
		// rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = FALSE;

		pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.inputElementDescs = VERTEX1_INPUT_LAYOUT,
			.vertexShaderPath = L"vs.hlsl",
			.vertexShaderEntryPoint = "vs_main1",
			.rasterizerState = rasterizerDesc,
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
			.size = MAT_BYTE_SIZE,
			.usage = BufferUsage::ConstantBuffer,
			.access = MemoryAccess::CPUWrite
		}, "Constant Buffer");
	}

	void EditorMainLoop::processInput() {
		auto m = OS_.getMouse();

		bool isMouseMoved = m.dx != 0 || m.dy != 0;
		if (m.midDown && isMouseMoved) {
			Vec2 mv = normalize(Vec2(-m.dy, -m.dx));
			Vec4 delta = axisAngle(toVec3(mv), 0.1);
			rotation = normalize(quatMul(delta, rotation));
		}
	}

	bool EditorMainLoop::update(float deltaTime, float totalTime) {
		return true;
	}

	void EditorMainLoop::render() {
		ClearColor clearColor{ 0, 0, 0, 1 };

		cmdList.begin();
		cmdList.beginRenderPass(
			swapchain,
			&clearColor
		);

		Mat4 mat = rotateMat(rotation);
		constantBuffer.upload(mat.data(), MAT_BYTE_SIZE);

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