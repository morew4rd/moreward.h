.PHONY: default reset build test amalgamate clean-tests config-tests build-tests test-unit test-e2e


### entry: "make" or "make all"

default: build test
	@echo

all: reset build test
	@echo

### shortcuts

reset: clean-tests config-tests

build: amalgamate build-tests

test: test-unit test-e2e


### dev: actual work

amalgamate:
	@echo "\n================================== MAKE =======\n==> amalgamate moreward.h\n"
	./amalgamate.sh

### test: actual work

clean-tests:
	@echo "\n================================== MAKE =======\n==> clean\n"
	rm -rf _b

config-tests:
	@echo "\n================================== MAKE =======\n==> config\n"
	cmake -S tests -B _b

build-tests:
	@echo "\n================================== MAKE =======\n==> build-tests\n"
	cmake --build _b

test-unit:
	@echo "\n================================== MAKE =======\n==> test-unit\n"
	_b/test_unit

test-e2e:
	@echo "\n================================== MAKE =======\n==> test-e2e\n"
	_b/test_e2e
