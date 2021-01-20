# xdk-datalogger
Raw sensors datalogging functionality using Bosch XDK110.

# DISCLAIMER
After reading this, download the `zip` file, over the Releases page, import into your XDK Workbench, compile and flash the project on the XDK.

## Brief description
XDK Datalogger logs sensor data to a file in the SD Card. The output file is `data_##.csv`.

## Sensors enabled
- Accelerometer [mG]
- Humidity [%]
- Pressure [hPa]
- Temperature [C]
- Light intensity [Lm]
- Battery Voltage [V]

## Requirements
- [XDK Workbench](https://developer.bosch.com/web/xdk/downloads)
- Micro SD Card with at least 32GB on FAT32 format
  - `index.xdk` file need to exist on the card, otherwise it will not work. 

## Features
- No more complete file overwrite on reboot. A new file it's creates for each session.
- File count index retention on a second file named "index.xdk", stops files getting overwrited on reboot.
- Battery voltage monitoring.
- No known file size limit for a session.
