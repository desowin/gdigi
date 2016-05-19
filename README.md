# Control your Digitech effect pedal under Linux!

gdigi is tool aimed to provide X-Edit functionality to Linux users. [Official website](http://desowin.org/gdigi/).

[![Build Status](https://travis-ci.org/desowin/gdigi.svg?branch=master)](https://travis-ci.org/desowin/gdigi)

## Supported devices

- RP150
- RP155
- RP250
- RP255
- RP355
- RP500
- RP1000
- GNX3000
- GNX4K

## Setup

gdigi is available in major Linux distributions. In Debian/Ubuntu it can be installed using `sudo apt-get install gdigi`

### Building instructions

gdigi uses following libraries: alsa, gtk+, glib, expat, libxml-2

Following commands can be used in order to build latest (development) version on Debian/Ubuntu system:

``` sh
sudo apt-get install build-essential libasound2-dev libgtk-3-dev libglib2.0-dev libexpat1-dev libxml2-dev
wget https://github.com/desowin/gdigi/archive/master.tar.gz -O gdigi-master.tar.gz
tar xvzpf gdigi-master.tar.gz
cd gdigi-master
make
./gdigi
```

## Usage

Command line options:

--device (-d)

Example:
``` sh
gdigi -d hw:1,0,0
gdigi --device=hw:1,0,0
```
