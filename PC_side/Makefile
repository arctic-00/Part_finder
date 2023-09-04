
./output/GPIO_Controller: ./objects/GPIO_Controller.o ./objects/Arduino_Communication.o
	gcc -o ./output/GPIO_Controller ./objects/GPIO_Controller.o ./objects/Arduino_Communication.o

./output/Part_Finder_Linux: ./objects/Part_Finder_Linux.o ./objects/Arduino_Communication.o
	gcc -o ./output/Part_Finder_Linux ./objects/Part_Finder_Linux.o ./objects/Arduino_Communication.o

./objects/Part_Finder_Linux.o: Part_Finder_Linux.c Arduino_Communication.h
	gcc -c -g -o ./objects/Part_Finder_Linux.o Part_Finder_Linux.c

./objects/GPIO_Controller.o: GPIO_Controller.c Arduino_Communication.h
	gcc -c -g -o ./objects/GPIO_Controller.o GPIO_Controller.c

./objects/Arduino_Communication.o: Arduino_Communication.c Arduino_Communication.h
	gcc -c -g -o ./objects/Arduino_Communication.o Arduino_Communication.c