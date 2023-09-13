#pragma once

std::string GetExeFilename();

template <class Container>
bool Contains(const Container& container, const typename Container::value_type& element) {
    return std::find(container.begin(), container.end(), element) != container.end();
}

void PrintNBytes(const BYTE* address, int length);