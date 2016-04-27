#include "Uuid.h"

#include <cassert>
#include <iostream>

using namespace urn;

void testThatZeroUuidIsZero() {
  Uuid uuid = { 0 };

  for (auto byte : uuid)
    assert(byte == 0 && "Expect all elements of zero Uuid to be zero");

  std::cout << "PASS: testThatZeroUuidIsZero\n";
}

void testThatTwoZeroUuidsAreEqual() {
  Uuid uuid1 = { 0 };
  Uuid uuid2 = { 0 };

  assert(uuid1 == uuid2 && "Expect two zero uuids to be equal");
  std::cout << "PASS: testThatTwoZeroUuidsAreEqual\n";
}

void testNonEquality() {
  Uuid uuid1 = { 0 };
  Uuid uuid2 = { 1 };

  assert(uuid1 != uuid2 && "Expect zero and non zero uuid to not be equal");
  std::cout << "PASS: testNonEquality\n";
}

template<std::size_t> void testHash();

template<> void testHash<4>() {
  Uuid uuid = {0};
  using std::hash;
  auto h = hash<Uuid>{}(uuid);
  assert(h == 0 && "Hash of zero Uuid not zero");
  std::cout << "PASS: testHash\n";
}

template<> void testHash<8>() {
  Uuid uuid = { 0 };
  using std::hash;
  auto h = hash<Uuid>{}(uuid);
  assert(h == 0x88201fb960ff6465 && "Hash of zero Uuid not zero");
  std::cout << "PASS: testNonEquality\n";
}

void testGenerate() {
  Uuid uuid = newUuid();
  assert(uuid != Uuid{ 0 } && "Expecting new UUID to be non-zero");
  std::cout << "PASS: testGenerate\n";
}

int main() {
  testThatZeroUuidIsZero();
  testThatTwoZeroUuidsAreEqual();
  testNonEquality();
  testHash<sizeof(std::size_t)>();
  testGenerate();
}