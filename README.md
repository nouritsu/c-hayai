# **LOOKING FOR TESTERS**

# About
A simple text editor written in C.  
The name comes from the Japanese word 速い (hayai) which means fast or swift.  

# Usage

## Installation
Clone the repository.  
Run the following command (only tested on linux) in the master directory to compile the program -  
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
Rules `clean` and `run` also exist in the Makefile, I will not insult your intellegence by explaning what they do.

## Navigation
### Movement
Arrow keys, PageUp, PageDown, Home and End can be used to navigate files.  
* Arrow keys work as intended.  
* PageUp and PageDown go up/down one terminal height worth of lines.  
* Home and End go to start or end of line.

### Exit
Press Control + Q to exit.  
* SIGINT has been turned off, so Control + C WILL NOT make the program exit.  
* SIGTSTP has been turned off too, so program CANNOT be run as a background task.  

## I/O
### Opening a file
Run the following command to open a file -
```
./hayai <file-path>
```
The file will be edited, edits will only be saved if saved manually.  
Hayai will warn you if the file is "dirty" i.e. modified, but not saved when you try to exit.  

### Saving a file
Press Control + S to save a file.
* You will be prompted for a file name if you run hayai without providing a file name.  

## Searching
Press Control + F to enter search mode. Start typing your query once prompted.  
This mode can be exited by pressing either ENTER or ESCAPE.  
Arrow keys can be used to move to next or previous occurence.
* Hayai will move your cursor to the query if found
* The search is incremental, meaning Hayai will search for your query as you type it
* If not found, you can exit out of search mode to get your cursor back where it was

# Hacking the editor
Editing `hayai_constants.h` in the `inc` directory can change how Hayai does certain things.
|     Constant     	|                    Description                    	|                                                 Effect                                                 	|
|:----------------:	|:-------------------------------------------------:	|:------------------------------------------------------------------------------------------------------:	|
|   HAYAI_VERSION  	|               Version of the editor               	|                Changes what version is listed when you open hayai without an active file               	|
|  HAYAI_TAB_STOP  	|       Number of spaces every tab represents       	| Changes the number of spaces each tab represents when rendering text. Does not affect saving to files. 	|
| HAYAI_QUIT_TIMES 	| Number of exit inputs when exiting a dirty buffer 	|         Changes the number of times Control + Q must be pressed before exiting a dirty buffer.         	|

# Known Issues / Bugs
* Possible memory leaks. Program has not been extensively tested for them yet.

# Plans for the future (Unsorted)
* Adding file type detection
* Adding syntax highlighting for code files
* Line numbers
* Making the editor use a config file

