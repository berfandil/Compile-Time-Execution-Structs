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

#include <type_traits>

namespace ctes {

/// @brief Interface implementation for a kernel executing some algorithm
///        on some data. Since the kernel takes its input as a non-const
///        reference, its lifetime should be guaranteed on the caller side.
///        The kernel, once executed, returns either a pointer to the
///        resulting data - which may as well have the same address as the
///        input - on success, or a nullptr on failure.
template <typename InputType,
          typename OutputType = InputType>
class IKernel
{
public:

    using input_t = InputType;
    using output_t = OutputType;

    /// @brief Executes the kernel.
    /// @param input_data The input data. Since it is a non-const reference,
    ///        its lifetime should be guaranteed on the caller side.
    /// @return A pointer to the resulting data - which may as well have the
    ///         same address as the input - on success, or a nullptr on
    ///         failure.
    virtual output_t* operator()(input_t& input_data) = 0;
};

/// @brief Type trait for checking, whether some class is a kernel.
template <class,
          typename = void>
struct is_kernel : public std::false_type {};

/// @brief Type trait for checking, whether some class is a kernel.
template <class T>
struct is_kernel<T, std::void_t<typename T::input_t, typename T::output_t>>
{
    static constexpr auto value = std::is_base_of_v<IKernel<typename T::input_t, typename T::output_t>, T>;
};

/// @brief The type trait value of checking, whether some class is a kernel.
template <class T>
constexpr auto is_kernel_v = is_kernel<T>::value;

} // namespace ctes
