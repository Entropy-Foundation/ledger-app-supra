# Ledger Supra Application

Supra wallet application for Nano S, Nano S Plus and Nano X.

## Quick start

> **Note:** The `make_wrapper.sh` script requires the name of the wallet model as an argument.  
> In the examples below, we use `nanos` to demonstrate usage with the **Nano S** device.  
> To use a different device model, replace:
>
> - `nanos` with `nanosp` for **Nano S Plus**
> - `nanos` with `nanox` for **Nano X**

- Building app.

```shell
# To build app for Nano S wallet.
./make_wrapper.sh 1 nanos
```

- Starting the speculos emulator

```shell
# To start speculos emulator with Nano S wallet model.
./make_wrapper.sh 2 nanos
```

- Loading app into ledger wallet.

```shell
# To load app in Nano S wallet.
./make_wrapper.sh 3 nanos
```

- Deleting app from the ledger wallet.

```shell
# To delete app from Nano S wallet.
./make_wrapper.sh 4 nanos
```

- Running tests.

```shell
# To run all tests with Nano S wallet model, replace nanos with nanosp for Nano S Plus and nanox for Nano X.
./make_wrapper.sh 5 nanos
```

If you'd prefer to perform these operations manually or want a deeper understanding of the workflow, refer to the other sections in this guide.  
Otherwise, this **Quick Start** section should be sufficient for most common use cases.

## Compilation

### Setup

To build the ledger app we will use [Ledger App Builder](https://github.com/LedgerHQ/ledger-app-builder), it is a container image which holds the all dependencies to compile an application for Nano hardware wallets.

To use the container image, you need to install [Docker](https://docs.docker.com/get-docker/).

We can build the image from source but its best to use image hosted on remote.

Pull `ledger-app-builder` docker image:

```shell
docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest
```

### Build the Application

**For the Nano S**:

Run command from the source folder of the application (or replace ‘realpath’ with your app’s local path):

```shell
# Run in your shell.
docker run --rm -ti -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest

# Run in docker container interactive shell.
make clean; make
```

**For the Nano X and Nano S Plus**:

For Nano X and S Plus, specify the `BOLOS_SDK` environment variable before building your app, in the source folder of the app.

Run command from the source folder of the application (or replace ‘realpath’ with your app’s local path):

For Nano S Plus:

```shell
# Run in your shell.
docker run --rm -ti -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest

# Run in docker container interactive shell.
make clean; BOLOS_SDK=$NANOSP_SDK make
```

For Nano X:

```shell
docker run --rm -ti -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:latest

# Run in docker container interactive shell.
make clean; BOLOS_SDK=$NANOX_SDK make
```

### Exit the image

The build generates several files in your application folder and especially the `app.elf` that can be loaded to a Nano S or S Plus or into the Nano X or S Emulator (Speculos).

You can exit the image, with the `exit` command.

## Speculos emulator

Speculos is the emulator of Ledger hardware wallets (Nano S, Nano X, Nano S Plus and Blue) on desktop computers. It is particularly useful when

- you don’t have the physical hardware device, or
- you want to facilitate the pressing of Nano buttons.

### Setup virtualenv

```shell
sudo apt install python3-pip

pip3 install virtualenv
```

### Create virtual environment

```shell
# Create virtual environment in current working directory, and it is expected that current working
# directory is a app source directory.
virtualenv -p python3 .

source ./bin/activate

pip3 install speculos
```

### Load app in speculos emulator

Run command from the source folder of the application (or replace ‘realpath’ with your app’s local path):

```shell

speculos "$(realpath .)"/bin/app.elf --model nanos --display headless --seed "shoot"

# Open http://127.0.0.1:5000 in your browser to view speculos ledger wallet emulator interface. 
```

_**NOTE: For more information see the Speculos GitHub repository:**_
<https://github.com/LedgerHQ/speculos/blob/master/docs/index.md>

## Documentation

High level documentation such as [APDU](doc/APDU.md), [commands](doc/COMMANDS.md) and [transaction serialization](doc/TRANSACTION.md) are included in developer documentation which can be generated with [doxygen](https://www.doxygen.nl)

```shell
doxygen .doxygen/Doxyfile
```

the process outputs HTML and LaTeX documentations in `doc/html` and `doc/latex` folders.
