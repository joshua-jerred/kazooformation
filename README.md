# Kazooformation

![Image of kazoo with text ka-zoo-for-ma-tion / kazoo information](doc/images/kazooformation.png)

An abstract data storage, modulation, and demodulation library using kazoo sounds.

This is an engaging teaching tool for the lowest layer of the OSI model.

## Build & Run

Simple CMake/GCC build system

Build Essential, Lib FFTW-3, and Pulse Audio must be installed.

```bash
# Optional: install prerequisite packages
# @todo the FFTW-3 lib links to KTL but it cannot find the header
sudo apt install libgtest-dev libpulse-dev libfftw3-dev pulseaudio

# Create build dir
mkdir build && cd build

# CMake Configure
cmake ..

# Build
make

# Set Pulse Audio Defaults
pactl list sinks  
pactl set-default-sink 1 # Use the device from the list command instead of `1`

pactl list sources
pactl set-default-source 1 # Use the device from the list command instead of `1`

# Launch Terminal Client
./kazoo_connect

# Or Launch The Web Server
./kazoo_connect_server
```

## Run With Optional Web Server

The executable `kazoo_connect_server` runs a *very basic* http server that can
be used alongside a react app found in `./src/kazoo_connect_server/web_client`. This can be run
like any other NPM project.

```bash
npm install
npm run start
```

## Licensing

This project does not have a license yet, please contact me if you want to use
this or need a license applied.

## Attribution

`src/web_client/public/soundwave.svg` is from `https://composeicons.com/icons/bootstrap/soundwave`, it falls under the MIT license.