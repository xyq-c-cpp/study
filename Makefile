TARGET  := websvr
CC      := g++
OBJ     := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(wildcard *.cpp)))
CFLAGS  := -g -Wall -O3
CXXFLAGS := -std=c++11 -pthread $(CFLAGS)

$(TARGET) : $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $(OBJ)    
    
all:$(TARGET)    

.PHONY: clean    
clean :
	rm -rf *.o $(TARGET)




    


