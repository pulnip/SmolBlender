#include <fstream>
#include "Util.hpp"

namespace Smol
{
	std::vector<u8> readFileAsBinary(const std::filesystem::path& path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path.string());

		auto size = file.tellg();
		file.seekg(0);

		std::vector<u8> buffer(size);
		file.read(reinterpret_cast<char*>(buffer.data()), size);

		return buffer;
	}
}