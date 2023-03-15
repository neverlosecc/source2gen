# NEVERLOSE :: Source2Gen

NEVERLOSE Source2Gen is a tool to generate Source 2 SDKs. This project aims to provide an easy way to generate SDKs for various Source 2 games.

---

## Getting Started

These instructions will help you set up the project on your local machine for development and testing purposes.

### Prerequisites

- Visual Studio 2019 or newer
- premake5

### Clone the repository

To clone the repository with submodules, run the following command:

```bash
git clone --recurse-submodules https://github.com/neverlosecc/source2gen.git
```
### Building the project
* Open a command prompt or terminal in the project's root directory.
* Run the following command to generate the Visual Studio solution:

```bash
premake5 vs2019
```
* Open the generated source2gen.sln file in Visual Studio.
* Build the solution in the desired configuration (Debug, Release, or Dist).

---
## Credits

This project is made possible by the contributions of various individuals and projects. Special thanks to the following:

- **[praydog](https://github.com/praydog)** - Original Source2Gen project

This project also utilizes the following open-source libraries:

- **[fmt](https://github.com/fmtlib/fmt)** - Modern C++ formatting library
- **[Premake](https://github.com/premake/premake-core)** - Build configuration tool

If you've contributed to the project and would like to be listed here, please submit a pull request with your information added to the credits.
