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
			bool leftDown = false;
			bool leftPressed = false, leftReleased = false;
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
		void onMouseLeftDown(int x, int y) {
			mouse.leftDown = true;
			mouse.leftPressed = true;
			mouse.x = x;
			mouse.y = y;
		}
		void onMouseLeftUp(int x, int y) {
			mouse.leftDown = false;
			mouse.leftReleased = true;
		}
		void consumeFrameInput() {
			mouse.dx = mouse.dy = 0;
			mouse.leftPressed = mouse.leftReleased = false;
		}
	};
}

#define OS_ Smol::OS::get()
