#ifndef KMODS_H
#define KMODS_H

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <ctime>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>

#include "Log.h"
#include "Process.h"
#include "Mem.h"

#if defined(__LP64__)
#include "Offsets64.h"
#include "ELF64/fix.h"
#else
#include "Offsets.h"
#include "ELF/ElfReader.h"
#include "ELF/ElfRebuilder.h"
#endif

string pkg("com.tencent.ig");
static const char* lib_name = "libUE4.so";

bool isStartWith(string str, const char* check) {
	return (str.rfind(check, 0) == 0);
}

bool isEqual(char* s1, const char* s2) {
	return (strcmp(s1, s2) == 0);
}

bool isEqual(string s1, const char* check) {
	string s2(check);
	return (s1 == s2);
}

bool isEqual(string s1, string s2) {
	return (s1 == s2);
}

bool isContain(string str, string check) {
	size_t found = str.find(check);
	return (found != string::npos);
}

#endif
