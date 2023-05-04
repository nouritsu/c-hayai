build: ./src/hayai.c ./src/abuf.c
	$(CC) ./src/*.c -I "./inc" -o ./bin/hayai -Wall -Wextra -pedantic -std=c99

run: build
	./bin/hayai 

clean:
	rm ./bin/*
