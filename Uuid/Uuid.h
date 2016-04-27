#pragma once

#include <array>
#include <random>

namespace urn {
  using Uuid = std::array<uint8_t, 16>;
}

namespace details {
  constexpr std::size_t SEED_LENGTH = 8;

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

  template<int> std::size_t hashImpl(const urn::Uuid& value);

  template<> std::size_t hashImpl<4>(const urn::Uuid& value) {
    // FNV-1a hash for 32bit size_t
    // http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1a
    std::size_t hash = 2166136261;
    constexpr std::size_t p = 16777619;
    for (auto byte : value)
      hash = (hash ^ byte) * p;
    return hash;
  }

  template<> size_t hashImpl<8>(const urn::Uuid& value) {
    // FNV-1a hash for 64bit size_t
    // http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1a
    std::size_t hash = 14695981039346656037;
    constexpr std::size_t p = 1099511628211;
    for (auto byte : value)
      hash = (hash ^ byte) * p;
    return hash;
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
}  // namespace urn

namespace std {
  template <>
  struct hash<urn::Uuid> {
    std::size_t operator()(const urn::Uuid& value) const {
      return details::hashImpl<sizeof(std::size_t)>(value);
    }
  };
}

