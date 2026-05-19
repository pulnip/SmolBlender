#pragma once

#include <chrono>
#include "INC_Windows.h"
#include "fwd.hpp"
#include "D3D11Device.hpp"

extern LRESULT CALLBACK MyWndProc(HWND, UINT, WPARAM, LPARAM);

namespace Smol
{
	struct WindowConfig {
		const wchar_t* title;
		u32 width = 800, height = 600;
	};

	HWND createMyWindow(const WindowConfig&, bool immediateShow = true);

	class Timer {
	private:
		using Clock = std::chrono::steady_clock;
		Clock::time_point lastTime = Clock::now();

	public:
		float tick();
	};

	class OS {
	private:
		HWND hwnd = nullptr;
		u32 width, height;

		bool forceQuit = false;

		static OS* singleton;

		Timer timer;

		struct MouseState {
			i32 x = 0, y = 0;
			i32 dx = 0, dy = 0;
			bool midDown = false;
			bool midPressed = false, midReleased = false;
		} mouse;

	public:
		OS(const WindowConfig&);
		virtual ~OS();

		void run(MainLoop& mainLoop);
		void processEvents();

		inline static OS& get(){ return *singleton; }

		HWND getWindow() const { return hwnd; }
		u32 getWidth() const { return width; }
		u32 getHeight() const {	return height; }

		MouseState getMouse() const { return mouse; }

	private:
		friend LRESULT CALLBACK ::MyWndProc(HWND, UINT, WPARAM, LPARAM);

		void onMouseMove(i32 x, i32 y) {
			mouse.dx += x - mouse.x;
			mouse.dy += y - mouse.y;
			mouse.x = x;
			mouse.y = y;
		}
		void onMidDown(int x, int y) {
			mouse.midDown = true;
			mouse.midPressed = true;
			mouse.x = x;
			mouse.y = y;
		}
		void onMidUp(int x, int y) {
			mouse.midDown = false;
			mouse.midReleased = true;
		}
		void consumeFrameInput() {
			mouse.dx = mouse.dy = 0;
			mouse.midPressed = mouse.midReleased = false;
		}
	};
}

#define OS_ Smol::OS::get()
