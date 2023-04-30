# About
A simple text editor written in C.  
The name comes from the Japanese word 速い (hayai) which means fast or swift.  

# Usage

## Installation
Clone the repository.  
Run the following command (only tested on linux) to compile in the master directory -  
```
make build
```
Compiled binary is in bin directory
```
cd bin
```
Use the following command to start the editor
```
./hayai [file-path]
```
If file path is provided, editor will show contents of file.

## Navigation
### Movement
Arrow keys, PageUp, PageDown, Home and End can be used to navigate files.  
* Arrow keys work as intended.  
* PageUp and PageDown go up/down one terminal height worth of lines.  
* Home and End go to start or end of line.

### Exit
Press Control + Q to exit.  
SIGINT has been turned off, so Control + C WILL NOT make the program exit.  
SIGTSTP has been turned off too, so program cannot be run as a background task.  
