#ifndef GENERATOR_HEADER_HPP
#define GENERATOR_HEADER_HPP

class Protocol;

class GeneratorHeader {
  public:
    explicit GeneratorHeader() noexcept = default;

    static void dump(const Protocol &protocol) noexcept;
};
#endif
