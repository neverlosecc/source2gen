# Source2Gen

Source2Gen is a tool to generate Source 2 SDKs. \
This project aims to provide an easy way to generate SDKs for various Source 2 games. \
An example of such SDKs could be found at [source2sdk](https://github.com/neverlosecc/source2sdk/tree/cs2).

---

## Usage

Open the source2gen-loader binary, it will automatically(if supported) find the game path,
and it will also set up the needed environment

### Windows

```commandline
source2gen-loader.exe
```

### Linux

```sh
./scripts/run.sh "$HOME/.steam/steam/steamapps/cs2/"
cp -r ./sdk-static/* ./sdk
# view generated sdk
ls ./sdk
```

You can also invoke source2gen directly , e.g. for debugging, by running

```sh
LD_LIBRARY_PATH=$HOME/.steam/steam/steamapps/cs2/game/bin/linuxsteamrt64/:$HOME/.steam/steam/steamapps/cs2/game/csgo/bin/linuxsteamrt64/ \
  source2gen
```

Source2Gen for Linux support is currently in an alpha state. There will be
errors, bugs, and wrong output. Please only file issues if you want to work on
them. This note will be removed once we have thoroughly tested Source2Gen on
Linux.

### Using the generated SDK

The sdk depends on a file/module called "source2gen_user_types". This file has
to be provided by the user and expose all types listed in
[source2gen_user_types.hpp](sdk-static/source2gen_user_types.hpp). If you don't
intend to access any of these types, you can use the dummy file
[source2gen_user_types.hpp](sdk-static/source2gen_user_types.hpp).

## Limitations

### Disabled entities

Under the following conditions, entities are either entirely omitted, or emitted
as a comment and replaced with a dummy:

- Overlapping fields: Fields that share memory with another field
- Misaligned fields: Fields that cannot be placed at the correct in-class offset
  because of their type's alignment requirements
- Misaligned types: Class types that would exceed their correct size because
  padding bytes would have to be inserted to meet alignment requirements
- Fields with template types

Some of these disabled entities can be made to work by using compiler-specific
attributes.

## Getting Started with Development

These instructions will help you set up the project on your local machine for development and testing purposes.

### Prerequisites

#### Windows

- Visual Studio 2019 or newer
- CMake

#### Linux

- g++-13 or newer
- CMake

### Clone the repository

To clone the repository with submodules, run the following command:

```bash
git clone --recurse-submodules https://github.com/neverlosecc/source2gen.git
```

### Update the Game Definition

Before building the project in Visual Studio, you will need to update the game definition.
The default definition is `CS2`. \
Possible options are: `CS2`, `SBOX`, `ARTIFACT2`, `ARTIFACT1`, `DOTA2`, `UNDERLORDS`, `DESKJOB`, `DEADLOCK`.

When using CMake, you can set `cmake -DSOURCE2GEN_GAME=CS2`

### Building the project

#### With CMake

- Open a command prompt or terminal in the project's root directory.
- Run the following sequence of commands to build the project:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSOURCE2GEN_GAME=CS2
cmake --build build
```

---

## Credits

This project is made possible by the contributions of various individuals and projects. Special thanks to the following:

- **[es3n1n](https://github.com/es3n1n)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=es3n1n)
- **[cpz](https://github.com/cpz)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=cpz)
- **[Cre3per](https://github.com/Cre3per/)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=cre3per), Linux support
- **[Soufiw](https://github.com/Soufiw)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=Soufiw)
- **[anarh1st47](https://github.com/anarh1st47)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=anarh1st47)
- **[praydog](https://github.com/praydog)** - the author of the original [Source2Gen](https://github.com/praydog/Source2Gen) project

This project also utilizes the following open-source libraries/tools:

- **[CMake](https://github.com/Kitware/CMake)** - Build tool
- **[ValveFileVDF](https://github.com/TinyTinni/ValveFileVDF)** - VDF file parser

If you've contributed to the project and would like to be listed here, please submit a [pull request](https://github.com/neverlosecc/source2gen/pulls) with your information added to the credits.
