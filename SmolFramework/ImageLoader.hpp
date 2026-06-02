#pragma once

#include <filesystem>
#include <vector>
#include "Util.hpp"

namespace Smol
{
    struct ImageData {
        std::vector<u8> pixels;
        u32 width = 0;
        u32 height = 0;
        u32 channels = 0;

        inline bool isValid() const {
            return !pixels.empty() && width > 0 && height > 0;
        }
        inline size_t dataSize() const {
            return pixels.size();
        }
    };

    ImageData loadImage(const std::filesystem::path&);
}