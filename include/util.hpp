#ifndef UTIL_HPP
#define UTIL_HPP

#include <string_view>

void nullptrCheck(const void *ptr);

[[noreturn]]
void halt(std::string_view message);

#endif
