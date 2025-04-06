# Source2Gen

Source2Gen is a tool designed to generate SDKs for Source 2 games.

Example SDKs generated with this tool can be found at [neverlosecc/source2sdk](https://github.com/neverlosecc/source2sdk/tree/cs2).

---

## Features

- Extracts SDK definitions from Source 2 binaries with minimal effort
- Supports multiple Source 2 games (full list below)
- Emits **C++23**, **C23**, or **IDA-compatible C** headers
- Cross-platform: Windows and Linux

---

## Usage

### Windows

Launch `source2gen-loader.exe`:

```commandline
source2gen-loader.exe
```

The loader will attempt to automatically detect the game path.  
To specify the game directory manually:

```commandline
source2gen-loader.exe --game-path "C:\\Games\\CS2"
```

Additional arguments can be passed directly to source2gen via the loader, for example:

```commandline
source2gen-loader.exe --game-path "C:\\Games\\CS2" --emit-language c
```

---

### Linux (alpha-quality support)

Run the provided wrapper script:

```bash
./scripts/run.sh "$HOME/.steam/steam/steamapps/cs2/" [options]
```

Generated SDKs will appear under the `./sdk` directory.

For manual or debugging use, invoke directly:

```bash
LD_LIBRARY_PATH=$HOME/.steam/steam/steamapps/cs2/game/bin/linuxsteamrt64/:$HOME/.steam/steam/steamapps/cs2/game/csgo/bin/linuxsteamrt64/ \
  ./build/source2gen
```

**Note:** Linux support is experimental. Expect issues, incomplete output, or errors. Contributions to improve Linux support are encouraged.

---

## Using the Generated SDK

By default, the SDK contains dummy implementations for certain types, located in [sdk-static](sdk-static/cpp/include/source2sdk/source2gen/source2gen.hpp)

It is recommended to replace these dummy implementations with actual implementations specific to your needs.

---

## Output languages (`--emit-language`)

| Language   | Minimum Language Standard        |
|------------|----------------------------------|
| `cpp`      | C++23                            |
| `c`        | C23                              |
| `c-ida`    | C (single file: `sdk/ida.h`)     |

---

### Using IDA-compatible output (`c-ida`)

1. In IDA, navigate to **File -> Load File -> Parse C Header File...**
2. Select `sdk/ida.h`
3. Wait for compilation to complete (this may take some time)

All defined types will then be available under **Local Types** in IDA.

> [!NOTE]  
> The IDA support was tested on IDA Pro 9.1, all the "older" versions of IDA might not proceed the output well, feel free to open the issues in case you run into any issues though.

---

## Limitations

Some entities are omitted or replaced with dummy implementations in the generated SDK due to technical limitations:

- **Overlapping fields:** when multiple fields occupy the same memory location
- **Misaligned fields:** fields whose type alignment requirements prevent correct in-structure offset placement
- **Misaligned types:** class/struct types where padding would be needed beyond the original size to meet alignment rules
- **Entities using C++ templates**

Some of these limitations might be addressed in future versions using compiler-specific extensions.

---

## Development Setup

### Prerequisites

**Windows**

- Visual Studio 2019 or newer (build inside the "Developer Command Prompt")
- CMake
- Conan

**Linux**

- g++-13 or newer
- CMake
- Conan

---

### Clone repository

```bash
git clone --recurse-submodules https://github.com/neverlosecc/source2gen.git
```

---

### Selecting your game and building

Supported games:

`CS2` (default), `SBOX`, `ARTIFACT2`, `ARTIFACT1`, `DOTA2`, `UNDERLORDS`, `DESKJOB`, `DEADLOCK`, `HL_ALYX`, `THE_LAB_ROBOT_REPAIR`

Example build targeting DEADLOCK:

```bash
conan build -o "game=DEADLOCK" --build=missing .
```

Replace `DEADLOCK` with the chosen target.

---

### Running tests

Run unit tests (Linux):

```bash
LD_LIBRARY_PATH=$HOME/.steam/steam/steamapps/cs2/game/bin/linuxsteamrt64/:$HOME/.steam/steam/steamapps/cs2/game/csgo/bin/linuxsteamrt64/ \
  ./build/Release/bin/source2gen-test
```

Test whether the generated SDK compiles:

```bash
./scripts/test-cpp.sh ~/games/cs2/
```

---

## Internal Design

### C Generator

Implements the `codegen::IGenerator` interface, designed to be language-agnostic. Notable considerations:

- Namespaces and module information are encoded into identifiers to avoid conflicts
- Enumerator names include the enum name prefix to minimize name collisions
- Classes are emitted as `struct` in C/C++
- Uses of `struct`, `union`, `enum` types are prefixed with the "struct", "union", "enum" keyword respectively (see `codegen::TypeCategory`)
- Static fields are omitted for C output (the only currently known use case of C is to generate IDA headers)

---

### C-IDA Generator

Implemented as a wrapper on top of the C generator:

- Sets options `--no-static-assertions` and `--no-static-members`
- Performs postprocessing:
  - Merges all generated files into a single header (`sdk/ida.h`)
  - Removes system includes

---

## Credits

Special thanks to project contributors:

- [es3n1n](https://github.com/es3n1n)
- [cpz](https://github.com/cpz)
- [Cre3per](https://github.com/Cre3per) (Linux support)
- [Soufiw](https://github.com/Soufiw)
- [anarh1st47](https://github.com/anarh1st47)
- [praydog](https://github.com/praydog) (author of the original [Source2Gen](https://github.com/praydog/Source2Gen) tool)

If you have contributed and are not listed here, please submit a [pull request](https://github.com/neverlosecc/source2gen/pulls).

---

## Dependencies

- [Conan](https://github.com/conan-io/conan) - C and C++ package manager
- [CMake](https://github.com/Kitware/CMake) - Build system
- [ValveFileVDF](https://github.com/TinyTinni/ValveFileVDF) - VDF file parser
- [argparse](https://github.com/p-ranav/argparse) - C++ argument parser
- [Abseil](https://github.com/abseil/abseil-cpp) - Common C++ libraries from Google
- [GoogleTest](https://github.com/google/googletest) - Testing framework
