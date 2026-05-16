/**
    The enitity heap is to help get around some Issues I have with polymorphism, namely, iterating over collections of
    base types but those collections can't own the objects themselves.

    Which means I need somewhere else to keep ownership.  The other niggle is levels will likely be loaded off of disk,
    so I need to allocate space for the dizzying array of different entities any level could present at runtime.  Which
    means I need a heap.

    This module will be a specialized heap to control the number of entities, but can be used generally to allocate
    space at runtime of C++ objects.

 */
#if !defined(ENTITY_HEAP_HPP)
#define ENTITY_HEAP_HPP

#include <cstdint>

namespace chippie
{

/**
    For example:

    arena_allocator<250> heap;
    std::array<entity*, 10> entity_list;

    entity_list[0] = heap.allocate<purple_ball>(ball1_arg1, ball1_arg_2); // returns null on failure
    entity_list[1] = heap.allocate<purple_ball>(ball2_arg1, ball2_arg_2);
    entity_list[2] = heap.allocate<frog_monster>(fmon_arg);

    heap.deallocate(); // clears all the objects, calls their destructors. Also does this when arena_allocator
   destructs.
 */
template <uint32_t Bytes, uint32_t Alignment = 32> class arena_allocator
{

  public:
    template <typename T, typename... Args> [[nodiscard]] T *allocate(Args... args) noexcept
    {
        head_ptr = align_properly<T>(head_ptr);
        if (head_ptr + sizeof(T) < Bytes)
        {
            T *result = new (&buf[head_ptr]) T{std::forward<Args>(args)...};
            head_ptr += sizeof(T);
            return result;
        }

        return nullptr;
    }

  private:
    alignas(Alignment) uint8_t buf[Bytes];
    uint32_t head_ptr{0};

    template <typename T> [[nodiscard]] static constexpr uint32_t align_properly(uint32_t address) noexcept
    {
        /* advance the address N bytes, where N is the minimum amount to achived T's alignment requirements */
        const auto adju = alignof(T) - (address & (alignof(T) - 1));
        if (adju == alignof(T))
        {
            return address;
        }
        return address + adju;
    }

    static_assert(align_properly<uint8_t>(0) == 0);
    static_assert(align_properly<uint8_t>(1) == 1);
    static_assert(align_properly<uint8_t>(10) == 10);
    static_assert(align_properly<uint16_t>(0) == 0);
    static_assert(align_properly<uint16_t>(1) == 2);
    static_assert(align_properly<uint16_t>(10) == 10);
    static_assert(align_properly<uint32_t>(0) == 0);
    static_assert(align_properly<uint32_t>(1) == 4);
    static_assert(align_properly<uint32_t>(10) == 12);
    static_assert(align_properly<uint64_t>(0) == 0);
    static_assert(align_properly<uint64_t>(1) == 8);
    static_assert(align_properly<uint64_t>(10) == 16);
};

} // namespace chippie

#endif