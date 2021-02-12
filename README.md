
# Source code for an ideal free distribution model in continuous time

This is the source code supporting a Comment (Netz et al. 2021), submitted to the _Proceeedings of the Royal Society: Series B_, on DiNuzzo and Griffen (2020).

The source code is for a reimplementation of the individual based model presented in DiNuzzo and Griffen (2020), in continuous time.

This readme describes the repository contents.

## Contact

Please contact Christoph Netz at c.f.g.netz@rug.nl for more information.

## Model documentation

- `Model_description.pdf` The model description for this model.

- `Model-notes_DiNuzzo&Griffen2020.pdf` A technical discussion of the DiNuzzo and Griffen model.

## Model code

All model code is the directory `IFD_activity/`.

- `settings/config.ini` The configuration file for this model.

- `cmd_line.h` A simple command line argument parser utility.

- `main.cpp` The main C++ file that parses command line arguments and the model configuration file.

- `parameter.cpp` and `parameter.h` Code for model parameters.

- `rnd.cpp`, `rnd.hpp`, and `rndutils.hpp` Code for random number generation.

- `simulation.hpp` Code for the simulation dynamics.

- `Source.cpp`  Code that runs the simulation.

All other files store local Visual Studio settings.
