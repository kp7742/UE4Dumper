## UE4Dumper(Unreal Engine 4 Dumper)
Dump Lib libUE4.so from Memory of Game Process and Generate Structure SDK of Supported Game

## Features
- No need of Ptrace
- Bypass Anti Debugging
- Dumping of Lib from Memory of Game
- Fix and Regenerate So(Elf) File
- Dumping of Game Structure SDK file(Need to Find GNames and GUObject Address Manually)
- Support Fast Dumping(May Miss some data)
- Tested on PUBG Mobile and PUBG Mobile Lite

## Note
- If it stuck during Generating SDK, Then Simple Stop it and Try again.
 
## How to use
- You can Use latest precompiled Binaries from [HERE](https://github.com/kp7742/UE4Dumper/tree/master/libs/) or You Can build your Own.
- Needs Root Access or Virtual Space
- Get Root Shell through Adb or Terminal Apps(type and run: 'su') or Normal Shell into Virtual Space via Terminal Apps
- For Usage Help: ue4dumper -h
- For General Usage: ue4dumper <option(s)>
	```
	 ./ue4dumper -h
	 
	 UE4Dumper v0.2 <==> Made By KMODs(kp7742)
	 Usage: ue4dumper <option(s)>
	 Dump Lib libUE4.so from Memory of Game Process and Generate structure SDK
	 Tested on PUBG Mobile and PUBG Mobile Lite
	  Options:
	 --Utils Dump Args-----------------------------------------------------------------------
	   -g --gname <address>                  GNames Pointer Address
	   -u --guobj <address>                  GUObject Pointer Address
	   -n --objs                             Dumping Object List
	   -s --strings                          Dump Strings
	   -e --sdk                              Dump SDK
	 --Lib Dump Args-------------------------------------------------------------------------
	   -l --lib                              Dump libUE4.so from Memory
	   -r --raw(Optional)                    Output Raw Lib and Not Rebuild It
	   -f --fast(Optional)                   Enable Fast Dumping(May Miss Some Bytes in Dump)
	 --Other Args----------------------------------------------------------------------------
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
- [SoFixer](https://github.com/F8LEFT/SoFixer): So(Elf) Rebuilding

## Technlogy Communication
> Email: patel.kuldip91@gmail.com
