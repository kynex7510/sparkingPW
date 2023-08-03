#ifndef _SPARKINGPW_METEOR_H
#define _SPARKINGPW_METEOR_H

#include "Base.h"

#include <string>

namespace sparking::meteor {
class CharacterData : public base::Data<CharacterData> {
  friend class base::Data<CharacterData>;

public:
  constexpr static std::size_t NUM_Z_ITEMS = 8;

protected:
  constexpr static std::size_t PASSWORD_SIZE = 34;
  constexpr static std::size_t DATA_SIZE = 26;

  constexpr static char const DICT[] =
      "QRST2VWX1JKLMN5P0BCD3FGHYZ4bcd9fwxyz!#$%6pqrst8vgh7jklmn&@-+*()?";

protected:
  constexpr static std::size_t TABLE[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  36, 0,  37, 38, 39, 56, 0,  61, 62, 60, 59, 0,  58, 0,  0,
      16, 8,  4,  20, 26, 14, 40, 50, 46, 30, 0,  0,  0,  0,  0,  63,
      57, 0,  17, 18, 19, 0,  21, 22, 23, 0,  9,  10, 11, 12, 13, 0,
      15, 0,  1,  2,  3,  0,  5,  6,  7,  24, 25, 0,  0,  0,  0,  0,
      0,  0,  27, 28, 29, 0,  31, 48, 49, 0,  51, 52, 53, 54, 55, 0,
      41, 42, 43, 44, 45, 0,  47, 32, 33, 34, 35, 0,  0,  0,  0,  0,
  };

  std::uint8_t m_ID;
  std::uint32_t m_ZItems[NUM_Z_ITEMS];
  std::uint32_t m_Unknown1;
  bool m_Unknown2;

  static std::uint8_t
  generateChecksum(std::span<std::uint8_t const> const buffer,
                   std::size_t end) {
    std::uint8_t checksum = 0;
    for (std::size_t i = 0; i < end; ++i)
      checksum += util::readBit(buffer, i);
    return checksum;
  }

  bool read(std::span<std::uint8_t const> const buffer,
            bool skipVerification) override {
    // Verify checksums.
    auto checksum = util::readBits<std::uint8_t, 8>(buffer, 119);
    if (checksum != generateChecksum(buffer, 119))
      return false;

    checksum = util::readBits<std::uint8_t>(buffer, 196);
    if (checksum != generateChecksum(buffer, 196))
      return false;

    // Verify signature.
    for (std::size_t i = 0; i < 8; i++) {
      auto const v = buffer[i] ^ buffer[8 + i];
      if (v != buffer[16 + i])
        return false;
    }

    // Read data.
    m_ID = util::readBits<std::uint8_t>(buffer, 32);

    for (std::size_t i = 0; i < NUM_Z_ITEMS; ++i)
      m_ZItems[i] = util::readBits<std::uint32_t, 9>(buffer, 40 + (9 * i));

    m_Unknown1 = util::readBits<std::uint32_t, 4>(buffer, 112);
    m_Unknown2 = util::readBit(buffer, 116);
    return true;
  }

  void write(std::span<std::uint8_t> buffer) override {
    // Write data.
    util::writeBits<std::uint8_t>(buffer, 32, m_ID);

    for (std::size_t i = 0; i < NUM_Z_ITEMS; ++i)
      util::writeBits<std::uint32_t, 9>(buffer, 40 + (9 * i), m_ZItems[i]);

    util::writeBits<std::uint32_t, 4>(buffer, 112, m_Unknown1);
    util::writeBit(buffer, 116, m_Unknown2);

    // Write first checksum.
    util::writeBits<std::uint8_t>(buffer, 119, generateChecksum(buffer, 119));

    // Write signature.
    for (std::size_t i = 0; i < 8; i++)
      buffer[16 + i] = buffer[i] ^ buffer[8 + i];

    // Write second checksum.
    util::writeBits<std::uint8_t>(buffer, 196, generateChecksum(buffer, 196));
  }

public:
  CharacterData() : m_ID(0), m_Unknown1(0), m_Unknown2(false) {
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

  std::uint32_t getUnknown1() const { return m_Unknown1; }
  bool getUnknown2() const { return m_Unknown2; }
};

} // namespace sparking::meteor

#endif /* _SPARKINGPW_METEOR_H */