// Copyright (C) 2016 by Chris Drew

// Provides an implementation of Universally Unique Identifier (UUID). 
// Generates version 4 UUIDs as specified in RFC 4122. 
// Parse UUIDs from string. 
// Provides hash so that UUID can be used in the standard library associative containers.
// Easily convertible to boost uuid.

#pragma once

#include <array>
#include <random>
#include <string>
#include <algorithm>
#include <functional>
#include <cstring>
#include <cwchar>

namespace urn {
  using Uuid = std::array<uint8_t, 16>;
}

namespace details {
  const std::size_t SEED_LENGTH = 8;

  std::array<uint_fast32_t, SEED_LENGTH> generateSeedData() {
    std::array<uint_fast32_t, SEED_LENGTH> random_data;
    std::random_device random_source;
    std::generate(random_data.begin(), random_data.end(), std::ref(random_source));
    return random_data;
  }

  std::mt19937 createEngine() {
    auto random_data = generateSeedData();
    std::seed_seq seed_seq(random_data.begin(), random_data.end());
    return std::mt19937{ seed_seq };
  }

  void setVersion4(urn::Uuid& uuid) {
    uuid[6] &= 0x4F;
    uuid[6] |= 0x40;
  }

  void setVariant(urn::Uuid& uuid) {
    uuid[8] &= 0xBF;
    uuid[8] |= 0x80;
  }

  template<int> std::size_t hash(const urn::Uuid& value);

  template<> std::size_t hash<4>(const urn::Uuid& value) {
    // FNV-1a hash for 32bit size_t
    // http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1a
    uint32_t hash = 2166136261ul;
    static const uint32_t p = 16777619ul;
    for (auto byte : value)
      hash = (hash ^ byte) * p;
    return static_cast<std::size_t>(hash);
  }

  template<> std::size_t hash<8>(const urn::Uuid& value) {
    // FNV-1a hash for 64bit size_t
    // http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1a
    uint64_t hash = 14695981039346656037ull;
    static const uint64_t p = 1099511628211ull;
    for (auto byte : value)
      hash = (hash ^ byte) * p;
    return static_cast<std::size_t>(hash);
  }

  template<typename Char>
  struct HexChars;

  template<>
  struct HexChars<char> {
    static std::string lower() {
      return std::string{"0123456789abcdef"};
    }
    static const std::string upper() {
      return std::string{"ABCDEF"};
    }
  };

  template<>
  struct HexChars<wchar_t> {
    static std::wstring lower() {
      return std::wstring{ L"0123456789abcdef" };
    }
    static const std::wstring upper() {
      return std::wstring{ L"ABCDEF" };
    }
  };

  template<typename Char>
  uint8_t hexCharToInt(Char c) {
    const static auto lower_chars = HexChars<Char>::lower();

    auto find_lower = lower_chars.find(c);
    if (find_lower != std::string::npos)
      return static_cast<uint8_t>(find_lower);

    const static auto upper_chars = HexChars<Char>::upper();

    auto find_upper = upper_chars.find(c);
    if (find_upper != std::string::npos)
      return static_cast<uint8_t>(find_upper + 10);

    throw std::invalid_argument("Error parsing UUID string");
  }

  template<typename CharPtr>
  uint8_t toUuidByte(CharPtr& itr) {
    uint8_t byte = hexCharToInt(*itr++);
    byte <<= 4;
    byte |= hexCharToInt(*itr++);
    return byte;
  }

  template<typename CharPtr>
  void skipAnyDash(CharPtr& itr) {
    if (*itr == '-')
      ++itr;
  }

  const std::string urn_prefix = "urn:uuid:";
  const int urn_prefix_size = 9;

  template<typename StringItr>
  urn::Uuid toUuid(StringItr itr, StringItr end) {
    urn::Uuid uuid;

    if (std::distance(itr, end) < 32)
      throw std::invalid_argument("Error parsing UUID string");

    if (std::equal(itr, itr+urn_prefix_size, urn_prefix.begin(), urn_prefix.end()))
      itr += urn_prefix_size;

    if (*itr == '{')
      ++itr;

    uuid[0] = toUuidByte(itr);
    uuid[1] = toUuidByte(itr);
    uuid[2] = toUuidByte(itr);
    uuid[3] = toUuidByte(itr);
    skipAnyDash(itr);
    uuid[4] = toUuidByte(itr);
    uuid[5] = toUuidByte(itr);
    skipAnyDash(itr);
    uuid[6] = toUuidByte(itr);
    uuid[7] = toUuidByte(itr);
    skipAnyDash(itr);
    uuid[8] = toUuidByte(itr);
    uuid[9] = toUuidByte(itr);
    skipAnyDash(itr);

    if (std::distance(itr, end) < 12)
      throw std::invalid_argument("Error parsing UUID string");

    for (urn::Uuid::size_type i = 10; i != 16; ++i)
      uuid[i] = toUuidByte(itr);

    return uuid;
  }
}  // namespace details

namespace urn {
  class RandomUuidGenerator {
    std::mt19937 engine;
  public:
    RandomUuidGenerator() : engine(details::createEngine()) {}
    Uuid operator()() {
      Uuid uuid;
      auto itr = uuid.begin();

      for (int i = 0; i != 4; ++i) {
        auto random_value = engine();
        for (int j = 0; j != 4; ++j, ++itr)
          *itr = (random_value >> j * 8) & 0xFF;
      }

      details::setVariant(uuid);
      details::setVersion4(uuid);

      return uuid;
    }
  };

  Uuid newUuid() {
    thread_local RandomUuidGenerator generator;
    return generator();
  }

  Uuid to_uuid(const std::string& string) {
    return details::toUuid(string.begin(), string.end());
  }

  Uuid to_uuid(const char* string) {
    return details::toUuid(string, string + std::strlen(string));
  }

  Uuid to_uuid(const std::wstring& string) {
    return details::toUuid(string.begin(), string.end());
  }

  Uuid to_uuid(const wchar_t* string) {
    return details::toUuid(string, string + std::wcslen(string));
  }
}  // namespace urn

namespace std {
  template <>
  struct hash<urn::Uuid> {
    std::size_t operator()(const urn::Uuid& value) const {
      return details::hash<sizeof(std::size_t)>(value);
    }
  };
}

