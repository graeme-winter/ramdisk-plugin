# Ramdisk Plugin

XDS plugin to process diffraction data from raw HDF5 file chunks in RAM disk, not in an HDF5 file. Code derived from [generic plugins](https://wiki.uni-konstanz.de/xds/index.php/LIB) and very much early stage work in progress. Also substantial learning from the [durin plugin](https://github.com/DiamondLightSource/durin).

Goal: if the data are captured to a ramdisk from the DECTRIS zeroMQ stream defined in the [simplon API docs](https://media.dectris.com/210607-DECTRIS-SIMPLON-API-Manual_EIGER2-chip-based_detectros.pdf) then this plugin can be used with XDS to read that data.

Dependencies:

- [cJSON](https://github.com/DaveGamble/cJSON) to read the config packet
- [bitshuffle](https://github.com/kiyo-masui/bitshuffle) to unpack the image packets

Assumptions:

- the data are masked by the detector

## Building

First ensure you do a recursive clone of the repo i.e.

```
git clone --recursive git@github.com:graeme-winter/ramdisk-plugin.git
```

This will clone the `bitshuffle` and `cJSON` repositories also, which are dependencies of the ramdisk plugin. Building is simple: `make` - this will build the plugin `build/ramdisk-plugin.so`. You can also build a test program with `make test`.

The test program can be run as `./build/test <dumped stream directory>` and will first print out a few header details before printing the totals of unmasked pixels.

## Usage

As with other usages of plugins with XDS:

```
LIB= /Users/graeme/git/ramdisk-plugin/build/ramdisk-plugin.so
```

N.B. at the moment this _will not work_ because setting XDS.INP parameter:

```
NAME_TEMPLATE_OF_DATA_FRAMES= /Users/graeme/data/i04-1-run3-ins/ramdisk/
```

is not currently supported. Trying to figure out a workaround for this.

## Testing

There really should be some, certainly more than a `test` binary.
