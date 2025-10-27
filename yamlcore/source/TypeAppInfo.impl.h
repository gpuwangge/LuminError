#pragma once
template <typename T>
T getOrDefault(const YAML::Node& node, const std::string& key, const T& defaultValue) {
    if (node[key]) {
        return node[key].as<T>();
    }
    return defaultValue;
}
