#pragma once

namespace RE {
    template <typename T1, typename T2>
    inline T1 camp(T1 x, T2 min, T2 max) {
        return (x < min) ? min : ((x > max) ? max : x);
    }

    inline float lerp(float x1, float x2, float t) {
        return x1 + (x2 - x1) * t;
    }

    // 只能用于x1和x2是常量表达式的情况
    // constexpr float campInterpConstexpr(float x1, float x2, float t) {
    //     if (x1 > x2) {
    //         return interp(x1, x2, camp<float>(t, 0, 1));
    //     } else {
    //         return interp(x1, x2, camp<float>(t, 0, 1));
    //     }
    // }

    inline float campInterp(float x1, float x2, float t) {
        return lerp(x1, x2, camp<float>(t, 0, 1));
    }

#ifdef GLM_ENABLE_EXPERIMENTAL
    inline glm::u8vec3 lerp(glm::u8vec3 x1, glm::u8vec3 x2, float t) {
        return glm::u8vec3{
            static_cast<uint8_t>(x1.r + (x2.r - x1.r) * t),
            static_cast<uint8_t>(x1.g + (x2.g - x1.g) * t),
            static_cast<uint8_t>(x1.b + (x2.b - x1.b) * t)};
    }
    inline glm::u64vec2 lerp(glm::u64vec2 x1, glm::u64vec2 x2, float t) {
        return glm::u64vec2{
            static_cast<uint64_t>(x1.x + (x2.x - x1.x) * t),
            static_cast<uint64_t>(x1.y + (x2.y - x1.y) * t)};
    }
#endif // GLM_ENABLE_EXPERIMENTAL
}