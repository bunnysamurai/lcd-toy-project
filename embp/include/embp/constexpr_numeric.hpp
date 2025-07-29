#pragma once
#include "utility.hpp"

namespace embp
{
    template < class InputIterator, class InitType >
    constexpr inline InitType 
    accumulate(InputIterator first, InputIterator last, InitType init)
    {
        // could make concept checks here, but we won't until C++20 is widely available
        for(; first != last; ++first)
        {
            init = embp::move(init) + *first;
        }
        return init;
    }

    template < class InputIterator, class InitType, class BinaryOperator >
    constexpr inline InitType 
    accumulate(InputIterator first, InputIterator last, InitType init, BinaryOperator binary_op)
    {
        // could make concept checks here, but we won't until C++20 is widely available
        for(; first != last; ++first)
        {
            init = binary_op(embp::move(init), *first);
        }
        return init;
    }


} // embp
