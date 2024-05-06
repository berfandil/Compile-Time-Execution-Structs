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

namespace ctes {

//-------------------------------------------------------------------------

/// @brief Pipeline for executing kernels in a consecutive manner. Since
///        the pipeline takes its input as a non-const reference, its
///        lifetime should be guaranteed on the caller side. The pipeline,
///        once executed, returns either a pointer to the resulting data -
///        which may as well have the same address as the input - on
//         success, or a nullptr on failure.
template <class ...Kernels>
class Pipeline : public Kernels...
{
private:

    using KernelTypes = typename std::tuple<Kernels...>;

    template <size_t n>
    using nth_input_t = typename std::tuple_element_t<n, KernelTypes>::input_t;

    template <size_t n>
    using nth_output_t = typename std::tuple_element_t<n, KernelTypes>::output_t;

public:

    static_assert((is_kernel_v<Kernels> && ...), "All pprovided kernels have to be derived from IKernel!");

    static constexpr auto num_steps = sizeof...(Kernels);
    static_assert(num_steps > 0, "At least one kernel has to be provided!");

    using input_t  = nth_input_t<0>;              ///< The input type of the pipeline
    using output_t = nth_output_t<num_steps - 1>; ///< The output type of the pipeline

    /// @brief Constructor.
    /// @param kernels Rvalues to the contained kernels.
    explicit Pipeline(Kernels&& ...kernels)
        : Kernels(std::move(kernels))...
    {
        staticAssertTypeContinuity(std::make_index_sequence<num_steps - 1>());
    }

    /// @brief Executes the pipeline.
    /// @param input_data The input data. Since it is a non-const reference,
    ///        its lifetime should be guaranteed on the caller side.
    /// @return A pointer to the resulting data - which may as well have the
    ///         same address as the input - on success, or a nullptr on
    ///         failure.
    output_t* execute(input_t& input_data)
    {
        return executeImpl(std::make_index_sequence<num_steps>(), input_data);
    }

private:

    /// @brief Asserts, that all kernels' input types match their respective preceding kernels' output types.
    template <size_t ...i>
    static constexpr void staticAssertTypeContinuity(std::index_sequence<i...>)
    {
        static_assert((std::is_same_v<nth_output_t<i>, nth_input_t<i + 1>> && ...), "The input type of the provided kernels have to be the same as the output type of their respective preceding kernels!");
    }

    /// @brief Implementation for executing the pipeline.
    /// @param input_data The input data. Since it is a non-const reference,
    ///        its lifetime should be guaranteed on the caller side.
    /// @return A pointer to the resulting data - which may as well have the
    ///         same address as the input - on success, or a nullptr on
    ///         failure.
    template <size_t ...i>
    output_t* executeImpl(std::index_sequence<i...>, input_t& input_data)
    {
        std::tuple<nth_input_t<i>*..., output_t*> data_ptrs;
        std::get<0>(data_ptrs) = &input_data;

        ((std::get<i + 1>(data_ptrs) = static_cast<Kernels&>(*this)(*std::get<i>(data_ptrs))) && ...);

        return std::get<num_steps>(data_ptrs);
    }
};

} // namespace ctes
