#pragma once

namespace Videoland {
struct Vec3 {
    float x;
    float y;
    float z;

    Vec3() = default;

    constexpr Vec3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z) { }
};

struct Mat4 {
    float m[4][4];
    Mat4() = default;
};
}
