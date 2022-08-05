# Validate JSON Schema (vjson-schema)

C/C++ tool to verify a JSON file (or files) matches a given JSON Schema. Uses nlohmann/json-schema for validation. See `vjson-schema --help` for information on correct usage.

## Usage

```txt
$> vjson-schema --help

Usage: vjson-schema [OPTION...] SCHEMA [TARGET...]
C/C++ tool to verify that a JSON file (or files) matches a given JSON Schema. Uses nlohmann/json-schema for validation.

  SCHEMA                     Path to JSON schema
  TARGET                     Paths to validate against schema

 OPTIONS:
  -d, --depth=INT            Maximum depth to search

  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <eden@etylermoss.com>.
```

## Building 

### Dependencies

* json-schema-validator >= 2.2.0