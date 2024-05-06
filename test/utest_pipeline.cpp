///
/// Copyright (c) 2024, Matyas Lerant-Nyeste
/// All rights reserved
///
/// This file is part of the Compile-Time-Execution-Structs project.
/// Redistribution and use with or without modification, in source and
/// binary forms, for non-commercial or commercial purposes, are permitted
/// exclusively under the terms of CC BY 4.0 license. You should have
/// received a copy of the license with this file. If not, please or visit:
/// https://creativecommons.org/licenses/by/4.0/legalcode.en
///
/// Contact: matyas.lerant.nyeste (at) gmail.com
///

#include <ctes/kernel.h>
#include <ctes/pipeline.h>

#include <vector>
#include <algorithm>
#include <numeric>

class KernelDecStdVectorElementsByValue : public KernelBase<std::vector<int>>
{
public:

    KernelDecStdVectorElementsByValue(int value)
        : value_(value)
    {
    }

    std::vector<int>* operator()(std::vector<int>& input_data) override
    {
        for (auto& x : input_data)
        {
            x -= value_;
        }
        return &input_data;
    }

private:

    int value_;
};

class KernelFilterAllStdVectorElementsNonNegative : public KernelBase<std::vector<int>>
{
public:

    std::vector<int>* operator()(std::vector<int>& input_data) override
    {
        const auto all_non_negative = std::all_of(input_data.begin(), input_data.end(), [](int x)->bool { return x >= 0; });
        return all_non_negative ? &input_data : nullptr;
    }
};

class KernelSumOfStdVec : public KernelBase<std::vector<int>, int>
{
public:

    int* operator()(std::vector<int>& input_data) override
    {
        output_data_ = std::accumulate(input_data.begin(), input_data.end(), 0);
        return &output_data_;
    }

private:

    int output_data_;
};

TEST(CTES_PIPELINE, EXECUTION_SUCCESS)
{
    using Pipeline = Pipeline<
        KernelDecStdVectorElementsByValue,
        KernelFilterAllStdVectorElementsNonNegative,
        KernelSumOfStdVec>;

    Pipeline pipeline({ 2 }, {}, {});

    std::vector<int> v{ 2, 3, 4 };
    const auto res = pipeline.execute(v);
    ASSERT_NE(res, nullptr);
    EXPECT_EQ(*res, 3);
}

TEST(CTES_PIPELINE, EXECUTION_FAILURE)
{
    using Pipeline = Pipeline<
        KernelDecStdVectorElementsByValue,
        KernelFilterAllStdVectorElementsNonNegative,
        KernelSumOfStdVec>;

    Pipeline pipeline({ 2 }, {}, {});

    std::vector<int> v{ 2, 1, 4 };
    const auto res = pipeline.execute(v);
    ASSERT_EQ(res, nullptr);
}
