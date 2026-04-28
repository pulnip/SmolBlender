#pragma once

#include <cstdint>
#include <type_traits>

namespace Smol
{
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;

    template<typename T>
    concept enum_type = std::is_enum_v<T>;

    template<enum_type E>
    constexpr bool has_flag(E flags, E test) {
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) != 0;
    }

    template<enum_type E>
    constexpr bool has_all(E flags, E test) {
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) == static_cast<U>(test);
    }

    template<enum_type E, enum_type... Es>
    constexpr E combine(E first, Es... rest) {
        using U = std::underlying_type_t<E>;
        return static_cast<E>((
            static_cast<U>(first) | ... | static_cast<U>(rest)
        ));
    }
}