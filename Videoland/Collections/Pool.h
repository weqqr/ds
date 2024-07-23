#pragma once

#include <optional>
#include <vector>

namespace Videoland {
template<typename Value>
class Pool;

template<typename Value>
class Handle {
    friend class Pool<Value>;

private:
    Handle(uint32_t index, uint32_t generation)
        : m_index(index)
        , m_generation(generation) { }

    uint32_t m_index;
    uint32_t m_generation;
};

namespace Impl {
template<typename Value>
class PoolCell {
    friend class Pool<Value>;

private:
    PoolCell(Value&& value)
        : m_value(std::forward<Value>(value)) { }

    std::optional<Value> m_value = {};
    uint32_t m_generation = 0;
};
}

template<typename Value>
class Pool {
public:
    Pool() = default;

    Handle<Value> Insert(Value&& value) {
        if (m_free_cells.empty()) {
            uint32_t index = uint32_t(m_cells.size());
            m_cells.push_back(std::forward<Value>(value));

            return Handle<Value>(index, 0);
        } else {
            uint32_t index = m_free_cells.back();
            m_free_cells.pop_back();
            auto& cell = m_cells.at(index);
            cell.m_value = std::move(value);
            cell.m_generation++;

            return Handle<Value>(index, cell.m_generation);
        }
    }

    void Remove(Handle<Value> handle) {
        VERIFY(handle.m_index < m_cells.size());

        auto& cell = m_cells[handle.m_index];

        VERIFY(handle.m_generation == cell.m_generation);
        VERIFY(cell.m_value.has_value());

        cell.m_value = {};

        m_free_cells.push_back(handle.m_index);
    }

private:
    std::vector<Impl::PoolCell<Value>> m_cells;
    std::vector<uint32_t> m_free_cells;
};
}
