#ifndef _SPARKINGPW_NEO_H
#define _SPARKINGPW_NEO_H

#include "Base.h"

#include <string>

namespace sparking::neo {
enum class AILogic {
  Balance = 0,
  Powerful = 1,
  Technical = 2,
};

enum class Country {
  Japan = 0,
  America = 1,
  UK = 2,
  France = 3,
  Italy = 4,
  Germany = 5,
  Spain = 6,
  // TODO: Does Australia have its own code? And Korea?
};

class CharacterData : public base::Data<CharacterData> {
  friend class base::Data<CharacterData>;

public:
  constexpr static std::size_t NUM_Z_ITEMS = 7;

protected:
  constexpr static std::size_t PASSWORD_SIZE = 32;
  constexpr static std::size_t DATA_SIZE = 24;

  constexpr static char const DICT[] =
      "QRSTUVWXIJKLMNOPABCDEFGHYZabcdefwxyz!#$%opqrstuvghijklmn&@-+*()?";

  constexpr static std::size_t TABLE[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  36, 0,  37, 38, 39, 56, 0,  61, 62, 60, 59, 0,  58, 0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  63,
      57, 16, 17, 18, 19, 20, 21, 22, 23, 8,  9,  10, 11, 12, 13, 14,
      15, 0,  1,  2,  3,  4,  5,  6,  7,  24, 25, 0,  0,  0,  0,  0,
      0,  26, 27, 28, 29, 30, 31, 48, 49, 50, 51, 52, 53, 54, 55, 40,
      41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 0,  0,  0,  0,  0,
  };

  std::uint8_t m_ID;
  std::uint32_t m_ZItems[7];
  AILogic m_AILogic;
  bool m_Target; // TODO: unknown purpose.
  Country m_Country;
  bool m_PasswordType; // TODO: unknown purpose.

  static std::uint8_t
  generateChecksum(std::span<std::uint8_t const> const buffer) {
    std::uint8_t checksum = 0;
    for (std::size_t i = 0; i < 119; ++i)
      checksum += util::readBit(buffer, i);
    return checksum;
  }

  bool read(std::span<std::uint8_t const> const buffer,
            bool skipVerification) override {
    if (!skipVerification) {
      // Verify checksum.
      auto const checksum = util::readBits<std::uint8_t, 8>(buffer, 119);
      if (checksum != generateChecksum(buffer))
        return false;

      // Verify signature.
      for (std::size_t i = 0; i < 8; i++) {
        auto const v = buffer[i] ^ buffer[8 + i];
        if (v != buffer[16 + i])
          return false;
      }
    }

    // Read data.
    m_ID = util::readBits<std::uint8_t>(buffer, 32);

    for (std::size_t i = 0; i < NUM_Z_ITEMS; ++i)
      m_ZItems[i] = util::readBits<std::uint32_t, 10>(buffer, 40 + (10 * i));

    m_AILogic = util::readBits<AILogic, 3>(buffer, 110);
    m_Target = util::readBits<std::uint32_t, 1>(buffer, 113);
    m_Country = util::readBits<Country, 4>(buffer, 114);
    m_PasswordType = util::readBits<std::uint32_t, 1>(buffer, 118);
    return true;
  }

  void write(std::span<std::uint8_t> buffer) override {
    // Write data.
    util::writeBits<std::uint8_t>(buffer, 32, m_ID);

    for (std::size_t i = 0; i < NUM_Z_ITEMS; ++i)
      util::writeBits<std::uint32_t, 10>(buffer, 40 + (10 * i), m_ZItems[i]);

    util::writeBits<AILogic, 3>(buffer, 110, m_AILogic);
    util::writeBit(buffer, 113, m_Target);
    util::writeBits<Country, 4>(buffer, 114, m_Country);
    util::writeBit(buffer, 118, m_PasswordType);

    // Write checksum.
    util::writeBits<std::uint8_t>(buffer, 119, generateChecksum(buffer));

    // Write signature.
    for (std::size_t i = 0; i < 8; i++)
      buffer[16 + i] = buffer[i] ^ buffer[8 + i];
  }

public:
  CharacterData()
      : m_ID(0), m_AILogic(AILogic::Balance), m_Target(false),
        m_Country(Country::Japan), m_PasswordType(false) {
    std::fill(m_ZItems, m_ZItems + NUM_Z_ITEMS, 0);
  }

  std::uint8_t getID() const { return m_ID; }
  void setID(std::uint8_t id) { m_ID = id; }

  std::uint32_t getZItem(std::size_t index) const {
    if (index < NUM_Z_ITEMS)
      return m_ZItems[index];

    return 0;
  }

  void setZItem(std::size_t index, std::uint32_t item) {
    if (index < NUM_Z_ITEMS)
      m_ZItems[index] = item;
  }

  AILogic getAILogic() const { return m_AILogic; }
  void setAILogic(AILogic logic) { m_AILogic = logic; }

  bool getTarget() const { return m_Target; }

  Country getCountry() const { return m_Country; }
  void setCountry(Country country) { m_Country = country; }

  bool getPasswordType() const { return m_PasswordType; }
};

} // namespace sparking::neo

#endif /* _SPARKINGPW_NEO_H */