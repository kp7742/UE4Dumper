#include "kmods.h"
#include "SDK.h"

using namespace std;

const char* short_options = "hlrfnsep:o:g:u:";
const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"lib", no_argument, NULL, 'l'},
		{"raw", no_argument, NULL, 'r'},
		{"fast", no_argument, NULL, 'f'},
		{"package", required_argument, NULL, 'p'},
		{"output", required_argument, NULL, 'o'},
		{"gname", required_argument, NULL, 'g'},
		{"guobj", required_argument, NULL, 'u'},
		{"objs", no_argument, NULL, 'n'},
		{"strings", no_argument, NULL, 's'},
		{"sdk", no_argument, NULL, 'e'},
		{NULL, 0, NULL, 0}
};

void Usage() {
	printf("UE4Dumper v0.2 <==> Made By KMODs(kp7742)\n");
	printf("Usage: ue4dumper <option(s)>\n");
	printf("Dump Lib libUE4.so from Memory of Game Process and Generate structure SDK\n");
	printf("Tested on PUBG Mobile and PUBG Mobile Lite\n");
	printf(" Options:\n");
	printf("--Utils Dump Args-----------------------------------------------------------------------\n");
	printf("  -g --gname <address>                  GNames Pointer Address\n");
	printf("  -u --guobj <address>                  GUObject Pointer Address\n");
	printf("  -n --objs                             Dumping Object List\n");
	printf("  -s --strings                          Dump Strings\n");
	printf("  -e --sdk                              Dump SDK\n");
	printf("--Lib Dump Args-------------------------------------------------------------------------\n");
	printf("  -l --lib                              Dump libUE4.so from Memory\n");
	printf("  -r --raw(Optional)                    Output Raw Lib and Not Rebuild It\n");
	printf("  -f --fast(Optional)                   Enable Fast Dumping(May Miss Some Bytes in Dump)\n");
	printf("--Other Args----------------------------------------------------------------------------\n");
	printf("  -p --package <packageName>            Package Name of App(Default: com.tencent.ig)\n");
	printf("  -o --output <outputPath>              File Output path(Default: /sdcard)\n");
	printf("  -h --help                             Display this information\n");
}

kaddr getHexAddr(const char* addr){
	auto is16Bit = [](const char* c) {
		auto len = strlen(c);
		if(len > 2) {
			if(c[0] == '0' & c[1] == 'x') return true;
		}
		bool is10bit = true;
		for(auto i = 0; i < len; i++) {
			if((c[i] > 'a' && c[i] < 'f') ||
			   (c[i] > 'A' && c[i] < 'F')) {
				is10bit = false;
			}
		}
		return !is10bit;
	};
#ifndef __SO64__
	return (kaddr) strtoul(addr, nullptr, is16Bit(addr) ? 16: 10);
#else
	return (kaddr) strtoull(addr, nullptr, is16Bit(addr) ? 16: 10);
#endif
}

int main(int argc, char *argv[]) {
	int c;
	string pkg("com.tencent.ig"), outputpath("/sdcard");
	bool isValidArg = true,
	isLibDump = false,
	isFastDump = false,
	isRawDump = false,
	isObjsDump = false,
	isStrDump = false,
	isSdkDump = false;

	while((c = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
		switch (c) {
			case 'l':
				isLibDump = true;
				break;
			case 'r':
				isRawDump = true;
				break;
			case 'f':
				isFastDump = true;
				break;
			case 'p':
				pkg = optarg;
				break;
			case 'o':
				outputpath = optarg;
				break;
			case 'g':
				Offsets::GNames = getHexAddr(optarg);
				break;
			case 'u':
				Offsets::GUObjectArray = getHexAddr(optarg);
				break;
			case 'n':
				isObjsDump = true;
				break;
			case 's':
				isStrDump = true;
				break;
			case 'e':
				isSdkDump = true;
				break;
			default:
				isValidArg = false;
				break;
		}
	}

	if(!isValidArg || (!isLibDump && !isObjsDump && !isStrDump && !isSdkDump)) {
		printf("Wrong Arguments, Please Check!!\n");
		Usage();
		return -1;
	}

	if(Offsets::GNames < 1 || Offsets::GUObjectArray < 1){
		printf("Please Enter Correct GName and GUObject Addresses!!\n");
		Usage();
		return -1;
	}

	//Find PID
	target_pid = find_pid(pkg.c_str());
	if (target_pid == -1) {
		cout << "Can't find the process" << endl;
		return -1;
	}
	cout << "Process name: " << pkg.c_str() << ", Pid: " << target_pid << endl;

	//Lib Base Address
	libbase = get_module_base(lib_name);
	if (libbase == 0) {
		cout << "Can't find Library: " << lib_name << endl;
		return -1;
	}
	cout << "Base Address of " << lib_name << " Found At " << setbase(16) << libbase << setbase(10) << endl;

	if(isLibDump) {
		//Lib End Address
		kaddr start_addr = libbase;
		kaddr end_addr = get_module_end(lib_name);
		if (end_addr == 0) {
			cout << "Can't find End of Library: " << lib_name << endl;
			return -1;
		}
		cout << "End Address of " << lib_name << " Found At " << setbase(16) << end_addr << setbase(10) << endl;

		//Lib Dump
		size_t libsize = (end_addr - libbase);
		cout << "Lib Size: " << libsize << endl;

		if(isRawDump){
			ofstream rdump(outputpath + "/" + lib_name, ofstream::out | ofstream::binary);
			if (rdump.is_open()) {
				if (isFastDump) {
					uint8_t *buffer = new uint8_t[libsize];
					memset(buffer, '\0', libsize);
					vm_readv((void *) start_addr, buffer, libsize);
					rdump.write((char *) buffer, libsize);
				} else {
					char *buffer = new char[1];
					while (libsize != 0) {
						vm_readv((void *) (start_addr++), buffer, 1);
						rdump.write(buffer, 1);
						--libsize;
					}
				}
			} else {
				cout << "Can't Output File" << endl;
				return -1;
			}
			rdump.close();
		} else {
			ElfReader elf_reader;
			string tempPath = outputpath + "/KTemp.dat";

			ofstream ldump(tempPath, ofstream::out | ofstream::binary);
			if (ldump.is_open()) {
				if (isFastDump) {
					uint8_t *buffer = new uint8_t[libsize];
					memset(buffer, '\0', libsize);
					vm_readv((void *) start_addr, buffer, libsize);
					ldump.write((char *) buffer, libsize);
				} else {
					char *buffer = new char[1];
					while (libsize != 0) {
						vm_readv((void *) (start_addr++), buffer, 1);
						ldump.write(buffer, 1);
						--libsize;
					}
				}
			} else {
				cout << "Can't Output File" << endl;
				return -1;
			}
			ldump.close();

			//SoFixer Code//
            cout << "Rebuilding Elf(So)" << endl;

			elf_reader.setDumpSoFile(true);
			elf_reader.setDumpSoBaseAddr(start_addr);

			auto file = fopen(tempPath.c_str(), "rb");
			if (nullptr == file) {
				printf("source so file cannot found!!!\n");
				return -1;
			}
			auto fd = fileno(file);

			elf_reader.setSource(tempPath.c_str(), fd);

			if (!elf_reader.Load()) {
				printf("source so file is invalid\n");
				return -1;
			}

			ElfRebuilder elf_rebuilder(&elf_reader);
			if (!elf_rebuilder.Rebuild()) {
				printf("error occured in rebuilding elf file\n");
				return -1;
			}
			fclose(file);
			//SoFixer Code//

			ofstream redump(outputpath + "/" + lib_name, ofstream::out | ofstream::binary);
			if (redump.is_open()) {
				redump.write((char*) elf_rebuilder.getRebuildData(), elf_rebuilder.getRebuildSize());
			} else {
				cout << "Can't Output File" << endl;
				return -1;
			}

			remove(tempPath.c_str());
			redump.close();
		}
	}

	if(isStrDump) {
		DumpStrings(outputpath);
		cout << endl;
	}
	if(isObjsDump) {
		DumpObjects(outputpath);
		cout << endl;
	}
	if(isSdkDump) {
		DumpSDK(outputpath);
		cout << endl;
	}
	return 0;
}






