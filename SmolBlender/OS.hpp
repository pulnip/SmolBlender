#pragma once

#include "INC_Windows.h"
#include "fwd.hpp"
#include "D3D11Device.hpp"

namespace Smol
{
	struct WindowConfig {
		const wchar_t* title;
		u32 width = 800, height = 600;
	};

	HWND createMyWindow(const WindowConfig&, bool immediateShow = true);

	class OS {
	private:
		HWND hwnd = nullptr;
		u32 width, height;

		MainLoop* mainLoop = nullptr;
		bool forceQuit = false;

		// singleton
		static OS* instance;

	public:
		OS(const WindowConfig&);
		virtual ~OS();

		void run();
		void processEvents();

		inline static OS& singleton(){ return *instance; }

		HWND getWindow() const { return hwnd; }
		u32 getWidth() const { return width; }
		u32 getHeight() const {	return height; }

		void setMainLoop(MainLoop* mainLoop) { this->mainLoop = mainLoop; }
	};

#define OS_ OS::singleton()
}
