default: run

clean:
	rm -rf _b

config:
	cmake -DCMAKE_BUILD_TYPE=Debug -S . -B _b

build:
	cmake --build _b

amalgamate:
	./amalgamate.sh
	./amalgamate_2files.sh

run: amalgamate config build
	_b/unit_tests
	_b/e2e_tests
	_b/sample hi hello whats up