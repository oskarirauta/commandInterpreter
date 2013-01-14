CC=g++
LDFLAGS = -lm
CXXFLAGS = -c -Wall -ansi -g
OBJECTS = objdir/wiringSerial.o objdir/RS485.o objdir/commandInterpreter.o objdir/master.o

all: master

objdir:
	mkdir objdir

master: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o master

objdir/master.o: objdir master.cpp
	$(CC) $(CXXFLAGS) master.cpp -o objdir/master.o

objdir/wiringSerial.o: objdir include/wiringSerial.c
	$(CC) $(CXXFLAGS) include/wiringSerial.c -o objdir/wiringSerial.o

objdir/RS485.o: objdir include/RS485.cpp
	$(CC) $(CXXFLAGS) include/RS485.cpp -o objdir/RS485.o

objdir/commandInterpreter.o: objdir ../../../commandInterpreter.cpp
	$(CC) $(CXXFLAGS) ../../../commandInterpreter.cpp -o objdir/commandInterpreter.o

clean:
	rm -rf objdir master
