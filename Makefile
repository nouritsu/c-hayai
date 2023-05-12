build: ./src/*.c ./inc
	$(CC) ./src/*.c -I ./inc -o ./bin/hayai_debug -Wall -Wextra -pedantic -std=c99

release: ./src/*.c ./inc
	$(CC) ./src/*.c -I ./inc -o ./bin/hayai_release -O3 

run: release
	./bin/hayai_release

clean:
	rm ./bin/*
