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
		, swapchain(device.createSwapchain(cfg, "Smol Swapchain"))
		, cmdList(device.createCommandList()) {}

	void EditorMainLoop::initialize() {
		D3D11_RASTERIZER_DESC rasterizerDesc = DEFAULT_RASTERIZER_DESC;
		// enable culling for winding order test
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = FALSE;

		pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.inputElementDescs = VERTEX2_INPUT_LAYOUT,
			.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			.vertexShaderPath = L"vs2.hlsl",
			.vertexShaderEntryPoint = "vs_main",
			.rasterizerState = rasterizerDesc,
			.pixelShaderPath = L"ps1.hlsl",
			.pixelShaderEntryPoint = "ps_main"
		});

		std::array vertices = {
			Vertex2{.position = {-0.5f,  -0.5f, -0.5f}, .color = {1, 1, 1, 1}},
			Vertex2{.position = { 0.5f,  -0.5f, -0.5f}, .color = {0, 1, 1, 1}},
			Vertex2{.position = {-0.5f,   0.5f, -0.5f}, .color = {1, 0, 1, 1}},
			Vertex2{.position = { 0.5f,   0.5f, -0.5f}, .color = {0, 0, 1, 1}},
			Vertex2{.position = {-0.5f,  -0.5f,  0.5f}, .color = {0, 0, 0, 1}},
			Vertex2{.position = { 0.5f,  -0.5f,  0.5f}, .color = {1, 0, 0, 1}},
			Vertex2{.position = {-0.5f,   0.5f,  0.5f}, .color = {0, 1, 0, 1}},
			Vertex2{.position = { 0.5f,   0.5f,  0.5f}, .color = {1, 1, 0, 1}}
		};

		// Notice! Clockwise for front face
		std::array<u16, 36> indices = {
			0, 2, 1, 1, 2, 3, // front
			0, 1, 4, 1, 5, 4, // bottom
			0, 4, 2, 4, 6, 2, // left
			1, 3, 5, 3, 7, 5, // right
			3, 2, 6, 3, 6, 7, // top
			4, 5, 6, 5, 7, 6  // back
		};
		numIndices = indices.size();

		using enum BufferUsage;

		vertexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Vertex2) * vertices.size(),
			.usage = BufferUsage::VertexBuffer,
			.initialData = vertices.data()
		}, "Smol Vertex Buffer");
		indexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(u16) * indices.size(),
			.usage = BufferUsage::IndexBuffer,
			.initialData = indices.data()
		}, "Smol Index Buffer");

		constantBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Mat4),
			.usage = BufferUsage::ConstantBuffer,
			.access = MemoryAccess::CPUWrite
		}, "Smol Constant Buffer");
	}

	void EditorMainLoop::processInput() {
		auto m = OS_.getMouse();

		bool isMouseMoved = m.dx != 0 || m.dy != 0;
		if (m.leftDown && isMouseMoved) {
			Vec2 mv(-m.dy, -m.dx);
			Vec4 delta = axisAngle(toVec3(normalize(mv)), norm(mv)/100);
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
		constantBuffer.upload(mat.data(), sizeof(mat));

		cmdList.setPipelineState(pipeline);

		cmdList.setViewport(D3D11_VIEWPORT{
			.TopLeftX = 0, .TopLeftY = 0,
			.Width = static_cast<FLOAT>(OS_.getWidth()), .Height = static_cast<FLOAT>(OS_.getHeight()),
			.MinDepth = 0, .MaxDepth = 1
			}
		);

		cmdList.setConstantBuffer(constantBuffer, 0, ShaderStage::VertexShader);

		cmdList.setVertexBuffer(vertexBuffer, 0, sizeof(Vertex2));
		cmdList.setIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT);
		// cmdList.draw(numVertices);
		cmdList.drawIndexed(numIndices);

		cmdList.endRenderPass();
		cmdList.close();

		// Equivalent to swapchain.present();
		device.submit(cmdList, &swapchain);
	}

	void EditorMainLoop::finalize() {

	}
}