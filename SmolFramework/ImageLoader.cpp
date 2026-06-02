#include <cstring>
#include <print>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ImageLoader.hpp"

namespace {
	std::string toUtf8String(const std::filesystem::path& path) {
#ifdef __cpp_char8_t
		auto u8str = path.u8string();
		return std::string(u8str.begin(), u8str.end());
#else
		return path.u8string();
#endif
	}
}

template<>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
	inline auto format(const std::filesystem::path& p, auto& ctx) const {
		auto u8str = toUtf8String(p);
		return std::formatter<std::string>::format(u8str, ctx);
	}
};

namespace Smol
{
    ImageData loadImage(const std::filesystem::path& path) {
		auto blob = readFileAsBinary(path);

		int width = 0, height = 0, channels = 0;
		auto data = stbi_load_from_memory(
			blob.data(), static_cast<int>(blob.size()),
			&width, &height, &channels,
			STBI_rgb_alpha
		);

		if (data == nullptr) {
			std::println("Image load Failed: {} - {}", path, stbi_failure_reason());
			return {};
		}

		auto size = width * height * 4;
		std::vector<u8> pixels(size);
		memcpy(pixels.data(), data, size);

		stbi_image_free(data);

		return ImageData{
			.pixels = pixels,
			.width = static_cast<u32>(width),
			.height = static_cast<u32>(height),
			.channels = static_cast<u32>(channels)
		};
    }
}