TARGET = bin/dbview 
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default 
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db -a "Daniele R.,rua arquitecto cottinelli telmo 44,120" -l
	./$(TARGET) -f ./mynewdb.db -a "Joyce Silva,Via Boh,200" -l

default: $(TARGET)

clean: 
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
	