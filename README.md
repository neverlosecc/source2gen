TODO: remove these notes

Changes in this fork (ok = done, x = pending)

- x unix compatibility
  - ok compiles with clang
    - ok alias/redirect windows APIs (`LoadLibraryA()` and friends)
    - ok add missing includes
    - ok fix source file case mismatches
  - ok launches without crashing
  - ok dumps
  - x compatible with unix AND windows (compile-time changes)
- x source2gen is an executable (faster, easier to use, supports headless systems)
  - ok is an executable
    - ok cmake
    - x premake
    - ok `DLLMain` -> `main`
  - ok source2gen loads required game libraries
    - ok remove sleeps and threads
  - x command line flags (e.g. output directory)
  - ok initializes libraries into a "dumpable" state
    - ok (not needed) Call `CSchemaSystem::Connect()`?
      - ok Who calls this in production? `CAppSystemDict::LoadSystemAndDependencies()` -> virtual call to `CSchemaSystem::Connect()`
      - ok What factory? -> `AppSystemDictCreateInterfaceFn`
      - manual call accesses `s_pAppSystemDict`, which is a `nullptr` -> crash
        - `s_pAppSystemDict` is initialized via
          - `Source2MainInternal()`
          - -> `CMaterialSystem2AppSystemDict::CMaterialSystem2AppSystemDict()`
          - -> `CTier2AppSystemDict::CTier2AppSystemDict()`
          - -> `CAppSystemDict::CAppSystemDict()`
        - all of these functions have tons of parameters
      - Better let some other library call this? that'd be `Source2Main()`. if we call that, we might as well run cs2, which misses the point of this fork.
    - ok Call `InstallSchemaBindings()`

Notes and questions for the upstream maintainers

- is your compiler case-insensitive?
  - includes
  - function names
- thanks for .clang-format
- thanks for `static_assert`s of struct sizes
  - might need some work for 32 bit and 64 bit support
- what's going on with the includes? I added some, might want to add an analyzer
  to be friendlier to compilers and noisy linters

# Source2Gen

Source2Gen is a tool to generate Source 2 SDKs. \
This project aims to provide an easy way to generate SDKs for various Source 2 games. \
An example of such SDKs could be found at [source2sdk](https://github.com/neverlosecc/source2sdk/tree/cs2).

---

## Usage

### Windows

TODO: how to run on windows?

```sh
source2gen <C:\\path\\to\\cs2>
```

### Unix

```sh
LD_LIBRARY_PATH=$HOME/.steam/steam/steamapps/cs2/game/bin/linuxsteamrt64/:$HOME/.steam/steam/steamapps/cs2/game/csgo/bin/linuxsteamrt64/ \
  source2gen ~/.steam/steam/steamapps/common/cs2
```

Source2Gen for Unix support is currently in an alpha state. There will be
errors, bugs, and wrong output. Please only file issues if you want to work on
them. This note will be removed once we have thoroughly tested Source2Gen on
Unix.

## Getting Started

These instructions will help you set up the project on your local machine for development and testing purposes.

### Prerequisites

#### Windows

- Visual Studio 2019 or newer
- premake5

#### Unix

- clang++-17 or newer, other compilers are might work as well
- CMake

### Clone the repository

To clone the repository with submodules, run the following command:

```bash
git clone --recurse-submodules https://github.com/neverlosecc/source2gen.git
```

### Update the Game Definition

Before building the project in Visual Studio, you will need to update the game definition.
The default definition is `CS2`. \
Possible options are: `CS2`, `SBOX`, `ARTIFACT2`, `ARTIFACT1`, `DOTA2`, `UNDERLORDS`, `DESKJOB`.

or

You can use premake5 options and specify which game you want to dump:

```bash
 --game=CS2          
 Choose a particular game for dumping source 2 sdk; one of:
     ARTIFACT1        Artifact Classic
     ARTIFACT2        Artifact Foundry
     CS2              Counter-Strike 2
     DESKJOB          Aperture Desk Job
     DOTA2            Dota 2
     HL_ALYX          Half-Life: Alyx
     SBOX             S&BOX
     THE_LAB_ROBOT_REPAIR Portal: Aperture Robot Repair
     UNDERLORDS       Dota Underlords
```

or

When using CMake, you can set `cmake -DSOURCE2GEN_GAME=CS2`

### Building the project

#### With premake5

- Open a command prompt or terminal in the project's root directory.
- Run the following command to generate the Visual Studio solution:

```bash
premake5 vs2019 --game=CS2
```

- Open the generated source2gen.sln file in Visual Studio.
- Build the solution in the desired configuration (Debug, Release, or Dist).

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
- **[Cre3per](https://github.com/Cre3per/)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=cre3per), Unix support
- **[Soufiw](https://github.com/Soufiw)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=Soufiw)
- **[anarh1st47](https://github.com/anarh1st47)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=anarh1st47)
- **[praydog](https://github.com/praydog)** - the author of the original [Source2Gen](https://github.com/praydog/Source2Gen) project

This project also utilizes the following open-source libraries:

- **[Premake](https://github.com/premake/premake-core)** - Build configuration tool
- **[CMake](https://github.com/Kitware/CMake)** - Build tool

If you've contributed to the project and would like to be listed here, please submit a [pull request](https://github.com/neverlosecc/source2gen/pulls) with your information added to the credits.
