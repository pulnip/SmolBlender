#pragma once

#include "MainLoop.hpp"
#include "Util.hpp"

#include "D3D11Device.hpp"

namespace Smol {
	class EditorMainLoop : public MainLoop {
	private:
		D3D11Device device;
		D3D11Swapchain swapchain;
		D3D11CommandList cmdList;

		D3D11GraphicsPipelineState pipeline;

	public:
		EditorMainLoop(const SwapchainConfig&);
		~EditorMainLoop() = default;

		DECLARE_PINNED(EditorMainLoop)

		virtual void initialize() override;
		virtual bool update(float deltaTime, float totalTime) override;
		virtual void finalize() override;
	};
}