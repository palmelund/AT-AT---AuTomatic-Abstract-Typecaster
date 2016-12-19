# AT-AT - AuTomatic Abstract Typecaster

The AT-AT is a sorting machine running on an embedded system and built in lego. The system combines a real-time system running on an Arduino Mega and Machine Intelligence running on an assisting computer.

The machine operates better under the tunes of the Imperial March.

## 5th semester project
This project was made as part of fifth semester of the bachelor in Software Engineering at Aalborg University where the project theme was Embedded software.

## Project structure
The project has the following overall structure to help organising files and source code.

* BmpSort
  * This is the computer side part of the project, that handles communication with the Kinect used as a camera, and makes shape recognition on the resulting image.
  * This part of the project is written in C#. Uses WPF for UI so a windows PC is needed right now.
* arduino
  * This is where all the source code for the Arduino is located, including libraries, API and the souce code
  * Written in C with a little bit of C++. Has full build script for Linux, but more manual work is needed if building on Windows.
* prototype
  * This is where the code for prototypes that was developed early in the project are located
  * This part is programmed in NXT-OSEK using C, LeJOS using Java and on the Arduino using C.
  * This code haven't been touched for a long time, and prototype 4 may not compile due to changes in the API.
* schematics
  * This is where schematics for the construction are located. There are currently two.
  * A wire schematic showing how the hardware is wired up. This can be opened using Fritzinf.
  * A digital model of the machine. This can be opened using LEGO Digital Designer.
  * A fault tree conducted from a fault tree analysis.
* test
  * This is where the code for unit testing, component testing and worst case execution time are located
  
## The Name
The machine was nameless for most of the project, but after every other student told us it looked like an AT-AT we finally gave in, named it AT-AT and printed a paper model of an AT-AT and taped onto it.
  
The name AuTomatic Abstract Typecaster was chosen because it represents what the project is:
* AuTomatic: The machine can operate on its own after it is started, only needing help to fill the feeder and empty the separation unit. These are both components that could also be automated away.
* Abstract: The machine itself can not be used in the industry, but it has many ideas that can be inherited and implemented by other machines to be used in practice.
* Typecaster: Since it is unknown what enters the machine, all is seen as objetcs. By passing though the machine it is being recognised and passed on as a shape, and in case of being a circle, it also has a color propery assigned to it.
