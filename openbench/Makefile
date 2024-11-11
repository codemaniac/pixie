EXE ?= pixie
PIXIE_CONFIG = test_macos64

all: pixie move

pixie:
	@echo "==== Building pixie ($(PIXIE_CONFIG)) from OpenBench ===="
	@${MAKE} --no-print-directory -C . -f ./Makefile config=$(PIXIE_CONFIG)

move:
	@mv ./openbench/pixie ./openbench/$(EXE)
