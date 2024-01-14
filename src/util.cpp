#include <util.hpp>

#include <stdexcept>

void nullptrCheck(const void *ptr) {
    if (ptr == nullptr) {
        throw std::runtime_error("Error parsing file.");
    }
}
