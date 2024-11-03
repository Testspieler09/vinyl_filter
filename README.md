# Audio to Vinyl

 Add a vinyl sound to audio files. Currently the program only supports WAV files.

> [!IMPORTANT]
> The script will delete any data between the `bits_per_sample` part of the WAV and the `data sub-chunk`!


## Installation

1. Please compile the program with the `make` command in the root directory.
2. The program should now be in the `build` directory with the name `output`
3. If you want rename and move the program to the desired location to work with it.

> [!CAUTION]
> The makefile does not recognize when a header file changed.
> Just run `make clean` and then `make` again in the root folder of this project.


## Usage

> [!TIPP]
> The program already has some great default settings which you can change with the flags.

To see the help message run the program with the `-h` flag.

```txt
Usage: Audio to Vinyl [--help] [--version] [--samples VAR] [--bitDepth VAR] [--cracklingNoiseLvl VAR] [--generalNoiseLvl VAR] [--needleDropDuration VAR] [--needleLiftDuration VAR] Sourcepath Outputpath

Positional arguments:
  Sourcepath                  The path to the file(s) you want to convert. [required]
  Outputpath                  The path to the output folder [required]

Optional arguments:
  -h, --help                  shows help message and exits
  -v, --version               prints version information and exits
  -s, --samples               The number of samples you want in 1Hz [nargs=0..1] [default: 48000]
  -bD, --bitDepth             The bit depth you want in 1 Bit [nargs=0..1] [default: 24]
  -cNL, --cracklingNoiseLvl   The amount of crackling noise you want in 0.01% [nargs=0..1] [default: 100]
  -gNL, --generalNoiseLvl     The amount of white noise you want in 0.001% [nargs=0..1] [default: 5]
  -nDD, --needleDropDuration  The duration of the needle sound in 1s (at start of file) [nargs=0..1] [default: 0.8]
  -nLD, --needleLiftDuration  The duration of the needle sound in 1s (at end of file) [nargs=0..1] [default: 1]
```

The `filters.hpp` and `filters.cpp` file could be used as a library. However I would not recommend you doing so as they are not build for that purpose.


## Structure of this Repo

```txt
├── LICENSE
├── README.md
├── build                   // is created when you build with make
├── include                 // external libraries
│   └── argparse
│       └── argparse.hpp
├── makefile                // the makefile for compiling the program more easily
└── src                     // all other files the program needs to work
    ├── audio_to_vinyl.cpp  // the main file with argparse
    ├── build.ps1           // a Powershell script to build the program from the src directory
    ├── filehandler.cpp     // read / write the WAV file and output the WAVHeader
    ├── filehandler.hpp
    ├── filters.cpp         // apply some filters to make it sound more like vinyl
    ├── filters.hpp
    └── run.ps1             // a Powershell script to run the program form the src directory
```

## Contributing

If you want to contribute open an issue with a new feature or problem. Or open a pull request with the changes you made.
