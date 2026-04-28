#include "D3D11Swapchain.hpp"

namespace Smol
{
	D3D11Swapchain::D3D11Swapchain(
		Device& device,
		Factory& factory,
		const SwapchainConfig& cfg
	)
		: device(&device)
		, width(cfg.width), height(cfg.height)
		, vsync(cfg.vsync), allowTearing(cfg.allowTearing)
	{
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
                .Width = cfg.width,
                .Height = cfg.height,
                .Format = cfg.format,
                .Stereo = FALSE,
                // No MSAA for swapchain
                .SampleDesc = {1, 0},
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = cfg.bufferCount,
                .Scaling = DXGI_SCALING_STRETCH,
                .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
                .Flags = cfg.allowTearing ?
                    DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : UINT(0)
        };

        factory.CreateSwapChainForHwnd(
            &device,
            cfg.hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapchain
        );
        createBackBuffer();


#if defined(_DEBUG) || !defined(NDEBUG)
        if (!cfg.debugName.empty()) {
            swapchain->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                static_cast<UINT>(cfg.debugName.length()),
                cfg.debugName.c_str()
            );
        }
#endif

	}
	
    void D3D11Swapchain::present() {
        UINT syncInterval = vsync ? 1 : 0;
        UINT flags = (!vsync && allowTearing) ?
            DXGI_PRESENT_ALLOW_TEARING : 0;

        swapchain->Present(syncInterval, flags);
	}

    void D3D11Swapchain::resize(uint32_t newWidth, uint32_t newHeight) {
        if (newWidth == width && newHeight == height) {
            return; // No need to resize
        }
        width = newWidth;
        height = newHeight;

        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);
        
        swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, desc.Flags);
        
        createBackBuffer();
	}

    void D3D11Swapchain::createBackBuffer() {
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(backBuffer.GetAddressOf())
        );
        device->CreateRenderTargetView(backBuffer.Get(), nullptr, &rtv);
    }
}