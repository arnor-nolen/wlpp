#ifndef UTIL_HPP
#define UTIL_HPP

#include <string_view>

void nullptrCheck(const void *ptr);

[[noreturn]]
void halt(std::string_view message);

auto snakeToPascal(std::string_view snakeCase) -> std::string;

auto snakeToCamel(std::string_view snakeCase) -> std::string;

#endif
