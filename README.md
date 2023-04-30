# About
A simple text editor written in C.

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
