#include <csignal>
#include <print>
#include "D3D11Device.hpp"
#include "INC_Windows.h"
#include "Util.hpp"

using namespace Smol;

static LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return NULL;
}

static HWND createMyWindow(const wchar_t* title, u32 width, u32 height) {
	WNDCLASSEX wc{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		MyWndProc,
		0L, 0L,
		GetModuleHandle(NULL),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		L"SmolBlenderWindowClass", // lpszClassName
		nullptr
	};

	RegisterClassEx(&wc);

	RECT rect{ 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"SmolBlender",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);
	if (hwnd == nullptr) {
		throw std::runtime_error("Failed to create window");
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return hwnd;
}

int main(int argc, char* argv[]) {
	u32 width = 1280, height = 720;

	try{
		auto hwnd = createMyWindow(L"SmolBlender", width, height);

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