# To produce executable, run:
#	make all


OBJS = CityBuilder.o TerrainGrid.o
CC = g++
DEBUG = -g
LIBS = -lGL -lglut -lGLU
CFLAGS = -c $(DEBUG)
LFLAGS = $(DEBUG)

all: CityBuilder

CityBuilder: $(OBJS)
	@echo 'Building $@'
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o CityBuilder

CityBuilder.o: src/CityBuilder.cpp src/TerrainGrid.cpp src/TerrainGrid.h src/cube.h
	$(CC) $(CFLAGS) src/CityBuilder.cpp $(LIBS)
		
TerrainGrid.o: src/TerrainGrid.cpp src/TerrainGrid.h src/VECTOR3D.h
	$(CC) $(CFLAGS) src/TerrainGrid.cpp $(LIBS)
	
			
clean:
	rm *.o *~ CityBuilder 2>/dev/null
