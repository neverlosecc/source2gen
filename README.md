# Source2Gen

Source2Gen is a tool to generate Source 2 SDKs. \
This project aims to provide an easy way to generate SDKs for various Source 2 games. \
An example of such SDKs could be found at [source2sdk](https://github.com/neverlosecc/source2sdk/tree/cs2).

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

### Update the Game Definition

Before building the project in Visual Studio, you will need to update the game define in the file `include\sdk\interfaces\schemasystem\schema.h`. 
The current definition is `CS2`, which corresponds to the game that this project is used for. \
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

### Building the project

* Open a command prompt or terminal in the project's root directory.
* Run the following command to generate the Visual Studio solution:

```bash
premake5 vs2019 --game=CS2
```
* Open the generated source2gen.sln file in Visual Studio.
* Build the solution in the desired configuration (Debug, Release, or Dist).

---

## Credits

This project is made possible by the contributions of various individuals and projects. Special thanks to the following: 
- **[es3n1n](https://github.com/es3n1n)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=es3n1n)
- **[cpz](https://github.com/cpz)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=cpz)
- **[Soufiw](https://github.com/Soufiw)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=Soufiw)
- **[anarh1st47](https://github.com/anarh1st47)** - source2gen [contributor](https://github.com/neverlosecc/source2gen/commits?author=anarh1st47)
- **[praydog](https://github.com/praydog)** - the author of the original [Source2Gen](https://github.com/praydog/Source2Gen) project

This project also utilizes the following open-source libraries:
- **[Premake](https://github.com/premake/premake-core)** - Build configuration tool

If you've contributed to the project and would like to be listed here, please submit a [pull request](https://github.com/neverlosecc/source2gen/pulls) with your information added to the credits.
