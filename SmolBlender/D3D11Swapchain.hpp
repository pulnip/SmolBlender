#pragma once

#include <cstdint>
#include "D3D11Definitions.hpp"

namespace Smol
{
	class D3D11Swapchain {
	private:
		SwapchainRAII swapchain = nullptr;
		uint32_t width = 0, height = 0;
		bool vsync, allowTearing;

		// Back buffer and RTV for rendering
		TextureRAII backBuffer = nullptr;
		RTVRAII rtv = nullptr;

		// Cached pointer to the device for resource creation
		Device* device = nullptr;

	public:
		D3D11Swapchain(Device&, Factory&, const SwapchainConfig&);
		~D3D11Swapchain() = default;

		DECLARE_MOVE_ONLY(D3D11Swapchain)

		void present();

		void resize(uint32_t newWidth, uint32_t newHeight);

		uint32_t getWidth() const { return width; }
		uint32_t getHeight() const { return height; }

		RTV* getRTV() const { return rtv.Get(); }

	private:
		void createBackBuffer();
	};
}