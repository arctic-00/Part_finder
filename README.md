# Part Finder
A laser is pointed at the entered component


# Setup
Change relevant constants such as container size and recompile using 'make' command for PC side. Change the serial terminal to the one that comes up when selecting arduino board. Constants relating to reading database file can be found in Part_Finder_Linux.c.

Pin name aliases can be added/modified to GPIO_Pin_names.txt in the output folder. 
The component database can be modified by running Part_Finder_Linux and typing 'edit' or by opening 'Part_Finder.csv' directly.

When first uploading the arduino's '.ino' file, set FIRST_RUN to true. After it has uploaded, set it back to false and upload.

