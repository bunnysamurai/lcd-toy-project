#include <numeric>
#include <gtest/gtest.h>
#include <utility>

#include "circular_array.hpp"

#ifndef NDEBUG
namespace {
    template < class Iter >
    void printArray(const char* msg, Iter first, Iter last)
    {
        std::cerr << msg << " { ";
        for(; first != last; ++first)
        {
            std::cerr << *first << " ";
        }
        std::cerr << "}\n";
    }
    template < class T >
    void printValue(const char* msg, const T &val)
    {
        std::cerr << msg << ": " << val << '\n';
    }
}
#endif

template < class TypeUnderTest >
struct get_base_type
{
    using base_type = std::remove_const_t<std::remove_reference_t<TypeUnderTest>>;
};
template < class TypeUnderTest >
struct get_base_type<TypeUnderTest&>
{
    using base_type = std::remove_const_t<std::remove_reference_t<TypeUnderTest>>;
};
template < class TypeUnderTest >
struct get_base_type<TypeUnderTest&&>
{
    using base_type = std::remove_const_t<std::remove_reference_t<TypeUnderTest>>;
};

TEST(CircularArray, Construction_PositiveCase)
{
    using DataType = float;
    constexpr size_t N = 10;
    constexpr DataType testVal = -24.42f;

    auto checkRefSpec = [](const auto &container)
    {
        using base_type = typename get_base_type<decltype(container)>::base_type;
        using value_type = typename get_base_type<decltype(container)>::base_type::value_type;
        static_assert(
                    std::is_reference_v<typename base_type::reference>
                    &&
                    !std::is_const_v<std::remove_reference_t<typename base_type::reference>>
                    &&
                    std::is_same_v<std::remove_reference_t<typename base_type::reference>, value_type>
                    &&
                    std::is_reference_v<typename base_type::const_reference>
                    &&
                    std::is_const_v<std::remove_reference_t<typename base_type::const_reference>>
                    &&
                    std::is_same_v<std::remove_reference_t<typename base_type::const_reference>, std::add_const_t<value_type>>
        );
        return true;
    };

    auto checkPointerSpec = [](const auto &container)
    {
        using base_type = typename get_base_type<decltype(container)>::base_type;
        using value_type = typename get_base_type<decltype(container)>::base_type::value_type;
        static_assert(
                    std::is_pointer_v<typename base_type::pointer>
                    &&
                    !std::is_const_v<std::remove_pointer_t<typename base_type::pointer>>
                    &&
                    std::is_same_v<std::remove_pointer_t<typename base_type::pointer>, value_type>
                    &&
                    std::is_pointer_v<typename base_type::const_pointer>
                    &&
                    std::is_const_v<std::remove_pointer_t<typename base_type::const_pointer>>
                    &&
                    std::is_same_v<std::remove_pointer_t<typename base_type::const_pointer>, std::add_const_t<value_type>>
        );
        return true;
    };

    auto checkIteratorSpec = [](const auto &container)
    {
        using base_type = typename get_base_type<decltype(container)>::base_type;

        using const_iterator = typename base_type::const_iterator;
        using iterator = typename base_type::iterator;

        static_assert(std::is_pointer_v<typename std::iterator_traits<const_iterator>::pointer>);
        static_assert(std::is_pointer_v<typename std::iterator_traits<iterator>::pointer>);

        static_assert( std::is_const_v<std::remove_pointer_t<typename std::iterator_traits<const_iterator>::pointer>>);
        static_assert(!std::is_const_v<std::remove_pointer_t<typename std::iterator_traits<iterator>::pointer>>);

        static_assert(std::is_reference_v<typename std::iterator_traits<const_iterator>::reference>);
        static_assert(std::is_reference_v<typename std::iterator_traits<iterator>::reference>);

        static_assert(std::is_const_v<std::remove_reference_t<typename std::iterator_traits<const_iterator>::reference>>);
        static_assert(!std::is_const_v<std::remove_reference_t<typename std::iterator_traits<iterator>::reference>>);

        return true;
    };

    auto checkBaseTypes = [](const auto &container)
    {
        using base_type = typename get_base_type<decltype(container)>::base_type;
        static_assert(
                    std::is_same_v<typename base_type::value_type, DataType>
                    &&
                    std::is_same_v<typename base_type::size_type, size_t>
                    &&
                    std::is_same_v<typename base_type::difference_type, std::ptrdiff_t>
        );
    };

    auto checkTypeSpec = [&](const auto &container)
    {
        using base_type = typename get_base_type<decltype(container)>::base_type;
        static_assert (std::is_same_v<base_type, embp::circular_array<DataType, N>>);
        checkBaseTypes(container);
        checkRefSpec(container);
        checkPointerSpec(container);
        checkIteratorSpec(container);
        return container.capacity() == N;
    };

    // Default
    {
        embp::circular_array<DataType, N> t;
        EXPECT_TRUE(checkTypeSpec(t));
        EXPECT_EQ(t.size(), 0);
    }

    // Size specifier
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t(ii);
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_EQ(t.size(), ii);
        }
    }

    // Size specifier, fill with value
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t(ii, testVal);
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_EQ(t.size(), ii);
            EXPECT_EQ(t.head(), 0);
            EXPECT_TRUE(std::all_of(t.begin(), t.end(), [&](const auto &val){ return val == testVal; }));
        }
    }

    // Iterator range
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            std::vector<DataType> testVector(ii);
            std::iota(testVector.begin(), testVector.end(), DataType{});
            embp::circular_array<DataType, N> t(testVector.cbegin(), testVector.cend());
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_TRUE(std::equal(t.begin(), t.end(), testVector.cbegin()));
        }
    }

    auto &&checkTwoAreEqual = [&](const auto &lhs, const auto &rhs)
    {
        return
                checkTypeSpec(lhs)
                &&
                checkTypeSpec(rhs)
                &&
                std::is_same_v<decltype (lhs), decltype(rhs)>
                &&
                std::equal(lhs.begin(), lhs.end(), rhs.begin());
    };
    // Copy Construct
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t1(ii, testVal);
            auto t2{t1};

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Move Construct
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t1(ii, testVal);
            auto t2{std::move(t1)};

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Copy assignment
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t1(ii, testVal);
            embp::circular_array<DataType, N> t2;
            t2 = t1;

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Move assignment
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::circular_array<DataType, N> t1(ii, testVal);
            embp::circular_array<DataType, N> t2;
            t2 = std::move(t1);

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Size spec assign
    {
        embp::circular_array<DataType, N> t;
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            t.assign(ii, testVal);
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_EQ(t.size(), ii);
            EXPECT_TRUE(std::all_of(t.begin(), t.end(), [&](const auto &val){ return val == testVal; }));
        }
    }

    // Iterator range assign
    {
        embp::circular_array<DataType, N> t;
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            std::vector<DataType> testVector(ii);
            std::iota(testVector.begin(), testVector.end(), DataType{});
            t.assign(testVector.cbegin(), testVector.cend());

            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_TRUE(std::equal(t.begin(), t.end(), testVector.cbegin()));
        }
    }

}

TEST(CircularArray, ElementAccess_PositiveCase)
{
    using DataType = uint16_t;
    constexpr size_t N = 150;
    using container_type = embp::circular_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;
    const container_type baseDataSet = [&]()
    {
        container_type rv (SizeCount);
        std::iota(rv.begin(), rv.end(), DataType{});
        return rv;
    }();

    std::vector<DataType> testVector(SizeCount);
    std::iota(testVector.begin(), testVector.end(), DataType{});
    // quick check
    ASSERT_TRUE(std::equal(baseDataSet.cbegin(), baseDataSet.cend(), testVector.cbegin()));
    ASSERT_TRUE(std::equal(baseDataSet.cbegin(), baseDataSet.cend(), baseDataSet.cbegin()));

    // Square bracket
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;
        // type spec

        static_assert(std::is_same_v<decltype(nonConst[0]), typename decltype(nonConst)::reference>);
        static_assert(std::is_same_v<decltype(constExample[0]), typename decltype(nonConst)::const_reference>);
        // read check
        bool squareBracketValueCheck = true;
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            squareBracketValueCheck &= (nonConst[ii] == ii) && (constExample[ii] == ii);
        }
        EXPECT_TRUE(squareBracketValueCheck);
        // write check
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            ++nonConst[ii];
        }
        bool squareBracketValueCheck2nd = true;
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            squareBracketValueCheck2nd &= nonConst[ii] == (constExample[ii] + 1);
        }
        EXPECT_TRUE(squareBracketValueCheck2nd);
    }
    // Front
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;
        // type spec
        static_assert(std::is_same_v<decltype(nonConst.front()), typename decltype(nonConst)::reference>);
        static_assert(std::is_same_v<decltype(constExample.front()), typename decltype(nonConst)::const_reference>);
        // value check
        EXPECT_EQ(nonConst.front(), 0);
        EXPECT_EQ(nonConst.front(), constExample.front());
        // write check
        ++nonConst.front();
        EXPECT_EQ(nonConst.front(), constExample.front() + 1);
    }
    // Back
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;
        // type spec
        static_assert(std::is_same_v<decltype(nonConst.back()), typename decltype(nonConst)::reference>);
        static_assert(std::is_same_v<decltype(constExample.back()), typename decltype(nonConst)::const_reference>);
        // value check
        EXPECT_EQ(nonConst.back(), SizeCount - 1);
        EXPECT_EQ(nonConst.back(), constExample.back());
        // write check
        ++nonConst.back();
        EXPECT_EQ(nonConst.back(), constExample.back() + 1);
    }
    // Data
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;
        static_assert(std::is_same_v<decltype(nonConst.data()), typename decltype(nonConst)::const_pointer>);
        static_assert(std::is_same_v<decltype(constExample.data()), typename decltype(nonConst)::const_pointer>);
        // value check
        bool dataAccessValueCheck = true;
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            dataAccessValueCheck &= *(nonConst.data() + ii) == ii && *(constExample.data() + ii) == ii;
        }
        EXPECT_TRUE(dataAccessValueCheck);
    }
}


TEST(CircularArray, Iterator_PositiveCase)
{
    using DataType = double;
    constexpr size_t N = 34;
    using container_type = embp::circular_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;
    auto baseDataSet = [&](auto count)
    {
        container_type rv (count);
        std::iota(rv.begin(), rv.end(), DataType{});
        return rv;
    };

    // Begin
    {
        container_type nonConst = baseDataSet(SizeCount);
        const container_type constExample = baseDataSet(SizeCount);

        auto &&checkBegin = [](auto &container)
        {
            EXPECT_EQ(*(container.data()), *(container.begin()));
            EXPECT_EQ(*(container.data()), *(container.cbegin()));
        };
        checkBegin(nonConst);
        checkBegin(constExample);
    }

    // End
    {
        container_type nonConst = baseDataSet(SizeCount);
        const container_type constExample = baseDataSet(SizeCount);

        auto &&checkEnd = [](auto &container)
        {
            EXPECT_EQ(*(container.data() + container.size() - 1), *(container.end() - 1));
            EXPECT_EQ(*(container.data() + container.size() - 1), *(container.cend() - 1));
        };
        checkEnd(nonConst);
        checkEnd(constExample);
    }

    auto checkBeginAndEnd = [](auto &nonConst)
    {
        EXPECT_EQ(nonConst.begin(), nonConst.cbegin());
        EXPECT_EQ(nonConst.end(), nonConst.cend());
        
        EXPECT_NE(nonConst.begin(), nonConst.end());
        EXPECT_NE(nonConst.cbegin(), nonConst.cend());
        
        EXPECT_NE(nonConst.begin(), nonConst.cend());
        EXPECT_NE(nonConst.cbegin(), nonConst.end());
    };

    // what happens when container is full and overfull?
    {
        container_type notQuiteFull = baseDataSet(N-1);
        container_type full = baseDataSet(N-1);
        full.push_back(1);
        container_type overFull = baseDataSet(N);
        overFull.push_back(1);

        EXPECT_FALSE(notQuiteFull.full());
        EXPECT_TRUE(full.full());
        EXPECT_TRUE(overFull.full());

        checkBeginAndEnd(notQuiteFull);
        checkBeginAndEnd(full);
        checkBeginAndEnd(overFull);
    }

    // let's check the circular nature of the iterator
    {
        auto d = baseDataSet(SizeCount);
        EXPECT_EQ(d.begin() - 1, d.end());
        EXPECT_EQ(d.cbegin() - 1, d.cend());
    }


    // what about distances between iterators?
    {

    }
}

TEST(CircularArray, Capacity_PositiveCase)
{
    using DataType = double;
    constexpr size_t N = 34;
    using container_type = embp::circular_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;

    // read
    {
        for(size_t ii = 1; ii < SizeCount; ++ii)
        {
            const container_type baseDataSet = [&]()
            {
                container_type rv (ii);
                std::iota(rv.begin(), rv.end(), DataType{});
                return rv;
            }();
            EXPECT_TRUE(!baseDataSet.empty());
            EXPECT_EQ(baseDataSet.size(), ii);
            EXPECT_EQ(baseDataSet.max_size(), N);
            EXPECT_EQ(baseDataSet.capacity(), N);
        }
    }
}

TEST(CircularArray, Modifiers_PositiveCase)
{
    using DataType = uint32_t;
    constexpr size_t N = 34;
    using container_type = embp::circular_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;

    const auto createDataSet = [](size_t count)
    {
        container_type baseDataSet (count);
        for(size_t ii = 0; ii < baseDataSet.size(); ++ii)
        {
            baseDataSet[ii] = ii;
        }
        return baseDataSet;
    };

    // clear
    {
        container_type testData = createDataSet(SizeCount);
        testData.clear();
        EXPECT_TRUE(testData.empty());
        EXPECT_EQ(testData.size(), 0);
        EXPECT_EQ(testData.max_size(), N);
        EXPECT_EQ(testData.capacity(), N);
    }
    
    // insert
    {
        // lvalue value
        {
            auto insertSingleValue = [&](auto offset, auto containerSize)
            {
                const container_type referenceData = createDataSet(containerSize);
                container_type testData = referenceData;
                const DataType val = 534;
                const auto pos = testData.cbegin() + offset;
                const auto count = 1;
                auto returnIter = testData.insert(pos, val);

                /* Insert behaviour depends on whether or not the operation will fill the buffer
                 * When the buffer is not full, insert behaves identical to vector's insert
                 * When the buffer is full, new elements are inserted into the specified position, 
                 * then the old elements are shifted out.  Note that, if you insert a single value at the beginning, 
                 * this is a no-op.
                 */
                if(referenceData.size() != referenceData.capacity())
                {

                    EXPECT_EQ(returnIter, testData.begin() + offset);
                    EXPECT_EQ(*returnIter, val);
                    EXPECT_EQ(testData.size(), std::min(referenceData.capacity(), referenceData.size() + count));
                    const auto pos_offset = embp::distance(testData.cbegin(), pos);
                    // check either side of the insert for equality
                    const auto leftSide = std::equal(testData.begin(), returnIter, referenceData.cbegin());
                    const auto rightSide = std::equal(returnIter + count, testData.end(), referenceData.cbegin() + std::distance(testData.begin(), returnIter));
                    EXPECT_TRUE(rightSide && leftSide);
                }
                else
                {
                    EXPECT_EQ(returnIter, testData.begin() + offset - 1);
                    if(returnIter != testData.end())
                    {
                        EXPECT_EQ(*returnIter, val);
                    }
                    EXPECT_EQ(testData.size(), referenceData.capacity());
                    
                    // make sure the number of elements to check is okay
                    if(returnIter != testData.end())
                    {
                        ASSERT_TRUE(std::distance(testData.begin(), returnIter) <= std::distance(referenceData.cbegin()+count, referenceData.cend()));
                        // check the data (if any) in front of the inserted element
                        EXPECT_TRUE(std::equal(testData.begin(), returnIter, referenceData.cbegin() + count));
                        // check the data (if any) after the inserted element
                        EXPECT_TRUE(std::equal(returnIter + count, testData.end(), referenceData.cbegin() + count + std::distance(testData.begin(), returnIter)));
                    }
                }

            };
            for(size_t ii = 0; ii < SizeCount + 1; ++ii)
            {
                // insertSingleValue(ii, SizeCount);
            }
            for(size_t ii = 0; ii < /*N +*/ 1; ++ii)
            {
                insertSingleValue(ii, N);
            }

        }
        // rvalue value
        {
            const container_type referenceData = createDataSet(SizeCount);
            container_type testData = referenceData;
            constexpr DataType testVal = 534;
            DataType val = testVal;
            auto returnIter = testData.insert(testData.cbegin(), std::move(val));
            EXPECT_EQ(returnIter, testData.begin());
            EXPECT_EQ(testData.front(), testVal);

            EXPECT_EQ(testData.size(), referenceData.size() + 1);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin(), referenceData.cend(), testData.cbegin() + 1,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );
        }
        // insert value count times
        {
            const container_type referenceData = createDataSet(SizeCount);
            container_type testData = referenceData;

            const auto pos = testData.cbegin()+1;
            const auto count = testData.capacity() - testData.size();
            constexpr DataType val = 534;
            auto returnIter = testData.insert(pos, count, val);
            EXPECT_EQ(returnIter, pos);

            /* I expect testData to be equal to reference data at the front, and then everywhere after
             * testData.capacity() - testData.size() elements
             * size = 5
             * capacity = 9
             * count = 4
             * ref = {0, 1, 2, 3, 4}
             * After operation, I expect:
             * testData = {0, 534, 534, 534, 534, 1, 2, 3, 4}
             */

            EXPECT_EQ(testData.front(), referenceData.front());

            EXPECT_EQ(testData.size(), referenceData.size() + count);
            const auto pos_offset = embp::distance(testData.cbegin(), pos);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin() + pos_offset, referenceData.cend(), testData.cbegin() + pos_offset + count,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );
            EXPECT_TRUE(
                        std::all_of(
                            testData.cbegin() + pos_offset,
                            testData.cbegin() + pos_offset + count,
                            [&](const auto &test_value)
                            {
                                return val == test_value;})
                       );
        }
        // range insert
        {
            const container_type referenceData = createDataSet(SizeCount);
            container_type testData = referenceData;

            const auto pos = testData.cbegin()+1;
            const std::vector<DataType> valVector {534, 241, 62, 123421, 663};
            const auto count = valVector.size();
            auto returnIter = testData.insert(pos, valVector.cbegin(), valVector.cend());
            EXPECT_EQ(returnIter, pos);

            /* I expect testData to be equal to reference data at the front, and then everywhere after
             * testData.capacity() - testData.size() elements
             * size = 5
             * capacity = 9
             * count = 4
             * ref = {0, 1, 2, 3, 4}
             * After operation, I expect:
             * testData = {0, 534, 534, 534, 534, 1, 2, 3, 4}
             */

            EXPECT_EQ(testData.front(), referenceData.front());

            EXPECT_EQ(testData.size(), referenceData.size() + count);

            const auto pos_offset = embp::distance(testData.cbegin(), pos);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin() + pos_offset, referenceData.cend(), testData.cbegin() + pos_offset + count,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );

            
            EXPECT_TRUE(std::transform_reduce(
                            valVector.cbegin(), valVector.cend(), testData.cbegin() + pos_offset,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        ));
        }
        // range insert before the beginning on a full array, which should do nothing
        {
            const container_type referenceData = createDataSet(N);
            container_type testData = referenceData;

            const auto pos = testData.cbegin();
            const std::vector<DataType> valVector {534, 241, 62, 123421, 663};
            const auto count = 0;
            auto returnIter = testData.insert(pos, valVector.cbegin(), valVector.cend());
            EXPECT_EQ(returnIter, pos);

            /* I expect testData to be equal to reference data at the front, and then everywhere after
             * testData.capacity() - testData.size() elements
             * size = 5
             * capacity = 9
             * count = 4
             * ref = {0, 1, 2, 3, 4}
             * After operation, I expect:
             * testData = {0, 534, 534, 534, 534, 1, 2, 3, 4}
             */

            EXPECT_EQ(testData.front(), referenceData.front());

            EXPECT_EQ(testData.size(), referenceData.size());

            const auto pos_offset = embp::distance(testData.cbegin(), pos);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin() + pos_offset, referenceData.cend(), testData.cbegin() + pos_offset + count,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );

            
            EXPECT_FALSE(std::transform_reduce(
                            valVector.cbegin(), valVector.cend(), testData.cbegin() + pos_offset,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        ));
        }
        // range insert after the beginning on a full array, which should do something
        {
            const container_type referenceData = createDataSet(N);
            container_type testData = referenceData;

            const std::vector<DataType> valVector {534, 241, 62, 123421, 663};
            const auto pos = testData.cbegin() + valVector.size();
            // const auto pos_offset_pre = embp::distance(testData.cbegin(), pos);
            const auto count = valVector.size();
            auto returnIter = testData.insert(pos, valVector.cbegin(), valVector.cend());
            EXPECT_EQ(returnIter, pos);

            EXPECT_EQ(testData.size(), referenceData.size());
            // const auto pos_offset = embp::distance(testData.cbegin(), pos);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin() + valVector.size(), referenceData.cend(), returnIter + valVector.size(),
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );
            EXPECT_TRUE(std::transform_reduce(
                            valVector.cbegin(), valVector.cend(), returnIter,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        ));
        }
        // range insert such that array growth and cyclic insert will both occur
        {
            const container_type referenceData = createDataSet(N-1);
            container_type testData = referenceData;

            const std::vector<DataType> valVector {534, 241, 62, 123421, 663};
            const auto numElementsEliminated = testData.size() + valVector.size() - testData.capacity();
            const auto pos = testData.cbegin() + testData.capacity() / 2;
            const auto pos_dis = std::distance(testData.cbegin(), pos);
            auto returnIter = testData.insert(pos, valVector.cbegin(), valVector.cend());
            EXPECT_EQ(returnIter, pos);

            EXPECT_EQ(testData.size(), referenceData.size() + 1);
            const bool leftOfInsertOkay = std::transform_reduce(
                            referenceData.cbegin() + numElementsEliminated, referenceData.cbegin() + pos_dis, testData.cbegin(),
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        );
            const bool rightOfInsertOkay = std::transform_reduce(
                            referenceData.cbegin() + pos_dis, referenceData.cend(), returnIter + valVector.size(),
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        );
            EXPECT_TRUE(leftOfInsertOkay && rightOfInsertOkay);
            EXPECT_TRUE(std::transform_reduce(
                            valVector.cbegin(), valVector.cend(), returnIter,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        ));
        }
    }
    // erase
    {

        const container_type referenceData = createDataSet(SizeCount);
        // erase one specific element
        {
            
            container_type testData = referenceData;
            // const auto offset = referenceData.size()/2;
            const auto offset = 2;
            const auto pos = testData.cbegin() + offset;

            const auto returnIt = testData.erase(pos);
            EXPECT_EQ(returnIt, testData.cbegin() + offset);
            EXPECT_EQ(testData.size(), referenceData.size() - 1);
            EXPECT_TRUE(
                    testData.cend()
                    !=
                    std::search(testData.cbegin(), testData.cend(), referenceData.cbegin(), referenceData.cbegin() + offset)
                    );
        }
        // erase entire array, one element at a time
        {
            container_type testData = referenceData;
            while(!testData.empty())
                testData.erase(testData.cbegin());
            EXPECT_TRUE(testData.empty());
        }
        // erase a range
        {
            container_type testData = referenceData;
            const auto start = testData.cbegin() + referenceData.size() / 3;
            const auto stop = testData.cbegin() + referenceData.size() / 2;
            ASSERT_NE(start, stop);
            const auto numberToErase = embp::distance(start, stop);

            const auto returnIt = testData.erase(start, stop);
            EXPECT_EQ(returnIt, testData.begin() + referenceData.size() / 3);
            EXPECT_EQ(testData.size(), referenceData.size() - numberToErase);
            EXPECT_TRUE(
                    testData.cend()
                    !=
                    std::search(testData.cbegin(), testData.cend(), referenceData.cbegin(), referenceData.cbegin() + referenceData.size() / 3)
                    );
            EXPECT_TRUE(
                    testData.cend()
                    !=
                    std::search(testData.cbegin(), testData.cend(), referenceData.cbegin() + referenceData.size() / 3 + numberToErase, referenceData.cend())
                    );
        }
        // erase an empty range
        {
            container_type testData = referenceData;
            const auto start = testData.cbegin()+referenceData.size()/2;
            const auto stop = start;

            const auto returnIt = testData.erase(start, stop);
            EXPECT_EQ(returnIt, testData.cbegin() + referenceData.size()/2);
            EXPECT_EQ(testData.size(), referenceData.size());
            EXPECT_TRUE(std::equal(referenceData.cbegin(), referenceData.cend(), testData.cbegin()));
        }
    }
    // push_back
    {
        const container_type referenceData = createDataSet(SizeCount);
        // lvalue
        {
            container_type testData = referenceData;
            const DataType val = 534;
            testData.push_back(val);
            EXPECT_EQ(testData.size(), referenceData.size() + 1);
            EXPECT_TRUE(std::equal(testData.cbegin(), testData.cend() - 1, referenceData.cbegin()));
            EXPECT_EQ(testData.back(), val);
        }
        // rvalue
        {
            container_type testData = referenceData;
            constexpr DataType testVal = 534;
            DataType val = testVal;
            testData.push_back(std::move(val));
            EXPECT_EQ(testData.size(), referenceData.size() + 1);
            EXPECT_TRUE(std::equal(testData.cbegin(), testData.cend() - 1, referenceData.cbegin()));
            EXPECT_EQ(testData.back(), testVal);
        }
        // fill to capacity
        {
            container_type testData = referenceData;
            constexpr DataType testVal = 534;
            const auto numberToAdd = testData.capacity() - testData.size();
            while(testData.size() != testData.capacity())
            {
                testData.push_back(testVal);
            }

            EXPECT_EQ(testData.size(), testData.capacity());
            EXPECT_TRUE(std::equal(referenceData.cbegin(), referenceData.cend(), testData.cbegin()));
            EXPECT_TRUE(std::all_of(testData.cbegin() + numberToAdd, testData.cend(), [&](const auto &val) { return val == testVal;}));
        }
        // over fill
        {
            container_type testData = referenceData;
            constexpr DataType testVal = 534;
            const auto numberToAdd = testData.capacity() - testData.size() + 3;
            for(size_t ii = 0; ii < numberToAdd; ++ii)
            {
                testData.push_back(testVal);
            }
            EXPECT_EQ(testData.size(), testData.capacity());
            EXPECT_FALSE(std::equal(referenceData.cbegin(), referenceData.cbegin()+3, testData.cbegin()));
            EXPECT_TRUE(std::equal(referenceData.cbegin()+3, referenceData.cend(), testData.cbegin()));
            EXPECT_TRUE(std::all_of(testData.cbegin() + numberToAdd, testData.cend(), [&](const auto &val) { return val == testVal;}));
        }
    }
    // pop_back
    {
        // single pop
        {
            const container_type refData = createDataSet(SizeCount);
            container_type testData = refData;
            testData.pop_back();
            EXPECT_EQ(testData.size(), refData.size() - 1);
            EXPECT_TRUE(std::equal(testData.cbegin(), testData.cend(), refData.cbegin()));
        }
        // pop until empty
        {
            const container_type refData = createDataSet(SizeCount);
            container_type testData = refData;
            while(!testData.empty())
            {
                testData.pop_back();
            }
            EXPECT_TRUE(testData.empty());
        }
    }
    // resize
    {
        const container_type refData = createDataSet(SizeCount);
        // resize larger
        {
            container_type testData = refData;
            testData.resize(testData.capacity());

            // size and cap should be equivalent, and should contain a bunch of zeros at the end
            EXPECT_EQ(testData.size(), testData.capacity());
            EXPECT_TRUE(std::equal(refData.cbegin(), refData.cend(), testData.cbegin()));
            EXPECT_TRUE(std::all_of(testData.cbegin() + refData.size(), testData.cend(), [](const auto &val){ return val == 0;}));
        }
        // resize smaller
        {
            container_type testData = refData;
            testData.resize(refData.size()/2);

            EXPECT_EQ(testData.size(), refData.size()/2);
            EXPECT_TRUE(std::equal(testData.cbegin(), testData.cend(), refData.cbegin()));
        }

    }
    // swap
    {
        const container_type test1 {createDataSet(SizeCount)};
        const container_type test2 {test1.cbegin(), test1.cbegin() + test1.size()/2};

        container_type test1_postSwap = test1;
        container_type test2_postSwap = test2;
        test1_postSwap.swap(test2_postSwap);

        EXPECT_EQ(test1_postSwap.size(), test2.size());
        EXPECT_EQ(test2_postSwap.size(), test1.size());
        EXPECT_TRUE(std::equal(test2_postSwap.cbegin(), test2_postSwap.cend(), test1.cbegin()));
        EXPECT_TRUE(std::equal(test1_postSwap.cbegin(), test1_postSwap.cend(), test2.cbegin()));
    }
}
#if 1
TEST(VariableArray, NonMemberFunctions_PositiveCase)
{
    using DataType = uint32_t;
    constexpr size_t N = 71;
    using container_type = embp::circular_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;

    auto createDataSet = [](const size_t count)
    {
        container_type baseDataSet (count);
        std::iota(baseDataSet.begin(), baseDataSet.end(), DataType{});
        return baseDataSet;
    };

    // operator ==
    // operator !=
    // operator <
    // operator <=
    // operator >
    // operator >=
    {
        /* lexographically less is the container whose first different element
         * is less than (as determined by operator< on value_type) the
         * corresponding element in the other container
         */

        // identical
        {
            const auto t = createDataSet(N / 2);
            const auto t2 = createDataSet(N / 2);

            EXPECT_TRUE(t.size() == t2.size());
            EXPECT_TRUE(t == t);
            EXPECT_TRUE(t2 == t2);
            EXPECT_TRUE(t == t2);
            EXPECT_FALSE(t != t2);

            EXPECT_FALSE(t < t2);
            EXPECT_TRUE(t <= t2);
            EXPECT_FALSE(t > t2);
            EXPECT_TRUE(t >= t2);

        }
        // different sizes
        {
            const auto t = createDataSet(N / 3);
            const auto t2 = createDataSet(N / 2);

            EXPECT_FALSE(t.size() == t2.size());
            EXPECT_TRUE(t == t);
            EXPECT_TRUE(t2 == t2);
            EXPECT_FALSE(t == t2);
            EXPECT_TRUE(t != t2);

            EXPECT_TRUE(t < t2);
            EXPECT_TRUE(t <= t2);
            EXPECT_FALSE(t > t2);
            EXPECT_FALSE(t >= t2);
        }
        // same size, different values
        {
            const auto t = createDataSet(N / 2);
            auto t2 = createDataSet(N / 2);
            ++t2[t2.size()/2];

            EXPECT_TRUE(t.size() == t2.size());
            EXPECT_TRUE(t == t);
            EXPECT_TRUE(t2 == t2);
            EXPECT_FALSE(t == t2);
            EXPECT_TRUE(t != t2);

            EXPECT_TRUE(t < t2);
            EXPECT_TRUE(t <= t2);
            EXPECT_FALSE(t > t2);
            EXPECT_FALSE(t >= t2);
        }
        // one is empty
        {
            const auto t2 = createDataSet(N / 4);
            const auto t = decltype (t2){};

            EXPECT_FALSE(t.size() == t2.size());
            EXPECT_TRUE(t == t);
            EXPECT_TRUE(t2 == t2);
            EXPECT_FALSE(t == t2);
            EXPECT_TRUE(t != t2);

            EXPECT_TRUE(t < t2);
            EXPECT_TRUE(t <= t2);
            EXPECT_FALSE(t > t2);
            EXPECT_FALSE(t >= t2);
        }
        // both are empty
        {
            const auto t = container_type{};
            const auto t2 = decltype (t){};

            EXPECT_TRUE(t.size() == t2.size());
            EXPECT_TRUE(t == t);
            EXPECT_TRUE(t2 == t2);
            EXPECT_TRUE(t == t2);
            EXPECT_FALSE(t != t2);

            EXPECT_FALSE(t < t2);
            EXPECT_TRUE(t <= t2);
            EXPECT_FALSE(t > t2);
            EXPECT_TRUE(t >= t2);
        }
    }
    // swap
    {
        const container_type test1 {createDataSet(N/2)};
        const container_type test2 {test1.cbegin(), test1.cbegin() + test1.size()/2};

        container_type test1_postSwap = test1;
        container_type test2_postSwap = test2;
        embp::swap(test1_postSwap, test2_postSwap);

        EXPECT_EQ(test1_postSwap.size(), test2.size());
        EXPECT_EQ(test2_postSwap.size(), test1.size());
        EXPECT_TRUE(std::equal(test2_postSwap.cbegin(), test2_postSwap.cend(), test1.cbegin()));
        EXPECT_TRUE(std::equal(test1_postSwap.cbegin(), test1_postSwap.cend(), test2.cbegin()));
    }
}
#endif
