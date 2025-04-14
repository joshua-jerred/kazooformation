# Kazooformation

## Build & Run

Simple CMake/GCC build system

Build Essential, Lib FFTW-3, and Pulse Audio must be installed.

```bash
mkdir build && cd build

# CMake Configure
cmake ..

# Build
make

# Launch Terminal Client
./kazoo_connect

# Launch Web Server
./kazoo_connect_server
```

## Run With Optional Web Server

The executable `kazoo_connect_server` runs a *very basic* http server that can
be used alongside a react app found in `./src/web_client`. This can be run
like any other NPM project.

```bash
npm install
npm run start
```
