#pragma once

std::string GetExePath();
std::string GetExeFilename();

template <class Container>
bool Contains(const Container& container, const typename Container::value_type& element) {
    return std::find(container.begin(), container.end(), element) != container.end();
}

void   PrintNBytes(const BYTE* address, int length);
bool   EndsWith(std::string const& value, std::string const& ending);
UINT32 GetGameVersion();