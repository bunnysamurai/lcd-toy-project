#if !defined(CHIPPIE_INVENTORY_HPP)
#define CHIPPIE_INVENTORY_HPP

#include <array>
#include <cstdint>

namespace chippie
{

template <typename ItemEnum, uint32_t ItemCount, typename StorageElemType> class basic_inventory
{
  public:
    void constexpr add(ItemEnum item) noexcept
    {
        const uint32_t index{static_cast<uint32_t>(item)};
        if (index < std::size(item_stash))
        {
            ++item_stash[index];
        }
    }

    void constexpr remove(ItemEnum item) noexcept
    {
        const uint32_t index{static_cast<uint32_t>(item)};
        if (index < std::size(item_stash) && item_stash > 0)
        {
            --item_stash[index];
        }
    }

    [[nodiscard]] constexpr uint8_t check(ItemEnum item) noexcept
    {
        const uint32_t index{static_cast<uint32_t>(item)};
        if (index < std::size(item_stash))
        {
            return item_stash[index];
        }
    }

  private:
    std::array<StorageElemType, ItemCount> item_stash{};
};

enum struct inventory_item
{
    YELLOW_KEY,
    CYAN_KEY,
    RED_KEY,
    GREEN_KEY,
    FIRE_BOOTS,
    SUCTION_BOOTS,
    ICE_SKATES,
    FLIPPERS,
    ITEM_COUNT
};

using inventory = basic_inventory<inventory_item, static_cast<int>(inventory_item::ITEM_COUNT), uint8_t>;

[[nodiscard]] inventory &access_chippie_inventory();

} // namespace chippie

#endif