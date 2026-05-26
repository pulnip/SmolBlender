#include <memory>
#include <print>
#include "EditorMainLoop.hpp"
#include "OS.hpp"

using namespace Smol;

int main(int argc, char* argv[]) {
	try {
		OS os(WindowConfig{
			L"SmolBlender",
			800, 800
		});
		EditorMainLoop mainLoop(SwapchainConfig{
			.hwnd = OS_.getWindow(),
			.width = OS_.getWidth(), .height = OS_.getHeight()
		});

		os.run(mainLoop);
	}
	catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}