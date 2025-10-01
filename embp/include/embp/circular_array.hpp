#pragma once
#ifdef STD_LIB_AVAILABLE
#include <cstddef>
#else
#include <stddef.h>
#endif
#include "utility.hpp"
#include "circular_iterator.hpp"
#include "variable_array.hpp"
#include "reverse_iterator.hpp"

namespace embp
{

template < class DataType, size_t Capacity >
struct circular_array
{
public:
    using value_type = DataType;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = details::circular_iterator_impl<pointer, circular_array>;
    using const_iterator = details::circular_iterator_impl<const_pointer, circular_array>;
    using reverse_iterator = embp::reverse_iterator<iterator>;
    using const_reverse_iterator = embp::reverse_iterator<const_iterator>;

private:
    variable_array<DataType, Capacity> data_;
    // size_type head_offset;
    difference_type head_offset;

    template < class Offset >
    constexpr void rotate_impl(Offset middle)
    {
#ifdef STD_LIB_AVAILABLE
        static_assert(std::is_same_v<typename decltype(this->data_)::difference_type, difference_type>);
#endif
        difference_type n = embp::distance(this->data_.cbegin(), this->data_.cend());
        difference_type k = middle;

        if(!k || n == k)
            return;

        if(k == n - k)
        {
            auto &&swap_data_subarray_impl = [&](auto pos, const auto dis) noexcept
            {
                auto it = dis > 0 ? this->data_.end() - 1 : this->data_.begin() + embp::distance(this->data_.cbegin(), pos);
                auto end_itr = dis > 0 ? pos - 1 : this->data_.cend();
                for(;;)
                {
                    embp::swap(*(it + dis), *it);
                    it += 1 - 2*(dis>0);
                    if(it == end_itr)
                        break;
                }
            };

            // swap ranges
            swap_data_subarray_impl(this->data_.cbegin() + k, -k);
        }

        auto p_iter = this->data_.begin();
        for(;;)
        {
            if(k < n - k) // swap elements from front to middle
            {
                auto q_iter = p_iter + k;
                for(difference_type ii = 0; ii < n - k; ++ii)
                {
                    embp::swap(*p_iter++, *q_iter++);
                }
                n %= k;
                if(n == 0)
                {
                    break;
                }
                embp::swap(n, k);
                k = n - k;
            }
            else // swap elements from middle to back
            {
                k = n - k;
                auto q_iter = p_iter + n;
                p_iter = q_iter - k;
                for(difference_type ii = 0; ii < n - k; ++ii)
                {
                    embp::swap(*--p_iter, *--q_iter);
                }
                n %= k;
                if(n == 0)
                {
                    break;
                }
                embp::swap(n, k);
            }
        }
    }

    constexpr inline void shift_subarray_impl(const_iterator pos, const_iterator end_pos, const difference_type dis) noexcept
    {
        // We assume space has been created in the container when shifting to the right
        // dis is an alias for how much space was made.  We just need to shift the appropriate subarray.
        if(!dis)
            return;

        auto itr = dis > 0 ? this->begin() + embp::distance(this->cbegin(), end_pos - 1): this->begin() + embp::distance(this->cbegin(), pos);
        auto end_itr = dis > 0 ? pos - 1 : end_pos;
#if 1 // I wonder if there's a measurable difference between these two implementations?
        while(itr != end_itr)
        {
            *(itr + dis) = *itr;
            itr += 1 - 2*(dis>0);
        }
#else
        for(;;)
        {
            if(itr == end_itr)
                break;
            *(itr + dis) = *itr;
            itr += 1 - 2*(dis>0);
            
        }
#endif
    }

    constexpr void shift_head_to_front_impl() noexcept
    {
        /* shifting the head of the circular array
         * like shift_subarray_impl, but it operates on the data_ iterators, and not the circular iterators...
         * This will be a bit tricky.
         */
        // There may still be some swaps to do.  This will depend upon the distance between the front and the head, and the number of elements
        // essentially what I need to do is a rotate!
        rotate_impl(head_offset);
        head_offset = 0;
    }

    [[nodiscard]] inline constexpr size_type resolveIndexRequest(const size_type pos) const noexcept
    {
        return (head_offset + pos) % (Capacity + 1); // implicit that head_offset is always zero, unless size == Capacity
    }

    constexpr iterator createIterator(const difference_type offset) noexcept
    {
        return iterator{data_.data(), offset, static_cast<difference_type>(data_.size())};
    }
    constexpr const_iterator createIterator(const difference_type offset) const noexcept
    {
        return const_iterator{data_.data(), offset, static_cast<difference_type>(data_.size())};
    }

    constexpr void adjust_size(const difference_type n) noexcept
    {
        data_.resize(data_.size() + n);
    }

    template < class IteratorType >
    [[nodiscard]] constexpr IteratorType adjust_size_and_revalidate_iterator(IteratorType itr, const difference_type n) noexcept
    {
        const auto dis = embp::distance(this->cbegin(), itr);
        adjust_size(n); // adjusting size invalidates all iterators
        return this->cbegin() + dis;
    }

    constexpr void adjust_head_offset(const difference_type n) noexcept
    {
        // head_offset = ( head_offset + n ) % Capacity;
        // head_offset is aliased on Capacity, as it must always point to a valid element in the array
        constexpr difference_type size_ {Capacity};
        head_offset = ( size_ + ( (head_offset + n) % size_ ) ) % size_;
    }

    // value count
    constexpr iterator insert_when_not_full(const_iterator pos, size_type count, const value_type& value) noexcept
    {
        shift_subarray_impl(pos, this->cend() - count, count);
        const auto dis = embp::distance(this->cbegin(), pos);
        for(auto it = this->begin() + dis; it != this->begin() + dis + count; ++it)
        {
            *it = value;
        }
        return this->begin() + dis;
    }
    constexpr iterator insert_when_full(const_iterator pos, size_type count, const value_type& value) noexcept
    {
        if(pos == this->cbegin())
        {
            return this->end();
        }
        else
        {
            const auto end_dis = embp::distance(pos, this->cend());
            adjust_head_offset(count); // all iterator invalidated
            const auto dis = embp::distance(this->cbegin(), pos);
            shift_subarray_impl(pos, pos + end_dis, count);
            for(auto it = this->begin() + dis; it != this->begin() + dis + count; ++it)
            {
                *it = value;
            }
            return this->begin() + dis;
        }
        
    }
    // iterator range
    template < class InputIterator >
    constexpr iterator insert_when_not_full(const_iterator pos, InputIterator first, InputIterator last) noexcept
    {
        const auto count = embp::distance(first, last);
        shift_subarray_impl(pos, this->cend() - count, count);
        const auto dis = embp::distance(this->cbegin(), pos);
        for(auto it = this->begin() + dis; it != this->begin() + dis + count; ++it, ++first)
        {
            *it = *first;
        }
        return this->begin() + dis;
    }
    template < class InputIterator >
    constexpr iterator insert_when_full(const_iterator pos, InputIterator first, InputIterator last) noexcept
    {
        if(pos == this->cbegin())
        {
            return this->end();
        }
        const auto end_dis = embp::distance(pos, this->cend());
        const auto count = embp::distance(first, last);
        adjust_head_offset(count); // pos is now invalid
        const auto dis = embp::distance(this->cbegin(), pos);
        shift_subarray_impl(pos, pos + end_dis, count);
        for(auto it = this->begin() + dis; it != this->begin() + dis + count; ++it, ++first)
        {
            *it = *first;
        }
        return this->begin() + dis;
    }
    
    

public:
    // Constructors
    constexpr circular_array() = default;
    constexpr explicit circular_array(size_type n) noexcept :
        data_{n},
        head_offset{0}
    { }
    constexpr explicit circular_array(size_type n, const value_type &value) noexcept :
        data_{n, value},
        head_offset{0}
    { }
    template < class Iter >
    constexpr circular_array(Iter first, Iter last) noexcept :
        data_{first, last},
        head_offset{0}
    { }
    constexpr circular_array(const circular_array &cpy) noexcept :
        data_{cpy.data_},
        head_offset{cpy.head_offset}
    { }
    constexpr circular_array(circular_array &&mv) noexcept :
        data_{embp::move(mv.data_)},
        head_offset{embp::move(mv.head_offset)}
    { }


    // Assignment
    constexpr circular_array& operator=(const circular_array &cpy) noexcept
    {
        data_ = cpy.data_;
        head_offset = cpy.head_offset;
        return *this;
    }
    constexpr circular_array& operator=(circular_array &&mv) noexcept
    {
        data_ = embp::move(mv.data_);
        head_offset = embp::move(mv.head_offset);
        return *this;
    }
    constexpr void assign(size_type n, const value_type &value) noexcept
    {
        data_.assign(n, value);
        head_offset = 0;
    }
    template < class Iter >
    constexpr void assign(Iter first, Iter last) noexcept
    {
        data_.assign(first, last);
        head_offset = 0;
    }

    // Element access
    [[nodiscard]] constexpr reference operator[](size_type pos) noexcept { return data_[resolveIndexRequest(pos)]; }
    [[nodiscard]] constexpr const_reference operator[](size_type pos) const noexcept { return data_[resolveIndexRequest(pos)]; }
    [[nodiscard]] constexpr reference front() noexcept { return data_[resolveIndexRequest(0)]; }
    [[nodiscard]] constexpr const_reference front() const noexcept { return data_[resolveIndexRequest(0)]; }
    [[nodiscard]] constexpr reference back() noexcept { return data_[resolveIndexRequest(data_.size() - 1)]; }
    [[nodiscard]] constexpr const_reference back() const noexcept { return data_[resolveIndexRequest(data_.size() - 1)]; }

    [[nodiscard]] constexpr const_pointer data() const noexcept { return data_.data(); }
    [[nodiscard]] constexpr const difference_type& head() const noexcept { return head_offset; }

    // Iterators
    [[nodiscard]] constexpr iterator begin() noexcept { return createIterator(head_offset); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return createIterator(head_offset); }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return createIterator(head_offset); }
    [[nodiscard]] constexpr iterator end() noexcept { return createIterator(head_offset + data_.size()); }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return createIterator(head_offset + data_.size()); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return createIterator(head_offset + data_.size()); }
    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(this->end()); }
    // [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(this->end()); }
    // [[nodiscard]] const_reverse_iterator rcbegin() const noexcept { return const_reverse_iterator(this->cend()); }
    // [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(this->begin()); }
    // [[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator(this->begin()); }
    // [[nodiscard]] const_reverse_iterator rcend() const noexcept { return const_reverse_iterator(this->cbegin()); }

    // Capacity
    [[nodiscard]] constexpr bool empty() const noexcept { return this->begin() == this->end(); }
    [[nodiscard]] constexpr size_type size() const noexcept { return data_.size(); }
    [[nodiscard]] constexpr size_type max_size() const noexcept { return Capacity; }
    //reserve
    [[nodiscard]] constexpr size_type capacity() const noexcept { return Capacity; }
    //shrink_to_fit
    [[nodiscard]] constexpr bool full() const noexcept { return this->size() == Capacity; }


    // Modifiers
    constexpr void clear() noexcept
    {
        data_.clear();
        head_offset = 0;
    }
    constexpr iterator insert(const_iterator pos, const value_type& value) noexcept
    {
        return insert(pos, 1, value);
    }
    constexpr iterator insert(const_iterator pos, value_type&& value) noexcept
    {
        return insert(pos, 1, value);
    }
    constexpr iterator insert(const_iterator pos, size_type count, const value_type& value) noexcept
    {
        /* insert for more than one element is not so straightforward
         *  If we are not full before the insert, and the number we insert will not overfill, we can safely add "count" extra value
         *  If we are full before insert, then we do not adjust the size, and then adjust the head offset by count
         *  If we are not full before insert, but will be overfull after insert, then we need to do both of the above
         */
        /* TODO: On second thought, there really shouldn't be an insert.  I can instead replace it with an insert_front or insert_back.
         * insert_front -> insert items at the front, pushing items off the end of the array
         * insert_back -> insert items at the back, pushing items off the front of the array.
         */
        const auto count_after_fill = ((this->size() + count) - data_.capacity()) * (this->size() + count > data_.capacity());
        const auto count_before_fill = count - count_after_fill;
        const auto was_full = this->full();
        pos = adjust_size_and_revalidate_iterator(pos, count_before_fill * !was_full); // adjusting size invalidates all iterators
        const auto might_now_be_full = this->data_.size() == Capacity;

        auto itr = [&]()
        {
            auto ii = insert_when_not_full(pos, count_before_fill * !was_full, value);
            return this->cbegin() + embp::distance(this->begin(), ii);
        }();
        if(this->end() != insert_when_full(itr + count_before_fill * !was_full, count_after_fill * might_now_be_full, value))
        {
            return this->begin() + embp::distance(this->begin(), itr);
        }
        else
        {
            return this->end();
        }

    }
    template < class InputIterator >
    constexpr iterator insert(const_iterator pos, InputIterator first, InputIterator last) noexcept
    {
        const auto count = embp::distance(first, last);
        const auto count_after_fill = ((this->size() + count) - data_.capacity()) * (this->size() + count > data_.capacity());
        const auto count_before_fill = count - count_after_fill;
        const auto was_full = this->full();
        pos = adjust_size_and_revalidate_iterator(pos, count_before_fill * !was_full); // adjusting size invalidates all iterators

        auto itr_before_fill_first = first;
        auto itr_before_fill_last = first + count_before_fill * !was_full;

        auto itr_after_fill_first = itr_before_fill_last;
        auto itr_after_fill_last = last;

        auto itr = [&]()
        {
            auto ii = insert_when_not_full(pos, itr_before_fill_first, itr_before_fill_last);
            return this->cbegin() + embp::distance(this->begin(), ii);
        }();
        insert_when_full(itr + count_before_fill * !was_full, itr_after_fill_first, itr_after_fill_last);
        return this->begin() + embp::distance(this->begin(), itr);
    }
    //insert(const_iterator pos, std::initializer_list)
//    template < class ... Args >
//    constexpr iterator emplace(Args&&... args) noexcept;
    constexpr iterator erase(const_iterator pos) noexcept
    {
        return this->erase(pos, pos+1);
    }
    constexpr iterator erase(const_iterator first, const_iterator last) noexcept
    {
        // This probably needs to be (more) expensive
        // erase will eliminate the elements pointed to by this function's arguments
        // This will leave data_ in an unorganzied state such that elements towards the front are (possibly) invalid
        // To ease pain, I should then shift the data in the array to the left so that the head_offset is restored to 0.
        // I can then call data_.resize() safely.
        const auto dis = embp::distance(first, last);
        const auto count = dis * (dis > 0);
        shift_subarray_impl(last, this->cend(), -count);
        shift_head_to_front_impl();
        data_.resize(data_.size() - count);
        return this->begin() + embp::distance(this->cbegin(), first);
    }
    constexpr void push_back(const value_type &value) noexcept
    {
        this->insert(this->cend(), value);
    }
    constexpr void push_back(value_type &&value) noexcept
    {
        this->insert(this->cend(), embp::move(value));
    }
    constexpr void push_front(const value_type &value) noexcept
    {
        // TODO: we won't use insert when full, as I need to redo the semantics for it anyways to only insert_front or insert_back.
        if(!this->full())
        {
            this->insert(this->cbegin(), value);
        }
        else
        {
            *(this->end() - 1) = value;
            adjust_head_offset(-1);
        }

    }
//    template < class ... Args >
//    constexpr reference emplace_back(Args&&... args) noexcept;
    constexpr void pop_back() noexcept { this->erase(this->cend() - 1); }
    constexpr void pop_front() noexcept { this->erase(this->cbegin()); }
    constexpr void resize(const size_type count) noexcept
    {
        this->resize(count, value_type{});
    }
    constexpr void resize(const size_type count, const value_type &value) noexcept
    {
        const difference_type numberToAdd = count - data_.size();
        if(numberToAdd > 0)
        {
           this->insert(this->cend(), numberToAdd, value);
        }
        else
        {
            this->erase(this->cend() + numberToAdd, this->cend());
        }
    }
    constexpr void swap(circular_array &other) noexcept
    {
        auto& smallerArray = this->size() > other.size() ? other : *this;
        auto& largerArray = this->size() > other.size() ? *this : other;

        const auto smallSizeCount = smallerArray.size();
        for(size_type ii = 0; ii != smallSizeCount; ++ii)
        {
            embp::swap(smallerArray[ii], largerArray[ii]);
        }
        smallerArray.resize(largerArray.size());
        for(size_type ii = smallSizeCount; ii != smallerArray.size(); ++ii)
        {
            smallerArray[ii] = largerArray[ii];
        }
        largerArray.resize(smallSizeCount);
    }

};

// Non-member functions
template < class DataType, size_t Count >
bool operator==(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return
            lhs.size() == rhs.size()
            &&
            embp::compareElementWise(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [](const auto &l, const auto &r){ return l == r; });
}
template < class DataType, size_t Count >
bool operator!=(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return !(lhs == rhs);
}
template < class DataType, size_t Count >
bool operator<(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return embp::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), [](const auto &l, const auto &r){ return l < r; });

}
template < class DataType, size_t Count >
bool operator<=(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return embp::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.cend(), [](const auto &l, const auto &r){ return l <= r; });
}
template < class DataType, size_t Count >
bool operator>(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return !(lhs <= rhs);
}
template < class DataType, size_t Count >
bool operator>=(const circular_array<DataType, Count> &lhs, const circular_array<DataType, Count> &rhs) noexcept
{
    return !(lhs < rhs);
}

template < class DataType, size_t Count >
void swap(circular_array<DataType, Count> &lhs, circular_array<DataType, Count> &rhs) noexcept
{
    lhs.swap(rhs);
}


} // embp
