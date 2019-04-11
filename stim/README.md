# CwruStimLib
CWRU UECU Muscle Stimulation Board C Library

**Git Clone this repo to your local work folder. Branch it if you need to modify it.**

* [Folder/User] Example - Sample higher level code to use this library.
* [Folder/Advanced] CwruStimLibHAL - Platform specific Hardware Abstraction Layer (HAL).
* [Folder/Data] PresetPatterns - Preset pattern files.

* [Data] CwruStimLibStruct.h - The data structure of the Stim class.
* [Data] CwruStimLibConst.h - Constant values and presettings.

* [User] CwruStimLib.h/.c - Top level user functions. User 
only need to call these functions to setup, start, and update the stim boards.

* [Advanced] CwruStimExe.h/.c - Advanced user functions, mostly low level hardware related. Normal user don't need to call these functions, but it can give you the full control at channel level.
* [Advanced] CwruStimCmd.h/.c - UECU command functions. Specific design to match the UECU and stim board communication requirements.
* [Advanced] CwruStimLibInit.h/.c - Routine procedures to setup the boards, channels, and patterns.
* [Advanced] CwruStimLibUtils.h/.c - Utility functions to make life easier.

**Note**

The UECU Message Handbook I made, and it would be the most helpful doc if anyone want to work on the stim board communication in the future:
https://goo.gl/s20iH4

