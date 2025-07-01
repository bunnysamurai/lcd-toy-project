#include <numeric>
#include <gtest/gtest.h>

#include "variable_array.hpp"

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


TEST(VariableArray, Construction_PositiveCase)
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
        using value_type = typename get_base_type<decltype(container)>::base_type::value_type;
        static_assert(
                    std::is_pointer_v<typename base_type::iterator>
                    &&
                    !std::is_const_v<std::remove_pointer_t<typename base_type::iterator>>
                    &&
                    std::is_same_v<std::remove_pointer_t<typename base_type::iterator>, value_type>
                    &&
                    std::is_pointer_v<typename base_type::const_iterator>
                    &&
                    std::is_const_v<std::remove_pointer_t<typename base_type::const_iterator>>
                    &&
                    std::is_same_v<std::remove_pointer_t<typename base_type::const_iterator>, std::add_const_t<value_type>>
        );
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
        static_assert (std::is_same_v<base_type, embp::variable_array<DataType, N>>);
        checkBaseTypes(container);
        checkRefSpec(container);
        checkPointerSpec(container);
        checkIteratorSpec(container);
        return container.capacity() == N;
    };

    // Default
    {
        embp::variable_array<DataType, N> t;
        EXPECT_TRUE(checkTypeSpec(t));
        EXPECT_EQ(t.size(), 0);
    }

    // Size specifier
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::variable_array<DataType, N> t(ii);
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_EQ(t.size(), ii);
        }
    }

    // Size specifier, fill with value
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::variable_array<DataType, N> t(ii, testVal);
            EXPECT_TRUE(checkTypeSpec(t));
            EXPECT_EQ(t.size(), ii);
            EXPECT_TRUE(std::all_of(t.begin(), t.end(), [&](const auto &val){ return val == testVal; }));
        }
    }

    // Iterator range
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            std::vector<DataType> testVector(ii);
            std::iota(testVector.begin(), testVector.end(), DataType{});
            embp::variable_array<DataType, N> t(testVector.cbegin(), testVector.cend());
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
            embp::variable_array<DataType, N> t1(ii, testVal);
            auto t2{t1};

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Move Construct
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::variable_array<DataType, N> t1(ii, testVal);
            auto t2{std::move(t1)};

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Copy assignment
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::variable_array<DataType, N> t1(ii, testVal);
            embp::variable_array<DataType, N> t2;
            t2 = t1;

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Move assignment
    {
        for(size_t ii = 1; ii < N + 1; ++ii)
        {
            embp::variable_array<DataType, N> t1(ii, testVal);
            embp::variable_array<DataType, N> t2;
            t2 = std::move(t1);

            EXPECT_EQ(t1.size(), ii);
            EXPECT_TRUE(checkTwoAreEqual(t1, t2));
        }
    }

    // Size spec assign
    {
        embp::variable_array<DataType, N> t;
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
        embp::variable_array<DataType, N> t;
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

TEST(VariableArray, ElementAccess_PositiveCase)
{
    using DataType = uint16_t;
    constexpr size_t N = 150;
    using container_type = embp::variable_array<DataType, N>;

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
        ::testing::StaticAssertTypeEq<decltype(nonConst[0]), typename decltype(nonConst)::reference>();
        ::testing::StaticAssertTypeEq<decltype(constExample[0]), typename decltype(nonConst)::const_reference>();
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
        ::testing::StaticAssertTypeEq<decltype(nonConst.front()), typename decltype(nonConst)::reference>();
        ::testing::StaticAssertTypeEq<decltype(constExample.front()), typename decltype(nonConst)::const_reference>();
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
        ::testing::StaticAssertTypeEq<decltype(nonConst.back()), typename decltype(nonConst)::reference>();
        ::testing::StaticAssertTypeEq<decltype(constExample.back()), typename decltype(nonConst)::const_reference>();
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
        ::testing::StaticAssertTypeEq<decltype(nonConst.data()), typename decltype(nonConst)::pointer>();
        ::testing::StaticAssertTypeEq<decltype(constExample.data()), typename decltype(nonConst)::const_pointer>();
        // value check
        bool dataAccessValueCheck = true;
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            dataAccessValueCheck &= *(nonConst.data() + ii) == ii && *(constExample.data() + ii) == ii;
        }
        EXPECT_TRUE(dataAccessValueCheck);
        // write check
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            ++(*(nonConst.data() + ii));
        }
        bool dataAccessValueCheck2nd = true;
        for(size_t ii = 0; ii < nonConst.size(); ++ii)
        {
            dataAccessValueCheck2nd &= *(nonConst.data() + ii) == (*(constExample.data() + ii) + 1);
        }
        EXPECT_TRUE(dataAccessValueCheck2nd);
    }
}

TEST(VariableArray, Iterator_PositiveCase)
{
    using DataType = double;
    constexpr size_t N = 34;
    using container_type = embp::variable_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;
    const container_type baseDataSet = [&]()
    {
        container_type rv (SizeCount);
        std::iota(rv.begin(), rv.end(), DataType{});
        return rv;
    }();

    // Begin
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;

        auto &&checkBegin = [](auto &container)
        {

            ::testing::StaticAssertTypeEq<decltype(container.data()), decltype(container.begin())>();
            EXPECT_EQ(container.data(), container.begin());
            EXPECT_EQ(*(container.data()), *(container.begin()));
            if constexpr (std::is_const_v<std::remove_reference_t<decltype(container)>>)
            {
                ::testing::StaticAssertTypeEq<decltype(container.data()), decltype(container.cbegin())>();
            }
            else
            {
                static_assert(!std::is_same_v<decltype(container.data()), decltype(container.cbegin())>);
            }
            EXPECT_EQ(container.data(), container.cbegin());
            EXPECT_EQ(*(container.data()), *(container.cbegin()));
        };
        checkBegin(nonConst);
        checkBegin(constExample);
    }

    // End
    {
        container_type nonConst = baseDataSet;
        const container_type constExample = baseDataSet;

        auto &&checkEnd = [](auto &container)
        {

            ::testing::StaticAssertTypeEq<decltype(container.data()), decltype(container.end())>();
            EXPECT_EQ(container.data() + container.size(), container.end());
            EXPECT_EQ(*(container.data() + container.size() - 1), *(container.end() - 1));
            if constexpr (std::is_const_v<std::remove_reference_t<decltype(container)>>)
            {
                ::testing::StaticAssertTypeEq<decltype(container.data()), decltype(container.cend())>();
            }
            else
            {
                static_assert(!std::is_same_v<decltype(container.data()), decltype(container.cend())>);
            }
            EXPECT_EQ(container.data() + container.size(), container.cend());
            EXPECT_EQ(*(container.data() + container.size() - 1), *(container.cend() - 1));
        };
        checkEnd(nonConst);
        checkEnd(constExample);
    }

    // reverse iterators!
    {
        container_type data = baseDataSet;
        // hand check
        for(size_t ii = 0; ii < data.size(); ++ii)
        {
            EXPECT_EQ(data[data.size() - ii - 1], *(data.crbegin() + ii));
        }
        
        auto cpy = data;
        std::reverse(cpy.begin(), cpy.end());
        EXPECT_TRUE(std::equal(data.crbegin(), data.crend(), cpy.cbegin()));
    }
}

TEST(VariableArray, Capacity_PositiveCase)
{
    using DataType = double;
    constexpr size_t N = 34;
    using container_type = embp::variable_array<DataType, N>;

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

TEST(VariableArray, Modifiers_PositiveCase)
{
    using DataType = uint32_t;
    constexpr size_t N = 34;
    using container_type = embp::variable_array<DataType, N>;

    constexpr size_t SizeCount = N / 2;

    constexpr auto createDataSet = []()
    {
        container_type baseDataSet (SizeCount);
        for(size_t ii = 0; ii < baseDataSet.size(); ++ii)
        {
            baseDataSet[ii] = ii;
        }
        return baseDataSet;
    };
#ifndef NDEBUG
    auto &&printArray = [](const char* name, const auto &container)
    {
        std::cerr << name << ": { ";
        for(const auto &c : container)
            std::cerr << c << " ";
        std::cerr << "}\n";
    };
#endif

    // clear
    {
        container_type testData = createDataSet();
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
            const container_type referenceData = createDataSet();
            container_type testData = referenceData;
            const DataType val = 534;
            auto returnIter = testData.insert(testData.begin(), val);
            EXPECT_EQ(returnIter, testData.begin());
            EXPECT_EQ(testData.front(), val);

            EXPECT_EQ(testData.size(), referenceData.size() + 1);
            EXPECT_TRUE(std::transform_reduce(
                            referenceData.cbegin(), referenceData.cend(), testData.cbegin() + 1,
                            true,
                            [](const auto &l, const auto &r){ return l && r; },
                            [](const auto &l, const auto &r){ return l == r; }
                        )
                       );
        }
        // rvalue value
        {
            const container_type referenceData = createDataSet();
            container_type testData = referenceData;
            constexpr DataType testVal = 534;
            DataType val = testVal;
            auto returnIter = testData.insert(testData.begin(), std::move(val));
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
            const container_type referenceData = createDataSet();
            container_type testData = referenceData;

            const auto pos = testData.begin()+1;
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
            const auto pos_offset = std::distance(testData.begin(), pos);
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
            const container_type referenceData = createDataSet();
            container_type testData = referenceData;

            const auto pos = testData.begin()+1;
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
            const auto pos_offset = std::distance(testData.begin(), pos);
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
                        )
                       );
        }
    }
    // erase
    {
        const container_type referenceData = createDataSet();
        // erase one specific element
        {
            container_type testData = referenceData;
            const auto offset = referenceData.size()/2;
            const auto pos = testData.begin() + offset;

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
                testData.erase(testData.begin());
            EXPECT_TRUE(testData.empty());
        }
        // erase a range
        {
            container_type testData = referenceData;
            const auto start = testData.begin() + referenceData.size() / 3;
            const auto stop = testData.begin() + referenceData.size() / 2;
            ASSERT_NE(start, stop);
            const auto numberToErase = std::distance(start, stop);

            const auto returnIt = testData.erase(start, stop);
            EXPECT_EQ(returnIt, testData.cbegin() + referenceData.size() / 3);
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
            const auto start = testData.begin()+referenceData.size()/2;
            const auto stop = start;

            const auto returnIt = testData.erase(start, stop);
            EXPECT_EQ(returnIt, testData.cbegin() + referenceData.size()/2);
            EXPECT_EQ(testData.size(), referenceData.size());
            EXPECT_TRUE(std::equal(referenceData.cbegin(), referenceData.cend(), testData.cbegin()));
        }
    }
    // push_back
    {
        #ifdef NDEBUG
        constexpr 
        #endif
        container_type referenceData = createDataSet();
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

            EXPECT_TRUE(std::equal(referenceData.cbegin(), referenceData.cend(), testData.cbegin()));
            EXPECT_TRUE(std::all_of(testData.cbegin() + numberToAdd, testData.cend(), [&](const auto &val) { return val == testVal;}));
        }
    }
    // pop_back
    {
        // single pop
        {
            #ifdef NDEBUG
            constexpr 
            #endif
            container_type refData = createDataSet();
            container_type testData = refData;
            testData.pop_back();
            EXPECT_EQ(testData.size(), refData.size() - 1);
            EXPECT_TRUE(std::equal(testData.cbegin(), testData.cend(), refData.cbegin()));
        }
        // pop until empty
        {
            #ifdef NDEBUG
            constexpr 
            #endif
            container_type refData = createDataSet();
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
        #ifdef NDEBUG
        constexpr 
        #endif
        container_type refData = createDataSet();
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
        #ifdef NDEBUG
        constexpr 
        #endif
        container_type test1 {createDataSet()};
        #ifdef NDEBUG
        constexpr 
        #endif
        container_type test2 {test1.cbegin(), test1.cbegin() + test1.size()/2};

        container_type test1_postSwap = test1;
        container_type test2_postSwap = test2;
        test1_postSwap.swap(test2_postSwap);

        EXPECT_EQ(test1_postSwap.size(), test2.size());
        EXPECT_EQ(test2_postSwap.size(), test1.size());
        EXPECT_TRUE(std::equal(test2_postSwap.cbegin(), test2_postSwap.cend(), test1.cbegin()));
        EXPECT_TRUE(std::equal(test1_postSwap.cbegin(), test1_postSwap.cend(), test2.cbegin()));
    }
}

TEST(VariableArray, NonMemberFunctions_PositiveCase)
{
    using DataType = uint32_t;
    constexpr size_t N = 71;
    using container_type = embp::variable_array<DataType, N>;

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
