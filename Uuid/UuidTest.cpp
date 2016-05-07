// Copyright (C) 2016 by Chris Drew
//
#include "Uuid.h"

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <chrono>

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
  assert(h == 1768495365 && "Hash of zero Uuid not zero");
  std::cout << "PASS: testHash\n";
}

template<> void testHash<8>() {
  Uuid uuid = { 0 };
  using std::hash;
  auto h = hash<Uuid>{}(uuid);
  assert(h == 0x88201fb960ff6465 && "Hash of zero Uuid not zero");
  std::cout << "PASS: testHash\n";
}

void testGenerate() {
  Uuid uuid = newUuid();
  assert(uuid != Uuid{ 0 } && "Expecting new UUID to be non-zero");
  std::cout << "PASS: testGenerate\n";
}

void testGenerateUsingGenerator() {
  auto uuid_generator = urn::RandomUuidGenerator();
  auto uuid = uuid_generator();
  assert(uuid != Uuid{ 0 } && "Expecting new UUID to be non-zero");
  std::cout << "PASS: testGenerate\n";
}

auto expected = Uuid{ 0x69, 0x53, 0x8a, 0x3f, 0xc0, 0x7a, 0x4b, 0xe1, 0x87, 0x05, 0xfc, 0xc2, 0x01, 0xbd, 0x67, 0x3b };

void testConvertCString() {
  auto string = "69538a3f-c07a-4be1-8705-fcc201bd673b";
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertCString\n";
}

void testConvertString() {
  using namespace std::string_literals;
  auto string = "69538a3f-c07a-4be1-8705-fcc201bd673b"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertString\n";
}

void testConvertStringNoDash() {
  using namespace std::string_literals;
  auto string = "69538a3fc07a4be18705fcc201bd673b"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertStringNoDash\n";
}

void testConvertStringBraced() {
  using namespace std::string_literals;
  auto string = "{69538a3f-c07a-4be1-8705-fcc201bd673b}"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertStringBraced\n";
}

void testConvertStringBracedNoDash() {
  using namespace std::string_literals;
  auto string = "{69538a3fc07a4be18705fcc201bd673b}"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertStringBracedNoDash\n";
}

void testConvertStringWithPrefix() {
  using namespace std::string_literals;
  auto string = "urn:uuid:69538a3f-c07a-4be1-8705-fcc201bd673b"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertStringWithPrefix\n";
}

void testConvertWString() {
  using namespace std::string_literals;
  auto string = L"69538a3f-c07a-4be1-8705-fcc201bd673b"s;
  auto uuid = to_uuid(string);
  assert(uuid == expected);
  std::cout << "PASS: testConvertWString\n";
}

void testGenerateAndHash() {
  std::unordered_set<Uuid> generated;

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i != 10'000'000; ++i) {
    auto uuid = newUuid();
    auto insert_result = generated.insert(uuid);
    assert(insert_result.second);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> dur = end - start;
  std::cout << "PASS: testGenerateAndHash in " << dur.count() << "\n";
}

void runPerformanceTests() {
  std::cout << "Press any key to start performance tests...\n";
  std::cin.ignore();
  testGenerateAndHash();
}

int main(int argc, char* argv[]) {
  testThatZeroUuidIsZero();
  testThatTwoZeroUuidsAreEqual();
  testNonEquality();
  testHash<sizeof(std::size_t)>();
  testGenerate();
  testConvertCString();
  testConvertString();
  testConvertStringNoDash();
  testConvertStringBraced();
  testConvertStringWithPrefix();
  testConvertWString();

  if (argc > 1) {
    std::string option = argv[1];
    if (option == "-p" || option == "--performance") {
      runPerformanceTests();
    }
  }
}