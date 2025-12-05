# for any kind of build change, just change the name of the file
# here tetris is the name of the source+output file

all:
	g++ -I src/include -L src/lib -o StackDemo StackDemo.cpp \
-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
