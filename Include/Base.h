#ifndef _SPARKINGPW_BASE_H
#define _SPARKINGPW_BASE_H

#include "Util.h"

#include <cmath>
#include <string>
#include <string_view>

namespace sparking::base {
template <typename Derived> class Data {
protected:
  std::uint32_t m_Seed;

  void encode(std::string &out, std::span<std::uint8_t const> const in) {
    auto const numSextets = (in.size() * 8 / 6);
    for (std::size_t i = 0; i < numSextets; ++i) {
      if (i >= out.size())
        break;

      out[i] = Derived::DICT[util::readBits<std::uint8_t, 6>(in, i * 6) & 0x3F];
    }
  }

  void decode(std::span<std::uint8_t> out, std::string_view const in) {
    for (std::size_t i = 0; i < in.size(); ++i) {
      auto const offset = i * 6;
      if (((offset + 6) / 8) > out.size())
        break;

      util::writeBits<std::size_t, 6>(out, offset,
                                      Derived::TABLE[in[i] & 0x7F]);
    }
  }

  virtual bool read(std::span<std::uint8_t const> const buffer,
                    bool skipVerification) = 0;
  virtual void write(std::span<std::uint8_t> buffer) = 0;

public:
  Data() : m_Seed(0) {}

  bool deserialize(std::string_view const pw, bool skipVerification = false) {
    std::uint8_t buffer[Derived::DATA_SIZE] = {};

    if (pw.size() != Derived::PASSWORD_SIZE)
      return false;

    decode(buffer, pw);
    m_Seed = util::readBits<std::uint32_t, 32>(buffer, 0);
    util::cipher(buffer, 4, m_Seed);
    return read(buffer, skipVerification);
  }

  std::string serialize() {
    std::string out(Derived::PASSWORD_SIZE, ' ');
    std::uint8_t buffer[Derived::DATA_SIZE] = {};

    util::writeBits<std::uint32_t>(buffer, 0, m_Seed);
    write(buffer);
    util::cipher(buffer, 4, m_Seed);
    encode(out, buffer);
    return out;
  }

  std::uint32_t getSeed() const { return m_Seed; }
  void setSeed(std::uint32_t seed) { m_Seed = seed; }
};
} // namespace sparking::base

#endif /* _SPARKINGPW_BASE_H */