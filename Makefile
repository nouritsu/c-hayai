ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

build: ./src/*.c ./inc
	$(CC) ./src/*.c -I ./inc -o ./bin/hayai_debug -Wall -Wextra -pedantic -std=c99

release: ./src/*.c ./inc
	$(CC) ./src/*.c -I ./inc -o ./bin/hayai_release -O3 

run: release
	./bin/hayai_release $(RUN_ARGS)

clean:
	rm ./bin/*
