# s0
RasPi based S0 powermeter sampling + remote graphing tool

## Pi part
This part needs the "wiringPi" library and should be otherwise independent from any further tools.
Contact me if you find any further dependencies.
The main idea is to keep this as simple as possible.

```
git clone git://git.drogon.net/wiringPi
cd wiringPi/
./build
```

Once the library is installed, you can (hopefully) build the s0 binary by just executing "make" in the "s0" folder.

## configuration
The configuration is very simple, it consists of one file:  s0.ini (example included)

Each line in this file is either a GPIO configuration or a specific file to save the counters.

### s0 pin lines
```
#0,0,1000
```

### file config lines
Two settings are needed here:

LAST is the file where the most recent counters are saved. This one must be on a RAM disk, otherwise your SD card will be written to death pretty quickly.

SAVE is the file which keeps the values over reboots, this needs to be on a reboot-safe filesystem.

The "LAST" file (/var/run/s0.last) is currently hardcoded in the s0 binary, but you can change it by changing the line ```#define FILENAME "/var/run/s0.last"``` to what ever you desire and re-compiling the binary.

```
LAST=/var/run/s0.last
SAVE=/var/cache/s0.last
```

## remote part
(TODO)
