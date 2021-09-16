## UE4Dumper(Unreal Engine 4 Dumper)
Unreal Engine 4 Dumper for Android Devices, Dump Lib libUE4.so from Memory of Game Process and Generate Structure SDK of Supported Game in Android. You can Find Latest Dumped SDK from [HERE](https://github.com/kp7742/UE4Dumper/tree/master/SDKs/)

## Changelog
- v0.1: First Release
- v0.2: Experimental 64bit Support Added
- v0.3:
    - 1) Fix Object Iteration Issue during Dumping SDK 
    - 2) Added Support to Resolve Arrays, Sets and Maps Structure
- v0.4:
    - 1) Expanded 64bit Support,
    - 2) Fixed 64bit Library Rebuilding Not Working
    - 3) Added New Elf Dump Fix for 64bit Library
    - 4) Added Option to Dump SDK with GWorld
    - 5) Updated Usage Text
- v0.5: Added Support to Resolve Functions
- v0.6:
    - 1) Added Support for UE 4.23+ Games for Strings and Objects(Use new Option: --newue)
    - 2) Added 64bit Offsets to Fix 64bit Support
    - 3) Updated SDK Generation Method for Faster Dumping
    - 4) Short Options has been remove due to conflict with new options
- v0.7: Fixed Object Dumping issue for PUBG CN(Tested on GP v1.8.10)
- v0.8: Fixed 64bit Support for Latest PUBG Version
- v0.9: Fixed Dumping issue with 64bit PUBG
- v0.10:
    - 1) Added Option to View Actors of Main Level(Use new Option: --actors)
    - 2) Support for PUBG CN(GP) Fixed(Tested on GP v1.9.10)
    - 3) Fixed Some Offsets Issues due to Modified UE4 Versions
    - 4) Offsets System Updated to Work with Other games, other then PUBG
- v0.11: Fixed Dumping issue with 64bit PUBG Lite
- v0.12:
    - 1) Fixed Offsets for UE 4.23+ Mode
    - 2) Fixed SDK Dumping for UE 4.23+ Mode
    - 3) Fixed String Dumping for UE 4.23+ Mode
    - 4) Added Offset Support for Apex Legends Mobile
    - 5) Added Option to Detour or De-Reference GNames, GUObject Addresses
    - 6) Added Option to See Verbose Output of String, Object and SDK Dumping
- v0.13:
    - 1) Fixed String Reading for Objects for UE 4.23+ Mode
    - 2) Fixed SDK Dumping Issue Due to Invalid UStruct
    - 3) Added Offset Support for Fortnite Mobile
- v0.14: Fixed Support for Battlegrounds Mobile India
- v0.15:
    - 1) Fixed Struct Property Dumping
    - 2) Experimental Wide String Support Added for UE 4.23+ Mode
- v0.16: Fixed Dumping issue with 64bit PUBG Lite
- v0.17:
    - 1) Fixed Crash with Deref Options
    - 2) Fixed Issue with Wide String Dumping
- v0.18:
    - 1) Fixed Dumping issue with 64bit PUBG CN
    - 2) Fixed Another Issue with String Dumping

## Features
- No need of Ptrace
- Bypass Anti Debugging
- Dumping of Lib from Memory of Game
- Fix and Regenerate So(Elf) File from Dump
- Dumping of Game Structure SDK file(Need to Find Pointers Manually)
- Support Fast Dumping(Might Miss some data)
- Support SDK Dumping for UE4 Based Android Games
- Tested on 32bit and 64bit PUBG Mobile Series

## Note
- Use 32bit and 64bit Version on Respected Arch of Game.
- Recommend to use in Training Mode for PUBG Mobile.
- Some Games with Modified UE4 Might not Dump Correctly.
- For Modified Engines, You May Need Put Your Custom Offsets For That Game.
- If it stuck during Generating SDK, Then Simply Stop it, Check Dump file and If needed then Try again.
 
## How to use
- You can Use latest precompiled Binaries from [HERE](https://github.com/kp7742/UE4Dumper/tree/master/libs/) or You Can build your Own.
- Needs Either Root Access or Virtual Space
- Put Executable in folder like /data/local/tmp (/sdcard not allow to execute binary so don't put it there)
- Get Either Root Shell through Adb or Terminal Apps(type and run: 'su') or Normal Shell into Virtual Space via Terminal Apps in that folder
- Give it executable permission with either 'chmod +x ue4dumper' or 'chmod 755 ue4dumper'
- Run './ue4dumper -h' For Usage Help
	```
    ./ue4dumper -h
	 
    UE4Dumper v0.18 <==> Made By KMODs(kp7742)
    Usage: ./ue4dumper <option(s)>
    Dump Lib libUE4.so from Memory of Game Process and Generate structure SDK for UE4 Engine
    Tested on PUBG Mobile Series and Other UE4 Based Games
    Options:
    --SDK Dump With GObjectArray Args--------------------------------------------------------
      --sdku                              Dump SDK with GUObject
      --gname <address>                   GNames Pointer Address
      --guobj <address>                   GUObject Pointer Address
    --SDK Dump With GWorld Args--------------------------------------------------------------
      --sdkw                              Dump SDK with GWorld
      --gname <address>                   GNames Pointer Address
      --gworld <address>                  GWorld Pointer Address
    --Dump Strings Args----------------------------------------------------------------------
      --strings                           Dump Strings
      --gname <address>                   GNames Pointer Address
    --Dump Objects Args----------------------------------------------------------------------
      --objs                              Dumping Object List
      --gname <address>                   GNames Pointer Address
      --guobj <address>                   GUObject Pointer Address
    --Lib Dump Args--------------------------------------------------------------------------
      --lib                               Dump libUE4.so from Memory
      --raw(Optional)                     Output Raw Lib and Not Rebuild It
      --fast(Optional)                    Enable Fast Dumping(May Miss Some Bytes in Dump)
    --Show ActorList With GWorld Args--------------------------------------------------------
      --actors                            Show Actors with GWorld
      --gname <address>                   GNames Pointer Address
      --gworld <address>                  GWorld Pointer Address
    --Other Args-----------------------------------------------------------------------------
      --newue(Optional)                   Run in UE 4.23+ Mode
      --verbose(Optional)                 Show Verbose Output of Dumping
      --derefgname(Optional) <true/false> De-Reference GNames Address(Default: true)
      --derefguobj(Optional) <true/false> De-Reference GUObject Address(Default: false)
      --package <packageName>             Package Name of App(Default: com.tencent.ig)
      --output <outputPath>               File Output path(Default: /sdcard)
      --help                              Display this information
	```
	
## How to Build
- Clone this repo
- Install Android NDK, if not already.
- Open Shell/CMD in Project Folder
- Drag ndk-build from NDK in Shell or CMD and then Execute
- Output will be in libs Folder.

## Credits
- [SoFixer](https://github.com/F8LEFT/SoFixer): 32bit So(Elf) Rebuilding
- [elf-dump-fix](https://github.com/maiyao1988/elf-dump-fix): 64bit So(Elf) Rebuilding
- [UnrealDumper-4.25](https://github.com/guttir14/UnrealDumper-4.25): UE 4.25+ Support

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
