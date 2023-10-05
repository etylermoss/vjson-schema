# vjson-schema (Validate JSON Schema)

Validate JSON file(s) against a JSON schema. See `vjson-schema --help` for more information.

## Usage

```txt
$> vjson-schema --help

Usage: vjson-schema [OPTION...] SCHEMA [TARGET...]
Validate JSON file(s) against a JSON schema.

  SCHEMA                     Path to JSON schema
  TARGET                     Paths to validate against schema

 OPTIONS:
  -r, --recurse[=DEPTH]      Recurse to depth
  -v, --verbose              Verbose output

  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <eden@etylermoss.com>.
```

## Installation

* ### Archlinux

  * Use the AUR package [vjson-schema](https://aur.archlinux.org/packages/vjson-schema)

* ### Linux (All)

  * Follow the instructions for [building from source](#instructions)
  * ```shell
    # ... already built from source
    $> sudo cmake --install build/
    ```

## Building 

### Dependencies

* [nlohmann/json](https://github.com/nlohmann/json) >= 3.11.2
* [pboettch/json-schema-validator](https://github.com/pboettch/json-schema-validator) >= 2.2.0

### Instructions

```shell
$> git clone https://github.com/etylermoss/vjson-schema.git && cd vjson-schema/
$> cmake -DCMAKE_BUILD_TYPE=Release -B build/
$> cmake --build build/ -- -j 6
$> ./build/vjson-schema --help # done!
```