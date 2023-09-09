#pragma once

std::string GetExeFilename();

template <class Container>
const bool contains(const Container& container, const typename Container::value_type& element) {
    return std::find(container.begin(), container.end(), element) != container.end();
}