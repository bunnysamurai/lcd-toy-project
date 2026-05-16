#include "chippie_inventory.hpp"

namespace chippie
{

namespace
{
inventory static_inventory;
}

[[nodiscard]] inventory &access_chippie_inventory() noexcept
{
    return static_inventory;
}

} // namespace chippie