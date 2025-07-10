# Workshop - Embedded firmware development

This demo project was developed for a course on professional embedded firmware development using the ESP32 platform. The firmware is modular, testable, and portable, with hardware abstraction layers for both ESP32 and POSIX environments (for PC testing or emulators).

## Environment Setup

Follow these steps to get the project running locally for development and testing.

### Clone the repository

This project is hosted on GitHub. Clone it with all submodules:

```sh
git clone --recurse-submodules -j8 git@github.com:iotinga/univr-info-workshop.git
```

> Note: `-j8` speeds up submodule fetching.

If the project is already cloned, synchronize submodules manually

```sh
git submodule update --init --recursive
```

### Setup ESP-IDF commands (without the VSCode extension)

Navigate to the ESP-IDF directory (`./esp-idf/`) and run the installer:

```sh
./install.sh
```

> To switch SDK versions, run `install.sh` in the desired SDK directory.

Activate the environment (when using a normal shell):

```sh
. ./export.sh
```

> If using the VSCode extension terminal, this step is usually handled automatically.

### Install the ESP-IDF extension for VSCode (Optional)

In the setup wizard, choose **"USE EXISTING SETUP"**.

If previously configured, select **"Search ESP-IDF in system..."** and make sure:

- SDK path is set to `${workspaceFolder}/esp-idf`
- Tools directory is set to `${HOME}/.espressif`
- Python interpreter is your system’s Python (e.g. `/opt/homebrew/bin/python` on macOS), **not** the virtualenv inside `.espressif`, to avoid persistent misconfiguration.

### Setup IntelliSense, Linter and Formatter

Follow the official ESP-IDF guide:

https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/C_CPP_CONFIGURATION.md

Use the **second configuration** (with `build-commands.json`) which requires a successful build.

For a consistent coding style, install the recommended VSCode extensions listed in `.vscode/extensions.json`.

Install `clang-format` for formatting:

- On Linux:

```sh
sudo apt install clang-format
```

- On macOS:

```sh
brew install clang-format
```

### Select port interface (VSCode extension)

VSCode Command Palette -> ESP-IDF: Select port to use (...)
E.g: `/dev/cu.usbmodem1401/` or `/dev/tty.usbserial-1140`
Then select the UART interface.

### Select target device (VSCode extension)

It should be already set in workspace settings ("idf.adapterTargetName": "esp32").

VSCode Command Palette -> ESP-IDF: Set Espressif device target
Then select `esp32`.

### Open a shell with the ESP-IDF environment (VSCode extension)

VSCode Command Palette -> ESP-IDF: Open ESP-IDF Terminal

## Building the Project

### Building for ESP32

```sh
make build
```

## Building for Emulator (POSIX/Linux)

```sh
make build-emulator
```

> This builds and runs the POSIX target on a regular machine (no ESP32 required).

## Flashing the firmware

Flash to connected ESP32 DevKit:

```sh
make flash
```

## Monitor Serial Output

```sh
make monitor
```

## Testing

There are two types of tests:

- **Unit Tests**: test isolated logic in modules; run on the POSIX target.
- **Integration Tests**: verify interactions between components and with hardware; run on ESP32.

### Running Unit Tests (CMocka)

```sh
make tests
```

> Runs all CMocka tests found in the `test/` directory.

### Running Integration Tests (ESP32 via UART or QEMU)

Use `pytest-embedded` to run integration tests on ESP32:

```sh
pytest integration_tests --target esp32
```

## Clean Build Artifacts

```sh
make clean
```

## License

Copyright (c) 2025 IOTINGA S.r.l.

This project is released under the [MIT License](LICENSE) for academic and educational purposes.
