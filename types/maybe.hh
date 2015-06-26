#ifndef TYPES_MAYBE_HH
#define TYPES_MAYBE_HH

#include <experimental/optional>

template <typename T>
using maybe = std::experimental::optional <T>;

#endif
