#pragma once

#include <Videoland/Collections/Pool.h>
#include <Videoland/Math/Math.h>
#include <memory>

namespace Videoland {
class Spatial {
public:
    Spatial() = default;

    Vec3 Position() const;
    void SetPosition(Vec3 position);

    bool IsDirty() const;

    Mat4 GlobalTransform() const;
    Mat4 LocalTransform() const;

private:
    Vec3 m_position{};
    bool m_transformDirty{};
    Mat4 m_localTransform{};
    Mat4 m_globalTransform{};
};

class Camera : public Spatial {
public:
private:
};

class SceneGraph {
public:
private:
    Pool<std::unique_ptr<Spatial>> m_pool;
};
}
