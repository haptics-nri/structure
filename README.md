# structure
stuff to make the Structure Sensor work on the NUC


## Installation ##

- in Config/Drivers/OpenNI2/PS{1080,Link}.ini, change ';UsbInterface=2' to 'UsbInterface=0' (the doc says to change in /etc/openni2/GlobalDefaults.ini but that doesn't exist)
- `sudo apt-get install g++ python libusb-1.0-0-dev libudev-dev openjdk-6-jdk freeglut3-dev doxygen graphviz`
- `make`
