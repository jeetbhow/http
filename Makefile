MESON ?= meson
BUILD_DIR ?= build
RELEASE_DIR ?= build-release
CPP_STD ?= c++23

.PHONY: all build setup run test test-request release run-release clean

all: build

build:
	@test -f "$(BUILD_DIR)/meson-private/coredata.dat" || $(MESON) setup "$(BUILD_DIR)"
	$(MESON) compile -C "$(BUILD_DIR)"

setup:
	@if test -f "$(BUILD_DIR)/meson-private/coredata.dat"; then \
		$(MESON) configure "$(BUILD_DIR)" -Dcpp_std=$(CPP_STD) && \
		$(MESON) setup --reconfigure "$(BUILD_DIR)"; \
	else \
		$(MESON) setup "$(BUILD_DIR)" -Dcpp_std=$(CPP_STD); \
	fi

run: build
	"./$(BUILD_DIR)/http"

test: build
	$(MESON) test -C "$(BUILD_DIR)" --print-errorlogs

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
