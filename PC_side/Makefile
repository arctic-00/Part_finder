all: ./output/Part_Finder_Linux ./output/GPIO_Controller

./output/GPIO_Controller: ./objects/GPIO_Controller.o ./objects/Arduino_Communication.o
	gcc -Wall -o ./output/GPIO_Controller ./objects/GPIO_Controller.o ./objects/Arduino_Communication.o

./output/Part_Finder_Linux: ./objects/Part_Finder_Linux.o ./objects/Arduino_Communication.o
	gcc -Wall -o ./output/Part_Finder_Linux ./objects/Part_Finder_Linux.o ./objects/Arduino_Communication.o


./objects/Part_Finder_Linux.o: Part_Finder_Linux.c Arduino_Communication.h Arduino_Comm_Constants.h
	gcc -c -g -Wall -o ./objects/Part_Finder_Linux.o Part_Finder_Linux.c

./objects/GPIO_Controller.o: GPIO_Controller.c Arduino_Communication.h Arduino_Comm_Constants.h
	gcc -c -g -Wall -o ./objects/GPIO_Controller.o GPIO_Controller.c

./objects/Arduino_Communication.o: Arduino_Communication.c Arduino_Communication.h Arduino_Comm_Constants.h
	gcc -c -g -Wall -o ./objects/Arduino_Communication.o Arduino_Communication.c
