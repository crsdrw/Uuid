# urn-uuid

Provides an implementation of Universally Unique Identifier (UUID).
Generates version 4 UUIDs as specified in [RFC 4122](https://www.ietf.org/rfc/rfc4122.txt). 
Parse UUIDs from string. Convert UUIDs to string. 
Provides hash so that UUID can be used in the standard library associative containers.
Easily convertible to boost uuid.

## Installing

Just copy [Uuid.h](Uuid/Uuid.h) into your project. It requires a C++14 compiler.

## Usage

### Definition ###

`urn::Uuid` is defined simply as:
```
namespace urn {
  using Uuid = std::array<uint8_t, 16>;
}
```
so that it all the functionality of `std::array` 
like comparison and equality operators are available. 

### Generating random UUIDs

To generate a new UUID (version 4):

```
auto uuid = urn::newUuid();
```
`newUuid()` uses a thread local `RandomUuidGenerator`. You can also create your own generator:

```
auto uuid_generator = urn::RandomUuidGenerator{};
auto uuid = uuid_generator();
```

### Parsing UUIDs

To parse a UUID from a string

```
auto uuid_string = std::string{"69538a3f-c07a-4be1-8705-fcc201bd673b"};
auto uuid = urn::toUuid(uuid_string);
```

### Conversion to string

To convert a UUID to a string

```
auto uuid = urn::newUuid();
auto string = toString(uuid);
```

