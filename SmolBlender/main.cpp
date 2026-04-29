#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <memory>
#include <print>
#include "EditorMainLoop.hpp"
#include "OS.hpp"

using namespace Smol;

int main(int argc, char* argv[]) {
	try {
		OS os(WindowConfig{
			L"SmolBlender",
			1280, 720
		});
		auto mainLoop = std::make_unique<EditorMainLoop>(
			SwapchainConfig{
				.hwnd = OS_.getWindow(),
				.width = OS_.getWidth(), .height = OS_.getHeight()
			}
		);
		OS_.setMainLoop(mainLoop.get());

		os.run();
	}
	catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}