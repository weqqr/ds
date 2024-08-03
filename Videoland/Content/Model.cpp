#include <Videoland/Content/Model.h>

namespace Videoland {
std::expected<Model, int> Model::Load(const std::string& path) {
    return std::unexpected(0);
}
}
