#ifndef _SPARKINGPW_UTIL_H
#define _SPARKINGPW_UTIL_H

#include <algorithm>
#include <cstdint>
#include <span>
#include <string_view>

namespace sparking::util {
class MTRNG {
protected:
  constexpr static std::size_t TWISTER_STATE_SIZE = 624;

  std::uint32_t m_State[TWISTER_STATE_SIZE];
  std::size_t m_Index;

public:
  MTRNG() : m_Index(0) {
    std::fill(m_State, m_State + TWISTER_STATE_SIZE, 0);
    m_State[0] = 0x12BD6AA;
    m_Index = 1;

    for (std::size_t i = 1; i < TWISTER_STATE_SIZE; ++i) {
      m_State[i] = (0x6C078965 * (m_State[i - 1] ^ (m_State[i - 1] >> 30)) + i);
      m_State[i] &= 0xFFFFFFFF;
    }
  }

  void seed(std::span<std::uint32_t const> const seedData) {
    std::size_t stateIdx = 1, seedIdx = 0;

    for (std::size_t _ = 0; _ < std::max(TWISTER_STATE_SIZE, seedData.size());
         ++_) {
      auto const last = m_State[stateIdx - 1];
      m_State[stateIdx] =
          (m_State[stateIdx] ^ ((last ^ (last >> 30)) * 0x19660D)) +
          seedData[seedIdx] + seedIdx;

      if (++stateIdx >= TWISTER_STATE_SIZE) {
        stateIdx = 1;
        m_State[0] = m_State[TWISTER_STATE_SIZE - 1];
      }

      if (++seedIdx >= seedData.size())
        seedIdx = 0;
    }

    for (std::size_t _ = 0; _ < TWISTER_STATE_SIZE - 1; ++_) {
      auto const last = m_State[stateIdx - 1];
      m_State[stateIdx] =
          (m_State[stateIdx] ^ (0x5D588B65 * (last ^ (last >> 30)))) - stateIdx;

      if (++stateIdx >= TWISTER_STATE_SIZE) {
        stateIdx = 1;
        m_State[0] = m_State[TWISTER_STATE_SIZE - 1];
      }
    }
  }

  std::uint32_t generate() {
    auto v = m_State[m_Index];

    v ^= (v >> 11);
    v ^= ((v << 7) & 0x9D2C5680);
    v ^= ((v << 15) & 0xEFC60000);
    v ^= (v >> 18);

    m_Index = (m_Index + 1) % TWISTER_STATE_SIZE;
    return v;
  }
};

inline std::uint64_t readBit(std::span<std::uint8_t const> const buffer,
                             std::size_t index) {
  auto const delta = index % 8;
  auto const idx = (index - delta) / 8;

  if (idx < buffer.size())
    return (buffer[idx] >> delta) & 1;

  return 0;
}

inline void writeBit(std::span<std::uint8_t> buffer, std::size_t index,
                     std::uint32_t bit) {
  auto const delta = index % 8;
  auto const idx = (index - delta) / 8;

  if (idx < buffer.size()) {
    if (bit)
      buffer[idx] |= (1 << delta);
    else
      buffer[idx] &= ~(1 << delta);
  }
}

inline std::uint64_t readBits(std::span<std::uint8_t const> const buffer,
                              std::size_t offset, std::size_t size) {
  std::uint64_t x = 0;
  for (std::size_t i = 0; i < size; i++)
    x |= readBit(buffer, offset + i) << i;

  return x;
}

inline void writeBits(std::span<std::uint8_t> buffer, std::size_t offset,
                      std::size_t size, std::uint64_t value) {
  for (std::size_t i = 0; i < size; i++)
    writeBit(buffer, offset + i, value >> i & 1);
}

inline std::uint32_t pwChecksum(std::span<std::uint8_t const> const buffer) {
  std::uint32_t checksum = 0;
  for (auto i = 0u; i < 119; ++i)
    checksum += readBit(buffer, i);
  return checksum;
}

inline void pwCipher(std::span<std::uint8_t> buffer) {
  MTRNG rng;

  auto const seed = readBits(buffer, 0, 32);
  std::uint32_t const seedData[] = {seed, seed * 2, seed * 3, seed * 4};
  rng.seed(seedData);

  for (std::size_t i = 4; i < buffer.size(); ++i)
    buffer[i] ^= rng.generate();
}

inline void pwEncode(std::string &out, std::span<std::uint8_t const> const in,
                     std::string_view const dict) {
  std::size_t outIdx = 0;

  if ((!(in.size() % 3)) && out.size() >= (in.size() * 4 / 3)) {
    for (std::size_t i = 0; i < in.size(); i += 3) {
      auto const b0 = in[i];
      auto const b1 = in[i + 1];
      auto const b2 = in[i + 2];

      out[outIdx] = dict[b0 & 0x3F];
      out[outIdx + 1] = dict[((b0 >> 6) & 0x03) | ((b1 & 0x0F) << 2)];
      out[outIdx + 2] = dict[((b1 >> 4) & 0x0F) | (b2 & 0x03) << 4];
      out[outIdx + 3] = dict[b2 >> 2];
      outIdx += 4;
    }
  }
}

inline void pwDecode(std::span<std::uint8_t> out, std::string_view const in,
                     std::span<std::size_t const> const table) {
  std::size_t outIdx = 0;

  if ((!(in.size() % 4)) && out.size() >= (in.size() * 3 / 4)) {
    for (std::size_t i = 0; i < in.size(); i += 4) {
      auto const chunk0 = table[in[i] & 0x7F];
      auto const chunk1 = table[in[i + 1] & 0x7F];
      auto const chunk2 = table[in[i + 2] & 0x7F];
      auto const chunk3 = table[in[i + 3] & 0x7F];

      out[outIdx] = chunk0 | (chunk1 << 6);
      out[outIdx + 1] = (chunk1 >> 2) | (chunk2 << 4);
      out[outIdx + 2] = (chunk2 >> 4) | (chunk3 << 2);
      outIdx += 3;
    }
  }
}

} // namespace sparking::util

#endif /* _SPARKINGPW_UTIL_H */