#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <type_traits>

namespace Smol
{
    struct Vec3;
    struct Vec4;

    // column vector
    struct Vec2 {
        float x = 0.0f, y = 0.0f;

        constexpr float& operator[](size_t i) {
            switch (i) {
            case 0: return x;
            case 1: return y;
            default:
                assert(false && "Vec2 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const {
            switch (i) {
            case 0: return x;
            case 1: return y;
            default:
                assert(false && "Vec2 index out of range");
                std::unreachable();
            }
        }

        constexpr operator Vec3() const noexcept;
        constexpr operator Vec4() const noexcept;
    };
    static_assert(std::is_trivially_copyable_v<Vec2>);

    // column vector
    struct Vec3 {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        constexpr float& operator[](size_t i) {
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                assert(false && "Vec3 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const {
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                assert(false && "Vec3 index out of range");
                std::unreachable();
            }
        }

        constexpr explicit operator Vec2() const noexcept {
            return { x, y };
        }
        constexpr operator Vec4() const noexcept;
    };
    static_assert(std::is_trivially_copyable_v<Vec3>);

    // column vector
    struct Vec4 {
        float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

        constexpr float& operator[](size_t i) {
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:
                assert(false && "Vec4 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const {
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:
                assert(false && "Vec4 index out of range");
                std::unreachable();
            }
        }

        constexpr explicit operator Vec2() const noexcept {
            return { x, y };
        }
        constexpr explicit operator Vec3() const noexcept {
            return { x, y, z };
        }
    };
    static_assert(std::is_trivially_copyable_v<Vec4>);

    inline constexpr Vec2::operator Vec3() const noexcept {
        return { x, y, 0.0f };
    }
    inline constexpr Vec2::operator Vec4() const noexcept {
        return { x, y, 0.0f, 0.0f };
    }

    inline constexpr Vec3::operator Vec4() const noexcept {
        return { x, y, z, 0.0f };
    }

    inline constexpr Vec3 toVec3(Vec2 v, float z = 0.0f) {
        return { v.x, v.y, z };
    }
    inline constexpr Vec4 toVec4(Vec2 v, float z = 0.0f, float w = 0.0f) {
        return { v.x, v.y, z, w };
    }
    inline constexpr Vec4 toVec4(Vec3 v, float w = 0.0f) {
        return { v.x, v.y, v.z, w };
    }


    // column-major matrix
    using Mat4 = std::array<Vec4, 4>;
    static_assert(std::is_trivially_copyable_v<Mat4>);
    static_assert(sizeof(Mat4) == sizeof(float) * 16);

    inline constexpr Vec2 operator+(Vec2 lhs, Vec2 rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }
    inline constexpr Vec2& operator+=(Vec2& lhs, Vec2 rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator-(Vec2 v) {
        return { -v.x, -v.y };
    }
    inline constexpr Vec2 operator-(Vec2 lhs, Vec2 rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }
    inline constexpr Vec2& operator-=(Vec2& lhs, Vec2 rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(Vec2 lhs, Vec2 rhs) {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }
    inline constexpr Vec2& operator*=(Vec2& lhs, Vec2 rhs) {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(float f, Vec2 v) {
        return { f * v.x, f * v.y };
    }
    inline constexpr Vec2 operator*(Vec2 v, float f) {
        return f * v;
    }
    inline constexpr Vec2& operator*=(Vec2& v, float f) {
        v.x *= f;
        v.y *= f;
        return v;
    }
    inline constexpr Vec2 operator/(Vec2 v, float f) {
        return { v.x / f, v.y / f };
    }
    inline constexpr Vec2& operator/=(Vec2& v, float f) {
        v.x /= f;
        v.y /= f;
        return v;
    }

    inline constexpr bool operator==(Vec2 lhs, Vec2 rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline constexpr float dot(Vec2 lhs, Vec2 rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }
    inline constexpr float normSquared(Vec2 v) {
        return dot(v, v);
    }
    inline float norm(Vec2 v) {
        return std::sqrt(normSquared(v));
    }
    inline Vec2 normalize(Vec2 v) {
        return v / norm(v);
    }
    inline constexpr float cross(Vec2 lhs, Vec2 rhs) {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

    inline constexpr Vec3 zeros() {
        return { 0.0f, 0.0f, 0.0f };
    }
    inline constexpr Vec3 ones() {
        return { 1.0f, 1.0f, 1.0f };
    }
    inline constexpr Vec3 unitX() {
        return { 1.0f, 0.0f, 0.0f };
    }
    inline constexpr Vec3 unitY() {
        return { 0.0f, 1.0f, 0.0f };
    }
    inline constexpr Vec3 unitZ() {
        return { 0.0f, 0.0f, 1.0f };
    }

    inline constexpr Mat4 unitMat() {
        return {
            Vec4{1.0f, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f, 1.0f, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f, 1.0f, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr Vec3 operator+(Vec3 lhs, Vec3 rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }
    inline constexpr Vec3& operator+=(Vec3& lhs, Vec3 rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator-(Vec3 v) {
        return { -v.x, -v.y, -v.z };
    }
    inline constexpr Vec3 operator-(Vec3 lhs, Vec3 rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }
    inline constexpr Vec3& operator-=(Vec3& lhs, Vec3 rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(Vec3 lhs, Vec3 rhs) {
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }
    inline constexpr Vec3& operator*=(Vec3& lhs, Vec3 rhs) {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(float f, Vec3 v) {
        return {
            f * v.x,
            f * v.y,
            f * v.z
        };
    }
    inline constexpr Vec3 operator*(Vec3 v, float f) {
        return f * v;
    }
    inline constexpr Vec3& operator*=(Vec3& v, float f) {
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    inline constexpr Vec3 operator/(Vec3 v, float f) {
        return {
            v.x / f,
            v.y / f,
            v.z / f
        };
    }
    inline constexpr Vec3& operator/=(Vec3& v, float f) {
        v.x /= f;
        v.y /= f;
        v.z /= f;
        return v;
    }

    inline constexpr bool operator==(Vec3 lhs, Vec3 rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    inline constexpr float dot(Vec3 lhs, Vec3 rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
    inline constexpr float normSquared(Vec3 v) {
        return dot(v, v);
    }
    inline float norm(Vec3 v) {
        return std::sqrt(normSquared(v));
    }
    inline Vec3 normalize(Vec3 v) {
        return v / norm(v);
    }
    inline constexpr Vec3 cross(Vec3 lhs, Vec3 rhs) {
        return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }

    inline constexpr Vec4 unitQuat() {
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    inline constexpr Mat4 transpose(const Mat4 mat) {
        return Mat4{
            Vec4{mat[0][0], mat[1][0], mat[2][0], mat[3][0]},
            Vec4{mat[0][1], mat[1][1], mat[2][1], mat[3][1]},
            Vec4{mat[0][2], mat[1][2], mat[2][2], mat[3][2]},
            Vec4{mat[0][3], mat[1][3], mat[2][3], mat[3][3]}
        };
    }

    inline constexpr Vec4 conjugate(Vec4 q) {
        return { -q.x, -q.y, -q.z, q.w };
    }

    inline constexpr Vec4 operator+(Vec4 lhs, Vec4 rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
    }
    inline constexpr Vec4& operator+=(Vec4& lhs, Vec4 rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        lhs.w += rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator-(Vec4 v) {
        return { -v.x, -v.y, -v.z, -v.w };
    }
    inline constexpr Vec4 operator-(Vec4 lhs, Vec4 rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
    }
    inline constexpr Vec4& operator-=(Vec4& lhs, Vec4 rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        lhs.w -= rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator*(Vec4 lhs, Vec4 rhs) {
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
            lhs.w * rhs.w
        };
    }
    inline constexpr Vec4& operator*=(Vec4& lhs, Vec4 rhs) {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        lhs.w *= rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator*(float f, Vec4 v) {
        return {
            f * v.x,
            f * v.y,
            f * v.z,
            f * v.w
        };
    }
    inline constexpr Vec4 operator*(Vec4 v, float f) {
        return f * v;
    }
    inline constexpr Vec4& operator*=(Vec4& v, float f) {
        v.x *= f;
        v.y *= f;
        v.z *= f;
        v.w *= f;
        return v;
    }
    inline constexpr Vec4 operator/(Vec4 v, float f) {
        return {
            v.x / f,
            v.y / f,
            v.z / f,
            v.w / f,
        };
    }
    inline constexpr Vec4& operator/=(Vec4& v, float f) {
        v.x /= f;
        v.y /= f;
        v.z /= f;
        v.w /= f;
        return v;
    }
    inline constexpr float dot(Vec4 lhs, Vec4 rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }
    inline constexpr float normSquared(Vec4 v) {
        return dot(v, v);
    }
    inline float norm(Vec4 v) {
        return std::sqrt(normSquared(v));
    }
    inline Vec4 normalize(Vec4 v) {
        return v / norm(v);
    }
    inline constexpr Vec4 quatMul(Vec4 lhs, Vec4 rhs) {
        return Vec4{
            lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
            lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
            lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
        };
    }
    inline Vec4 toQuat(Vec3 r, Vec3 u, Vec3 f) {
        float m00 = r.x, m01 = u.x, m02 = f.x;
        float m10 = r.y, m11 = u.y, m12 = f.y;
        float m20 = r.z, m21 = u.z, m22 = f.z;

        float trace = m00 + m11 + m22;
        Vec4 q;

        if (trace > 0.0f) {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            q.x = (m21 - m12) / s;
            q.y = (m02 - m20) / s;
            q.z = (m10 - m01) / s;
            q.w = 0.25f * s;
        }
        else if ((m00 > m11) && (m00 > m22)) {
            float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
            q.x = 0.25f * s;
            q.y = (m01 + m10) / s;
            q.z = (m02 + m20) / s;
            q.w = (m21 - m12) / s;
        }
        else if (m11 > m22) {
            float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
            q.x = (m01 + m10) / s;
            q.y = 0.25f * s;
            q.z = (m12 + m21) / s;
            q.w = (m02 - m20) / s;
        }
        else {
            float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // s=4*q.z
            q.x = (m02 + m20) / s;
            q.y = (m12 + m21) / s;
            q.z = 0.25f * s;
            q.w = (m10 - m01) / s;
        }

        return normalize(q);
    }

    inline Vec4 rotateX(float theta) {
        float half = theta * 0.5f;
        return{
            std::sin(half),
            0.0f,
            0.0f,
            std::cos(half)
        };
    }
    inline Vec4 rotateY(float theta) {
        float half = theta * 0.5f;
        return{
            0.0f,
            std::sin(half),
            0.0f,
            std::cos(half)
        };
    }
    inline Vec4 rotateZ(float theta) {
        float half = theta * 0.5f;
        return{
            0.0f,
            0.0f,
            std::sin(half),
            std::cos(half)
        };
    }
    inline auto axisAngle(Vec3 axis, float radian) {
        auto half = radian / 2;
        float s = std::sin(half);
        return Vec4{
            axis.x * s,
            axis.y * s,
            axis.z * s,
            std::cos(half)
        };
    }
    inline constexpr auto rotate(Vec4 v, Vec4 q) {
        return quatMul(quatMul(q, v), conjugate(q));
    }

    // expected multiplication form
    inline constexpr Vec4 operator*(const Mat4& lhs, const Vec4& rhs) {
        return lhs[0] * rhs.x + lhs[1] * rhs.y + lhs[2] * rhs.z + lhs[3] * rhs.w;
    }

    inline constexpr auto operator*(const Mat4& lhs, const Mat4& rhs) {
        return Mat4{ lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3] };
    }

    inline auto rotateXMat(float theta) {
        float c = std::cos(theta);
        float s = std::sin(theta);
        // column-major
        return Mat4{
            Vec4{1.0f, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f,    c,    s, 0.0f},
            Vec4{0.0f,   -s,    c, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline auto rotateYMat(float theta) {
        float c = std::cos(theta);
        float s = std::sin(theta);
        // column-major
        return Mat4{
            Vec4{   c, 0.0f,   -s, 0.0f},
            Vec4{0.0f, 1.0f, 0.0f, 0.0f},
            Vec4{   s, 0.0f,    c, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline auto rotateZMat(float theta) {
        float c = std::cos(theta);
        float s = std::sin(theta);
        // column-major
        return Mat4{
            Vec4{   c,    s, 0.0f, 0.0f},
            Vec4{  -s,    c, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f, 1.0f, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr auto translateMat(Vec3 t) {
        // column-major
        return Mat4{
            Vec4{1.0f, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f, 1.0f, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f, 1.0f, 0.0f},
            Vec4{ t.x,  t.y,  t.z, 1.0f}
        };
    }

    inline constexpr auto rotateMat(Vec4 q) {
        float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
        float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
        float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;
        // column-major
        return Mat4{
            Vec4{1 - 2 * (yy + zz),   2 * (xy + wz),   2 * (xz - wy), 0.0f},
            Vec4{  2 * (xy - wz), 1 - 2 * (xx + zz),   2 * (yz + wx), 0.0f},
            Vec4{  2 * (xz + wy),   2 * (yz - wx), 1 - 2 * (xx + yy), 0.0f},
            Vec4{       0.0f,        0.0f,        0.0f, 1.0f}
        };
    }

    inline constexpr auto scaleMat(Vec3 s) {
        return Mat4{
            Vec4{ s.x, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f,  s.y, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f,  s.z, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr double toRadian(double degree) noexcept {
        return degree * (std::numbers::pi / 180.0);
    }
}
