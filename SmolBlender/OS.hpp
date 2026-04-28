#pragma once

#include "INC_Windows.h"
#include "fwd.hpp"
#include "D3D11Device.hpp"

namespace Smol
{
	struct WindowConfig {
		const wchar_t* title;
		int width = 800, height = 600;
	};

	class OS {
	private:
		HWND hwnd = nullptr;
		int width, height;

		MainLoop* mainLoop = nullptr;
		bool forceQuit = false;

		// singleton
		static OS* instance;

		D3D11Device device;

	public:
		OS(const WindowConfig&);
		virtual ~OS();

		void run();
		void processEvents();

		inline static OS& singleton(){ return *instance; }

		HWND getWindow() const { return hwnd; }
		int getWidth() const { return width; }
		int getHeight() const {	return height; }

		void setMainLoop(MainLoop* mainLoop) { this->mainLoop = mainLoop; }

		D3D11Device& getDevice() { return device; }
	};

#define OS_ OS::singleton()
}
