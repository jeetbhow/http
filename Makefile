MESON ?= meson
BUILD_DIR ?= build
RELEASE_DIR ?= build-release

.PHONY: all build run test-request release run-release clean

all: build

build:
	@test -f "$(BUILD_DIR)/meson-private/coredata.dat" || $(MESON) setup "$(BUILD_DIR)"
	$(MESON) compile -C "$(BUILD_DIR)"

run: build
	"./$(BUILD_DIR)/http"

test-request:
	python3 scripts/test_request.py

release:
	@test -f "$(RELEASE_DIR)/meson-private/coredata.dat" || $(MESON) setup "$(RELEASE_DIR)" --buildtype=release
	$(MESON) compile -C "$(RELEASE_DIR)"

run-release: release
	"./$(RELEASE_DIR)/http"

clean:
	@if test -f "$(BUILD_DIR)/meson-private/coredata.dat"; then $(MESON) compile -C "$(BUILD_DIR)" --clean; fi
	@if test -f "$(RELEASE_DIR)/meson-private/coredata.dat"; then $(MESON) compile -C "$(RELEASE_DIR)" --clean; fi
