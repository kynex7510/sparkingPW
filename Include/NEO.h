#ifndef _SPARKINGPW_NEO_H
#define _SPARKINGPW_NEO_H

#include "Util.h"

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
  // TODO: Australia has its own code? And Korea?
};

class CharacterData {
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

  constexpr static char const DICT[] =
      "QRSTUVWXIJKLMNOPABCDEFGHYZabcdefwxyz!#$%opqrstuvghijklmn&@-+*()?";

public:
  constexpr static std::size_t MAX_ZITEMS = 7;

protected:
  std::uint32_t m_ID;
  std::uint32_t m_ZItems[7];
  AILogic m_AILogic;
  std::uint32_t m_Target; // TODO: unknown purpose.
  Country m_Country;
  std::uint32_t m_PasswordType; // TODO: unknown purpose.

public:
  CharacterData()
      : m_ID(0), m_AILogic(AILogic::Balance), m_Target(0),
        m_Country(Country::Japan), m_PasswordType(0) {
    std::fill(m_ZItems, m_ZItems + MAX_ZITEMS, 0);
  }

  static CharacterData deserialize(std::string_view const pw) {
    CharacterData data;
    std::uint8_t buffer[24] = {};

    // Decode and decipher password.
    util::pwDecode(buffer, pw, TABLE);
    util::pwCipher(buffer);

    // Verify checksum.
    auto const checksum = util::readBits(buffer, 119, 8);
    if (checksum != util::pwChecksum(buffer))
      return data;

    // Verify signature.
    for (std::size_t i = 0; i < 8; i++) {
      auto const v = buffer[i] ^ buffer[i + 8];
      if (v != buffer[16 + i])
        return data;
    }

    // Read data.
    data.m_ID = util::readBits(buffer, 32, 8);

    for (std::size_t i = 0; i < MAX_ZITEMS; ++i)
      data.m_ZItems[i] = util::readBits(buffer, 40 + (10 * i), 10);

    data.m_AILogic = static_cast<AILogic>(util::readBits(buffer, 110, 3));
    data.m_Target = util::readBits(buffer, 113, 1);
    data.m_Country = static_cast<Country>(util::readBits(buffer, 114, 4));
    data.m_PasswordType = util::readBits(buffer, 118, 1);

    return data;
  }

  std::string serialize(std::uint32_t seed = 0) const {
    std::string out(32, ' ');
    std::uint8_t buffer[24] = {};

    // Write data.
    util::writeBits(buffer, 0, 32, seed);
    util::writeBits(buffer, 32, 8, m_ID);

    for (std::size_t i = 0; i < MAX_ZITEMS; ++i)
      util::writeBits(buffer, 40 + (10 * i), 10, m_ZItems[i]);

    util::writeBits(buffer, 110, 3, static_cast<std::uint8_t>(m_AILogic));
    util::writeBits(buffer, 113, 1, m_Target);
    util::writeBits(buffer, 114, 4, static_cast<std::uint8_t>(m_Country));
    util::writeBits(buffer, 118, 1, m_PasswordType);

    // Compute checksum.
    auto const checksum = util::pwChecksum(buffer);
    util::writeBits(buffer, 119, 8, checksum);

    // Compute signature.
    for (std::size_t i = 0; i < 8; ++i)
      buffer[16 + i] = buffer[i] ^ buffer[8 + i];

    // Encipher and encode password.
    util::pwCipher(buffer);
    util::pwEncode(out, buffer, DICT);

    return out;
  }

  std::uint32_t getID() const { return m_ID; }
  void setID(std::uint32_t id) { m_ID = id; }

  std::uint32_t getZItem(std::size_t index) const {
    if (index < MAX_ZITEMS)
      return m_ZItems[index];

    return 0;
  }

  void setZItem(std::size_t index, std::uint32_t item) {
    if (index < MAX_ZITEMS)
      m_ZItems[index] = item;
  }

  AILogic getAILogic() const { return m_AILogic; }
  void setAILogic(AILogic logic) { m_AILogic = logic; }

  std::uint32_t getTarget() const { return m_Target; }

  Country getCountry() const { return m_Country; }
  void setCountry(Country country) { m_Country = country; }

  std::uint32_t getPasswordType() const { return m_PasswordType; }
};
} // namespace sparking::neo

#endif /* _SPARKINGPW_NEO_H */