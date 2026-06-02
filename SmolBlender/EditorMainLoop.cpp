#include <array>
#include <cmath>
#include "EditorMainLoop.hpp"

#include "D3D11Device.hpp"
#include "ImageLoader.hpp"
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
		// rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = FALSE;

		D3D11_BLEND_DESC blendDesc = DEFAULT_BLEND_DESC;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.inputElementDescs = VERTEX3_INPUT_LAYOUT,
			.vertexShaderPath = L"vs3.hlsl",
			.vertexShaderEntryPoint = "vs_main",
			.rasterizerState = rasterizerDesc,
			.pixelShaderPath = L"ps2.hlsl",
			.pixelShaderEntryPoint = "ps_textured",
			.blendState = blendDesc
		});
		
		std::array vertices = {
			Vertex3{.position = {-0.5f,  -0.5f}, .uv = {0, 1}, .color = {1, 0, 0, 1}},
			Vertex3{.position = {-0.5f,   0.5f}, .uv = {0, 0}, .color = {0, 1, 0, 1}},
			Vertex3{.position = { 0.5f,   0.5f}, .uv = {1, 0}, .color = {1, 1, 1, 1}},
			Vertex3{.position = { 0.5f,  -0.5f}, .uv = {1, 1}, .color = {0, 0, 1, 1}},
		};
		// Notice! Clockwise for front face
		std::array<uint16_t, 6> indices = {
			// Top-Left Triangle
			0, 1, 2,
			// Bottom-Right Triangle
			0, 2, 3
		};
		numIndices = indices.size();

		ImageData image = loadImage(L"butterfly.png");

		using enum BufferUsage;
		
		vertexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Vertex3) * vertices.size(),
			.usage = BufferUsage::VertexBuffer,
			.initialData = vertices.data()
		}, "Smol Vertex Buffer");
		indexBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(uint16_t) * indices.size(),
			.usage = BufferUsage::IndexBuffer,
			.initialData = indices.data()
		}, "Smol Index Buffer");
		constantBuffer = device.createBuffer(BufferConfig{
			.size = sizeof(Mat4),
			.usage = BufferUsage::ConstantBuffer,
			.access = MemoryAccess::CPUWrite
		}, "Smol Constant Buffer");

		diffuseTexture = device.createTexture(TextureConfig{
			.width = image.width, .height = image.height,
			.format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.usage = TextureUsage::AllowShaderRead,
			.initialData = image.pixels.data()
		}, "Diffuse from Crate1");
		sampler = device.createSampler(
			LINEAR_WRAP_SAMPLER_CONFIG,
			"Smol Sampler"
		);
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
		ClearColor clearColor{ 0.5, 0.5, 0.5, 1 };

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

		cmdList.setVertexBuffer(vertexBuffer, 0, sizeof(Vertex3));
		cmdList.setIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT);

		cmdList.setTexture(diffuseTexture, 0, ShaderStage::PixelShader);
		cmdList.setSampler(sampler, 0, ShaderStage::PixelShader);

		cmdList.drawIndexed(numIndices);

		cmdList.endRenderPass();
		cmdList.close();

		// Equivalent to swapchain.present();
		device.submit(cmdList, &swapchain);
	}

	void EditorMainLoop::finalize() {

	}
}