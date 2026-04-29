
#include <fstream>
#include <d3dcompiler.h>
#include "D3D11PipelineState.hpp"

namespace Smol
{
	std::vector<uint8_t> read_file_as_binary(const std::filesystem::path& path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path.string());

		auto size = file.tellg();
		file.seekg(0);

		std::vector<uint8_t> buffer(size);
		file.read(reinterpret_cast<char*>(buffer.data()), size);

		return buffer;
	}

	using Blob = ID3DBlob;
	using BlobRAII = RAII<Blob>;

	struct CompiledShader {
		std::vector<u8> bytecode;

		CompiledShader(
			const std::filesystem::path& path,
			const std::string& entryPoint,
			const char* target
		) {
			auto ext = std::filesystem::path(path).extension().string();

			if (ext == ".cso" || ext == ".dxbc" || ext == ".dxil") {
				bytecode = read_file_as_binary(path);
			}
			else if(ext != ".hlsl") {
				throw std::runtime_error("Unsupported shader file extension: " + ext);
			}

			UINT compileFlags = 0;
#if defined(_DEBUG) || !defined(NDEBUG)
			compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			BlobRAII shaderBlob, errorBlob;

			if (FAILED(D3DCompileFromFile(
				path.c_str(),
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				entryPoint.c_str(),
				target,
				compileFlags,
				0,
				&shaderBlob,
				&errorBlob
			))) {
				std::string errorMsg = "HLSL compile failed";
				if (errorBlob) {
					errorMsg += ": ";
					errorMsg += static_cast<const char*>(errorBlob->GetBufferPointer());
				}
				throw std::runtime_error(errorMsg);
			}

			bytecode.resize(shaderBlob->GetBufferSize());
			memcpy(bytecode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

			if (bytecode.empty()) {
				throw std::runtime_error("Shader bytecode is empty");
			}
		}

		const void* getBytecode() const {
			return bytecode.data();
		}
		SIZE_T getBytecodeLength() const {
			return bytecode.size();
		}
	};

	D3D11GraphicsPipelineState::D3D11GraphicsPipelineState(
		Device& device,
		const GraphicsPipelineConfig& cfg
	){
		auto vsBytecode = CompiledShader(cfg.vertexShaderPath, cfg.vertexShaderEntryPoint, "vs_5_0");
		if (FAILED(device.CreateVertexShader(
			vsBytecode.getBytecode(),
			vsBytecode.getBytecodeLength(),
			nullptr,
			&vertexShader
		))) {
			throw std::runtime_error("Failed to create vertex shader");
		}

		if (cfg.inputElementDescs.has_value()) {
			const auto& inputElementDescs = cfg.inputElementDescs.value();

			if (FAILED(device.CreateInputLayout(
				inputElementDescs.data(),
				static_cast<UINT>(inputElementDescs.size()),
				vsBytecode.getBytecode(),
				vsBytecode.getBytecodeLength(),
				&inputLayout
			))) {
				throw std::runtime_error("Failed to create input layout");
			}

			primitiveTopology = cfg.primitiveTopology;
		}

		if(FAILED(device.CreateRasterizerState(
			&cfg.rasterizerState,
			&rasterizerState
		))) {
			throw std::runtime_error("Failed to create rasterizer state");
		}

		auto psBytecode = CompiledShader(cfg.pixelShaderPath, cfg.pixelShaderEntryPoint, "ps_5_0");
		if(FAILED(device.CreatePixelShader(
			psBytecode.getBytecode(),
			psBytecode.getBytecodeLength(),
			nullptr,
			&pixelShader
		))) {
			throw std::runtime_error("Failed to create pixel shader");
		}

		if(cfg.depthStencilState.has_value()) {
			const auto& depthStencilStateDesc = cfg.depthStencilState.value();

			if(FAILED(device.CreateDepthStencilState(
				&depthStencilStateDesc,
				&depthStencilState
			))) {
				throw std::runtime_error("Failed to create depth stencil state");
			}
		}

		if (cfg.blendState.has_value()) {
			const auto& blendStateDesc = cfg.blendState.value();

			if (FAILED(device.CreateBlendState(
				&blendStateDesc,
				&blendState
			))) {
				throw std::runtime_error("Failed to create blend state");
			}
		}	
	}

	void D3D11GraphicsPipelineState::bind(DeviceContext& context) const {
		// (optional) Input Assembler
		if (inputLayout != nullptr) {
			context.IASetInputLayout(inputLayout.Get());
			context.IASetPrimitiveTopology(primitiveTopology);
		}

		// (necessary) Vertex Shader - Rasterizer State - Pixel Shader
		context.VSSetShader(vertexShader.Get(), nullptr, 0);
		context.RSSetState(rasterizerState.Get());
		context.PSSetShader(pixelShader.Get(), nullptr, 0);

		// (optional) Output Merger
		if (depthStencilState != nullptr) {
			context.OMSetDepthStencilState(depthStencilState.Get(), 0);
		}
		if (blendState != nullptr) {
			context.OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
		}
	}
}