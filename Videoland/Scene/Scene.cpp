#include <Videoland/Scene/Scene.h>

namespace Videoland {
Vec3 Spatial::Position() const {
    return m_position;
}

void Spatial::SetPosition(Vec3 position) {
    m_position = position;
}

bool Spatial::IsDirty() const {
    return m_transformDirty;
}

Mat4 Spatial::GlobalTransform() const {
    return m_globalTransform;
}

Mat4 Spatial::LocalTransform() const {
    return m_localTransform;
}
}
