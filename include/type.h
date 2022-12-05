#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

template <typename T>
using Vec = std::vector<T>;

template <typename K, typename V>
using Map = std::map<K, V>;

template <typename T>
using Rc = std::shared_ptr<T>;

using Str = std::string;
