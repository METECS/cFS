all: readtemp

clean:
	rm -f readtemp
	rm -f *.o

readtemp: readtemp.o
readtemp: i2cdriver.o
	gcc -ggdb -Wall -Wextra -pedantic -o readtemp readtemp.o i2cdriver.o

readtemp.o: readtemp.c
readtemp.o: ../../3rdparty/serbus/include/i2cdriver.h
	gcc -ggdb -Wall -Wextra -pedantic -c -I ~/cFS/3rdparty/serbus/include readtemp.c

i2cdriver.o: ../../3rdparty/serbus/src/i2cdriver.c
i2cdriver.o: ../../3rdparty/serbus/include/i2cdriver.h
	gcc -ggdb -Wall -Wextra -pedantic -c -I ~/cFS/3rdparty/serbus/include ../../3rdparty/serbus/src/i2cdriver.c
