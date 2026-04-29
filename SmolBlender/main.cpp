#include <print>
#include "D3D11Device.hpp"
#include "OS.hpp"
#include "Util.hpp"

using namespace Smol;

int main(int argc, char* argv[]) {
	u32 width = 1280, height = 720;

	try {
		auto hwnd = createMyWindow(WindowConfig{
			L"SmolBlender", width, height
		});

		D3D11Device device;
		auto swapchain = device.createSwapchain(SwapchainConfig{
			.hwnd = hwnd,
			.width = width, .height = height
		});
		auto cmdList = device.createCommandList();

		auto pipeline = device.createPipelineState(GraphicsPipelineConfig{
			.vertexShaderPath = L"vs.hlsl",
			.vertexShaderEntryPoint = "vs_main",
			.pixelShaderPath = L"ps.hlsl",
			.pixelShaderEntryPoint = "ps_main"
		});

		bool isRunning = true;
		while (isRunning) {
			MSG msg{};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				switch (msg.message) {
				case WM_CLOSE:
					PostQuitMessage(0);
					break;
				case WM_QUIT:
					isRunning = false;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			cmdList.begin();
			cmdList.beginRenderPass(swapchain);

			cmdList.setPipelineState(pipeline);

			cmdList.setViewport(D3D11_VIEWPORT{
				.TopLeftX = 0, .TopLeftY = 0,
				.Width = static_cast<FLOAT>(width), .Height = static_cast<FLOAT>(height),
				.MinDepth = 0, .MaxDepth = 1
				});

			cmdList.draw(3, 1);

			cmdList.endRenderPass();
			cmdList.close();

			device.submit(cmdList, &swapchain);
		}
	}
	catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}