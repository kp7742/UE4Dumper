## UE4Dumper(Unreal Engine 4 Dumper)
Dump Lib libUE4.so from Memory of Game Process and Generate Structure SDK of Supported Game. You can Find Latest Dumped SDK from [HERE](https://github.com/kp7742/UE4Dumper/tree/master/SDKs/)

## Changelog
- v0.1: First Release
- v0.2: Experimental 64bit Support Added
- v0.3: Fix Object Iteration Issue during Dumping SDK, Added Support to Resolve Arrays, Sets and Maps Structure
- v0.4: 1) Expanded 64bit Support,
        2) Fixed 64bit Library Rebuilding Not Working
        3) Added New Elf Dump Fix for 64bit Library
        4) Added Option to Dump SDK with GWorld

## Features
- No need of Ptrace
- Bypass Anti Debugging
- Dumping of Lib from Memory of Game
- Fix and Regenerate So(Elf) File from Dump
- Dumping of Game Structure SDK file(Need to Find Pointers Manually)
- Support Fast Dumping(May Miss some data)
- Support Any UE 4.18 Game
- Tested on 32bit PUBG Mobile Series

## Note
- Use 32bit and 64bit Version on Respected Arch of Game
- Recommend to use in Training Mode for PUBG Mobile
- If it stuck during Generating SDK, Then Simple Stop it, Check Dump file and If needed then Try again.
 
## How to use
- You can Use latest precompiled Binaries from [HERE](https://github.com/kp7742/UE4Dumper/tree/master/libs/) or You Can build your Own.
- Needs Root Access or Virtual Space
- Get Root Shell through Adb or Terminal Apps(type and run: 'su') or Normal Shell into Virtual Space via Terminal Apps
- For Usage Help: ue4dumper -h
- For General Usage: ue4dumper <option(s)>
	```
    ./ue4dumper -h
	 
    UE4Dumper v0.4 <==> Made By KMODs(kp7742)
    Usage: ue4dumper <option(s)>
    Dump Lib libUE4.so from Memory of Game Process and Generate structure SDK for UE 4.18
    Tested on PUBG Mobile Series
     Options:
    --SDK Dump With GObjectArray Args--------------------------------------------------------
      -a --sdku                             Dump SDK with GUObject
      -g --gname <address>                  GNames Pointer Address
      -u --guobj <address>                  GUObject Pointer Address
    --SDK Dump With GWorld Args--------------------------------------------------------------
      -b --sdkw                             Dump SDK with GWorld
      -g --gname <address>                  GNames Pointer Address
      -w --gworld <address>                 GWorld Pointer Address
    --Dump Strings Args----------------------------------------------------------------------
      -s --strings                          Dump Strings
      -g --gname <address>                  GNames Pointer Address
    --Dump Objects Args----------------------------------------------------------------------
      -n --objs                             Dumping Object List
      -g --gname <address>                  GNames Pointer Address
      -u --guobj <address>                  GUObject Pointer Address
    --Lib Dump Args--------------------------------------------------------------------------
      -l --lib                              Dump libUE4.so from Memory
      -r --raw(Optional)                    Output Raw Lib and Not Rebuild It
      -f --fast(Optional)                   Enable Fast Dumping(May Miss Some Bytes in Dump)
    --Other Args-----------------------------------------------------------------------------
      -p --package <packageName>            Package Name of App(Default: com.tencent.ig)
      -o --output <outputPath>              File Output path(Default: /sdcard)
      -h --help                             Display this information
	```
	
## How to Build
- Clone this repo
- Install Android NDK, if not already.
- Open Shell/CMD in Project Folder
- Drag ndk-build of NDK in Shell or CMD and then Execute
- Output will be in libs Folder.

## Credits
- [SoFixer](https://github.com/F8LEFT/SoFixer): 32bit So(Elf) Rebuilding
- [elf-dump-fix](https://github.com/maiyao1988/elf-dump-fix): 64bit So(Elf) Rebuilding

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
