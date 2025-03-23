default: run

clean:
	rm -rf _b

amalgamate:
	./src/amalgamate.sh

config:
	cmake -DCMAKE_BUILD_TYPE=Debug -S src -B _b

build:
	cmake --build _b

sample:
	cc -o _b/sample sample.c

run: amalgamate config build sample
	_b/sample hi hello whats up
	_b/unit_tests
	_b/e2e_tests