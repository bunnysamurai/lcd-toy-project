#pragma once
#ifdef STD_LIB_AVAILABLE
#include <iterator>
#endif
#include "utility.hpp"

// TODO: should probably make non-const iterators convertable to const

namespace embp
{

namespace details
{

template < class Pointer, class Container >
struct circular_iterator_impl
{
    template < class pointer, class container, class const_pointer >
    friend bool operator==(const circular_iterator_impl<const_pointer, container> lhs, const circular_iterator_impl<pointer, container> rhs) noexcept;

    template < class pointer, class container, class const_pointer >
    friend typename circular_iterator_impl<pointer, container>::difference_type operator-(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept;

public:
    using value_type = embp::remove_pointer_t<Pointer>;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using pointer = Pointer;
    using reference = value_type&;
#ifdef STD_LIB_AVAILABLE
    using iterator_category = std::random_access_iterator_tag; // TODO: need to check for stdlib availability here
#endif
private:
    pointer front_;
    difference_type offset_;
    difference_type size_;

    [[nodiscard]] difference_type aliasedOffset() const noexcept
    {
        return ( size_ + ( offset_ % size_ ) ) % size_;
    }
    [[nodiscard]] difference_type aliasedOffsetWithEnd() const noexcept
    {
        return ( (size_+1) + ( offset_ % (size_+1) ) ) % (size_+1); // for portability reasons, I'll need to test whether or not this operation holds true... I think C++17 standard guarantees it?
    }

public:

    circular_iterator_impl() = default;
    explicit circular_iterator_impl(pointer front, difference_type offset, difference_type size) noexcept :
        front_{front},
        offset_{offset},
        size_{size}
    { }
    template < class T >
    explicit circular_iterator_impl(const circular_iterator_impl<T, Container> &other) noexcept :
        front_{other.front()},
        offset_{other.offset()},
        size_{other.size()}
    { }

    template < class T >
    circular_iterator_impl& operator=(const circular_iterator_impl<T, Container> &other)
    {
        return circular_iterator_impl{other};
    }

    // some accessors
    [[nodiscard]] pointer front() const noexcept { return front_; }
    [[nodiscard]] difference_type offset() const noexcept { return offset_; }
    [[nodiscard]] difference_type size() const noexcept { return size_; }

    // LegacyIterator is dereferancable and prefix-incrementable
    [[nodiscard]] reference operator*() const noexcept { return *(front_ + aliasedOffset()); }
    circular_iterator_impl& operator++() noexcept
    {
        ++offset_;
        return *this;
    }

    pointer operator->() noexcept
    {
        return &operator*();
    }
    pointer operator->() const noexcept
    {
        return &operator*();
    }
    circular_iterator_impl operator++(int) noexcept
    {
        auto tmp{*this};
        (void)operator++();
        return tmp;
    }

    // LegacyForwardIterator is default constructable (why?)
    // LegacyBiDirectionalIterator has post/prefix -- operators
    circular_iterator_impl& operator--() noexcept
    {
        --offset_;
        return *this;
    }
    circular_iterator_impl operator--(int) noexcept
    {
        auto tmp{*this};
        (void)operator--();
        return *this;
    }


    // This is a random access iterator property
    circular_iterator_impl& operator+=(const difference_type n) noexcept
    {
        offset_ += n;
        return *this;
    }
    circular_iterator_impl& operator-=(const difference_type n) noexcept
    {
        offset_ -= n;
        return *this;
    }

    [[nodiscard]] reference operator[](const difference_type n) noexcept
    {
        return *(*this + n);
    }

    [[nodiscard]] circular_iterator_impl operator+(const difference_type n) noexcept
    {
        auto tmp{*this};
        return tmp += n;
    }

    [[nodiscard]] circular_iterator_impl operator-(const difference_type n) noexcept
    {
        auto tmp{*this};
        return tmp -= n;
    }

};

// LegacyInputIterator is equality comparable, allows for member-access-through-pointer, and postfix increment
template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator==(const circular_iterator_impl<const_pointer, container> lhs, const circular_iterator_impl<pointer, container> rhs) noexcept
{
    return lhs.aliasedOffsetWithEnd() == rhs.aliasedOffsetWithEnd() && lhs.front_ == rhs.front_ ;
}
template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator!=(const circular_iterator_impl<const_pointer, container> lhs, const circular_iterator_impl<pointer, container> rhs) noexcept
{
    return !(lhs == rhs);
}

template < class pointer, class container >
[[nodiscard]] circular_iterator_impl<pointer, container> operator+(const typename circular_iterator_impl<pointer, container>::difference_type lhs, const circular_iterator_impl<pointer, container> rhs) noexcept
{
    auto tmp{lhs};
    return tmp += rhs;
}

template < class pointer, class container, class const_pointer >
[[nodiscard]] typename circular_iterator_impl<pointer, container>::difference_type operator-(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept
{
    return lhs.offset_ - rhs.offset_;
}

template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator<(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept
{
    return rhs - lhs > 0;
}

template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator>(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept
{
    return rhs < lhs;
}

template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator>=(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept
{
    return !(lhs < rhs);
}

template < class pointer, class container, class const_pointer >
[[nodiscard]] bool operator<=(const circular_iterator_impl<pointer, container> lhs, const circular_iterator_impl<const_pointer, container> rhs) noexcept
{
    return !(lhs > rhs);
}

} // details

} // embp
