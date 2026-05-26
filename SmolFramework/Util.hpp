#pragma once

#include <cstdint>
#include <type_traits>

// copy - move semantics
#define DECLARE_DEFAULT_COPYABLE(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;
#define DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    Type(const Type&) noexcept = default; \
    Type& operator=(const Type&) noexcept = default;
#define DECLARE_DEFAULT_MOVABLE(Type) \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;
#define DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type) \
    Type(Type&&) noexcept = default; \
    Type& operator=(Type&&) noexcept = default;
#define DECLARE_NON_COPYABLE(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;
#define DECLARE_NON_MOVABLE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;

#define DECLARE_PINNED(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_COPY_ONLY(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_COPY_ONLY_NOEXCEPT(Type) \
    DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_MOVE_ONLY(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)
#define DECLARE_MOVE_ONLY_NOEXCEPT(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type)
#define DECLARE_TRANSFERABLE(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)
#define DECLARE_TRANSFERABLE_NOEXCEPT(Type) \
    DECLARE_DEFAULT_COPYABLE_NOEXCEPT(Type) \
    DECLARE_DEFAULT_MOVABLE_NOEXCEPT(Type)

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