build: ./src/hayai.c
	$(CC) ./src/hayai.c -o ./bin/hayai -Wall -Wextra -pedantic -std=c99

run: build
	./bin/hayai 

clean:
	rm ./bin/*
