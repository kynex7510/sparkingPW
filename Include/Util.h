#ifndef _SPARKINGPW_UTIL_H
#define _SPARKINGPW_UTIL_H

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <span>

namespace sparking::util {

template <typename T>
concept EnumType = std::is_enum_v<T>;

class MT {
protected:
  constexpr static std::size_t TWISTER_STATE_SIZE = 624;

  std::uint32_t m_State[TWISTER_STATE_SIZE];
  std::size_t m_Index;

public:
  MT() : m_Index(0) {
    std::fill(m_State, m_State + TWISTER_STATE_SIZE, 0);
    m_State[0] = 0x12BD6AA;
    m_Index = 1;

    for (std::size_t i = 1; i < TWISTER_STATE_SIZE; ++i) {
      m_State[i] = (0x6C078965 * (m_State[i - 1] ^ (m_State[i - 1] >> 30)) +
                    static_cast<std::uint32_t>(i));
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
          seedData[seedIdx] + static_cast<std::uint32_t>(seedIdx);

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
          (m_State[stateIdx] ^ (0x5D588B65 * (last ^ (last >> 30)))) -
          static_cast<std::uint32_t>(stateIdx);

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

inline bool readBit(std::span<std::uint8_t const> const buffer,
                    std::size_t offset) {
  auto const delta = offset % 8;
  auto const idx = (offset - delta) / 8;

  if (idx < buffer.size())
    return (buffer[idx] >> delta) & 1;

  return 0;
}

inline void writeBit(std::span<std::uint8_t> buffer, std::size_t offset,
                     bool bit) {
  auto const delta = offset % 8;
  auto const idx = (offset - delta) / 8;

  if (idx < buffer.size()) {
    if (bit)
      buffer[idx] |= (1 << delta);
    else
      buffer[idx] &= ~(1 << delta);
  }
}

template <std::integral T, std::size_t SIZE = sizeof(T) * 8>
inline T readBits(std::span<std::uint8_t const> const buffer,
                  std::size_t offset) {
  T x = 0;

  static_assert(SIZE <= sizeof(T) * 8, "Destination type is too small!");

  for (std::size_t i = 0; i < SIZE; i++) {
    if (readBit(buffer, offset + i))
      x |= (1 << i);
  }

  return x;
}

template <EnumType T, std::size_t SIZE = sizeof(T) * 8>
inline T readBits(std::span<std::uint8_t const> const buffer,
                  std::size_t offset) {
  return static_cast<T>(readBits<std::uint32_t, SIZE>(buffer, offset));
}

template <std::integral T, std::size_t SIZE = sizeof(T) * 8>
inline void writeBits(std::span<std::uint8_t> buffer, std::size_t offset,
                      T value) {
  static_assert(SIZE <= sizeof(T) * 8, "Source type is too small!");

  for (std::size_t i = 0; i < SIZE; i++)
    writeBit(buffer, offset + i, value >> i & 1);
}

template <EnumType T, std::size_t SIZE = sizeof(T) * 8>
inline void writeBits(std::span<std::uint8_t> buffer, std::size_t offset,
                      T value) {
  writeBits<std::uint32_t, SIZE>(buffer, offset,
                                 static_cast<std::uint32_t>(value));
}

inline void cipher(std::span<std::uint8_t> buffer, std::size_t offset,
                   std::uint32_t seed) {
  MT rng;
  std::uint32_t const seedData[] = {seed, seed * 2, seed * 3, seed * 4};
  rng.seed(seedData);

  for (std::size_t i = offset; i < buffer.size(); ++i)
    buffer[i] ^= rng.generate();
}

} // namespace sparking::util

#endif /* _SPARKINGPW_UTIL_H */