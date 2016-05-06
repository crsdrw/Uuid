# urn-uuid

Provides an implementation of Universally Unique Identifier (UUID).
Generates version 4 UUIDs as specified in [RFC 4122](https://www.ietf.org/rfc/rfc4122.txt). 
Parse UUIDs from string. 
Provides hash so that UUID can be used in the standard library associative containers.
Easily convertible to boost uuid.

## Installing

Just copy [Uuid.h](Uuid.h) into your project.

## Usage

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
auto uuid = urn::to_uuid(uuid_string);
```


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
