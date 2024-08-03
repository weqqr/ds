#pragma once

#include <Videoland/Math/Math.h>
#include <expected>
#include <string>
#include <vector>

namespace Videoland {
template<typename V>
concept IsVertex = requires(V vertex, std::vector<float> v) {
    vertex.append(v);
};

class ModelVertex {
public:
    void append(std::vector<float>& data);
};

class Mesh {
public:
    Mesh() = default;

    template<typename V>
        requires IsVertex<V>
    void append(const V& vertex) {
        vertex.append(m_vertices);
    }

private:
    std::vector<float> m_vertices;
    // std::vector<uint32_t> m_indices;
};

class Model {
public:
    Model() = default;

    static std::expected<Model, int> Load(const std::string& path);

private:
};
}
