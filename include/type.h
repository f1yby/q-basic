#pragma once
#include <memory>
#include <string>
#include <vector>

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Rc = std::shared_ptr<T>;

using Str = std::string;
