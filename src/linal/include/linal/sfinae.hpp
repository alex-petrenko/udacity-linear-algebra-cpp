#pragma once

/// SFINAE is a C++ meta-programming trick, short for
/// "substitution failure is not an error".


namespace Linal
{

/// Tells if type is a specialization of a certain template or not.
/// Implemented via partial template specialization. Note syntax "template <typename...> typename TEMPLATE_T" - 
/// that's a template passed as a template argument. Crazy.
/// See tests for reference.
template <template <typename...> typename TEMPLATE_T, typename T>
struct IsSpecializationOf : std::false_type {};
template <template <typename...> typename TEMPLATE_T, typename... ARGS>
struct IsSpecializationOf<TEMPLATE_T, TEMPLATE_T<ARGS...>> : std::true_type {};

/// Simple helper to avoid typing ::value.
template <template <typename...> typename TEMPLATE_T, typename T>
constexpr bool IsSpecializationOfV = IsSpecializationOf<TEMPLATE_T, T>::value;

}