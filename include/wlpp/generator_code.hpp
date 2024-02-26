#ifndef GENERATOR_CODE_HPP
#define GENERATOR_CODE_HPP

class Protocol;

class GeneratorCode {
  public:
    explicit GeneratorCode() noexcept = default;

    static void dump(const Protocol &protocol) noexcept;
};
#endif
