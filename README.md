# Audio to Vinyl

 Add a vinyl sound to audio files. Currently the program only supports WAV files.

> [!IMPORTANT]
> The script will delete any metadata of the wav file(s) provided.


## Installation

1. Please compile the program with the `make` command in the root directory.
2. The program should now be in the `build` directory with the name `output`
3. If you want to rename and move it to the desired location to work with it.


## Usage

```txt
Enter the help message of output -h here
```


## Structure of this Repo

```txt
├── LICENSE
├── README.md
├── build                   // is created when you build with make
├── include                 // external librarys
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
