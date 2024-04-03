# Ramdisk Plugin

XDS plugin to process diffraction data from raw HDF5 file chunks in RAM disk, not in an HDF5 file. Code derived from [generic plugins](https://wiki.uni-konstanz.de/xds/index.php/LIB) and very much early stage work in progress. Also substantial learning from the [durin plugin](https://github.com/DiamondLightSource/durin).

Goal: if the data are captured to a ramdisk from the DECTRIS zeroMQ stream defined in the [simplon API docs](https://media.dectris.com/210607-DECTRIS-SIMPLON-API-Manual_EIGER2-chip-based_detectros.pdf) then this plugin can be used with XDS to read that data.

Dependencies:

- [cJSON](https://github.com/DaveGamble/cJSON) to read the config packet
- [bitshuffle](https://github.com/kiyo-masui/bitshuffle) to unpack the image packets

Assumptions:

- the data are masked by the detector

## Building

FIXME write this.

## Usage

FIXME also write this.

## Testing

Guess what? FIXME write this.
