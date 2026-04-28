#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <memory>
#include <print>
#include "EditorMainLoop.hpp"
#include "OS.hpp"

using namespace Smol;

int main_later(int argc, char* argv[]) {
	try {
		auto mainLoop = std::make_unique<EditorMainLoop>();

		OS os(WindowConfig{
			L"SmolBlender",
			1280, 720
		});
		os.setMainLoop(mainLoop.get());

		os.run();
	}
	catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}