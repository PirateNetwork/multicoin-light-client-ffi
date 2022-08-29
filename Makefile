SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

PLATFORMS = ios-device macos ios-simulator
IOS_DEVICE_ARCHS = aarch64-apple-ios
IOS_SIM_ARCHS_STABLE = x86_64-apple-ios  aarch64-apple-ios-sim
MACOS_ARCHS = x86_64-apple-darwin aarch64-apple-darwin
IOS_SIM_ARCHS = $(IOS_SIM_ARCHS_STABLE)

RUST_SRCS = $(shell find rust -name "*.rs") Cargo.toml
STATIC_LIBS = $(shell find target -name "libpiratelc.a")

install:
	rustup toolchain add stable
	rustup +stable target add aarch64-apple-ios x86_64-apple-ios x86_64-apple-darwin aarch64-apple-darwin aarch64-apple-ios-sim
	
.PHONY: install

release: clean xcframework
.PHONY: release

clean:
	rm -rf products
	rm -rf rust/target
.PHONY: clean

xcframework: products/libpiratelc.xcframework
	mkdir -p releases/XCFramework/
	rsync -avr --exclude='*.DS_Store' products/libpiratelc.xcframework releases/XCFramework/
.PHONY: xcframework

products/libpiratelc.xcframework: $(PLATFORMS)
	rm -rf $@
	mkdir -p $@
	cp -R products/ios-device/frameworks $@/ios-arm64
	cp -R products/ios-simulator/frameworks $@/ios-arm64_x86_64-simulator
	cp -R products/macos/frameworks $@/macos-arm64_x86_64
	cp support/Info.plist $@

frameworks: $(PLATFORMS)
.PHONY: frameworks

$(PLATFORMS): %: products/%/frameworks/libpiratelc.framework
.PHONY: $(PLATFORMS)

products/%/frameworks/libpiratelc.framework: products/%/universal/libpiratelc.a
	rm -rf $@
	mkdir -p $@
	cp products/$*/universal/libpiratelc.a $@/libpiratelc
	cp -R rust/target/Headers $@
	mkdir $@/Modules
	cp support/module.modulemap $@/Modules

products/macos/universal/libpiratelc.a: $(MACOS_ARCHS)
	mkdir -p $(@D)
	lipo -create $(shell find products/macos/static-libraries -name "libpiratelc.a") -output $@

products/ios-simulator/universal/libpiratelc.a: $(IOS_SIM_ARCHS)
	mkdir -p $(@D)
	lipo -create $(shell find products/ios-simulator/static-libraries -name "libpiratelc.a") -output $@

products/ios-device/universal/libpiratelc.a: $(IOS_DEVICE_ARCHS)
	mkdir -p $(@D)
	lipo -create $(shell find products/ios-device/static-libraries -name "libpiratelc.a") -output $@

$(MACOS_ARCHS): %: stable-%
	mkdir -p products/macos/static-libraries/$*
	cp rust/target/$*/release/libpiratelc.a products/macos/static-libraries/$*
.PHONY: $(MACOS_ARCHS)

$(IOS_DEVICE_ARCHS): %: stable-%
	mkdir -p products/ios-device/static-libraries/$*
	cp rust/target/$*/release/libpiratelc.a products/ios-device/static-libraries/$*
.PHONY: $(IOS_DEVICE_ARCHS)

$(IOS_SIM_ARCHS_STABLE): %: stable-%
	mkdir -p products/ios-simulator/static-libraries/$*
	cp rust/target/$*/release/libpiratelc.a products/ios-simulator/static-libraries/$*
.PHONY: $(IOS_SIM_ARCHS_STABLE)

stable-%: # target/%/release/libpiratelc.a:
	sh -c "RUSTUP_TOOLCHAIN=stable cargo build --manifest-path rust/Cargo.toml --target $* --release"
