# Driver KinectV1 [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

SteamVR driver for Kinect for XBox 360.

## Usage benefits
* No additional dependencies for SteamVR ([OpenVR InputEmulator](https://github.com/matzman666/OpenVR-InputEmulator) and others).
* Linear smoothing of movement. 

## Installation
* Install [Kinect for Windows Runtime 1.8](https://www.microsoft.com/en-us/download/details.aspx?id=40277)
* Extract [latest release archive](../../releases/latest) to `<SteamVR_folder>/drivers`
* Add line in section `steamvr` of `<Steam_folder>/config/steamvr.vrsettings` file:
  ```JSON
  "activateMultipleDrivers": true,
  ```

## Calibration
* Launch `kinect_calibrator` from `<SteamVR_folder>/drivers/kinectV1/bin/win##` directory.
* Allign base position using controllers to match virtual trackers with your hip and legs:
  * Left controller's touchpad/thumbstick: rotate left, rotate right, rotate up, rotate down. Press grip to reset rotation.
  * Right controller's touchpad/thumbstick: move right, move left, move up, move down. Press grip to reset position.
    * If trigger is pressed: move forward, move backward.
* Press application menu button or quit from SteamVR to save alligned base transformation.

## Usage
* Press **Ctrl-T** to disable/enable tracking.
  * Alternative way: send debug message `switch` to fake base station device.
* By default only `HipCenter`, `AnkleLeft` and `AnkleRight` bones are interpreted as trackers. You can add/remove specific bones for/from tracking by editing [`resources/settings.xml`](../master/resources/settings.xml).

## Building
* Install [Kinect for Windows SDK 1.8](https://www.microsoft.com/en-us/download/details.aspx?id=40278).
* Clone repository using `git`.
* Initialize submodules
```
git submodule update --init --depth=1
```
* Open `driver_kinectV1.sln` in Visual Studio 2013.

## Credits
* Thanks to @Holiks-Serbuchev for testing and debugging on real device.
