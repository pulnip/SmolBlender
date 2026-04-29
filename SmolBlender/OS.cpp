#include <stdexcept>
#include "INC_Windows.h"
#include "MainLoop.hpp"
#include "OS.hpp"

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

	OS* OS::instance = nullptr;

	OS::OS(const WindowConfig& cfg){
		if (instance != nullptr) {
			throw std::runtime_error("OS must be singleton object");
		}

		instance = this;

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

		instance = nullptr;
	}

	void OS::run() {
		if (mainLoop == nullptr) return;

		forceQuit = false;
		mainLoop->initialize();

		while (!forceQuit) {
			processEvents();

			if (!mainLoop->update(1.0f / 60, 0.0f))
				break;
		}

		mainLoop->finalize();
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