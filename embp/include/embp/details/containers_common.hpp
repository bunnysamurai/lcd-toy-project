#pragma once
#include "../utility.hpp"

namespace bp
{
namespace containers
{
namespace details
{

template < class IterOrValue, class ResolverFunc >
    constexpr void size_copy_impl(IterOrValue &&first, const size_type start, const size_type count, ResolverFunc resolve) noexcept
    {
        //TODO: no side effects in constexpr contexts
        for(size_type ii = start; ii < start + count; ++ii)
            data_[ii] = resolve(bp::utilities::forward<IterOrValue>(first));
    }

    constexpr bool is_available(const size_type request) noexcept
    {
        return request < Capacity + 1;
    }

    constexpr void swap_impl(value_type &lhs, value_type &rhs) noexcept
    {
        const value_type a = lhs;
        lhs = rhs;
        rhs = a;
    }

    constexpr void shift_subarray_impl(const_iterator pos, const difference_type dis) noexcept
    {
        auto it = dis > 0 ? this->end() - 1 : this->begin() + bp::utilities::distance(this->cbegin(), pos);
        auto end_itr = dis > 0 ? pos : this->cend();
        while(it != end_itr)
        {
            swap_impl(*it, *(it+dis));
            it += 1 - 2*(dis>0);
        }
        swap_impl(*it, *(it+dis));
    }

}
}
}
