#include <stdexcept>
#include "INC_Windows.h"
#include <windowsx.h>
#include "MainLoop.hpp"
#include "OS.hpp"

LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE: {
		OS_.onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	} break;
	case WM_MBUTTONDOWN: {
		SetCapture(hwnd);
		OS_.onMidDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	} break;
	case WM_MBUTTONUP: {
		ReleaseCapture();
		OS_.onMidUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	} break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return NULL;
}

namespace Smol
{
	HWND createMyWindow(const WindowConfig& cfg, bool immediateShow) {
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

		RECT rect{ 0, 0, static_cast<LONG>(cfg.width), static_cast<LONG>(cfg.height) };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hwnd = CreateWindow(
			wc.lpszClassName,
			cfg.title,
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

		if (immediateShow) {
			ShowWindow(hwnd, SW_SHOW);
			UpdateWindow(hwnd);
		}

		return hwnd;
	}

	float Timer::tick() {
		auto now = Clock::now();
		std::chrono::duration<float> delta = now - lastTime;
		lastTime = now;
		return delta.count();
	}

	OS* OS::singleton = nullptr;

	OS::OS(const WindowConfig& cfg){
		if (singleton != nullptr) {
			throw std::runtime_error("OS must be singleton object");
		}

		singleton = this;

		hwnd = createMyWindow(cfg, false);
		width = cfg.width;
		height = cfg.height;

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
	}

	OS::~OS() {
		if (hwnd != nullptr) {
			DestroyWindow(hwnd);
			hwnd = nullptr;
		}

		singleton = nullptr;
	}

	void OS::run(MainLoop& mainLoop) {
		forceQuit = false;
		mainLoop.initialize();

		// initialize Timer
		timer.tick();

		auto totalTime = 0.0f;

		while (!forceQuit) {
			auto deltaTime = timer.tick();
			totalTime += deltaTime;

			processEvents();

			mainLoop.processInput();
			if (!mainLoop.update(deltaTime, totalTime))
				break;

			mainLoop.render();

			consumeFrameInput();
		}

		mainLoop.finalize();
	}

	void OS::processEvents() {
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			switch (msg.message) {
			case WM_QUIT:
				forceQuit = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}