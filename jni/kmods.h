#ifndef KMODS_H
#define KMODS_H

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <dirent.h>
#include <unistd.h>
#include <cmath>
#include <ctime>
#include <algorithm>
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

#include "ELF64/fix.h"

#else
#include "ELF/ElfReader.h"
#include "ELF/ElfRebuilder.h"
#endif

bool isUE423 = false;
bool isVerbose = false;
bool deRefGNames = true;
bool deRefGUObjectArray = false;
string pkg("com.tencent.ig");
static const char *lib_name = "libUE4.so";

bool isStartWith(string str, const char *check) {
    return (str.rfind(check, 0) == 0);
}

bool isEqual(char *s1, const char *s2) {
    return (strcmp(s1, s2) == 0);
}

bool isEqual(string s1, const char *check) {
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

void trimStr(string &str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

bool isASCII(const string &s) {
    return !any_of(s.begin(), s.end(), [](char c) {
        return static_cast<unsigned char>(c) > 127;
    });
}

bool isApexLegends() {
    return isEqual(pkg, "com.ea.gp.apexlegendsmobilefps");
}

bool isFortnite() {
    return isEqual(pkg, "com.epicgames.fortnite");
}

bool isARKSurvival() {
    return isEqual(pkg, "com.studiowildcard.wardrumstudios.ark");
}

bool isGameOfPeace() {
    return isEqual(pkg, "com.tencent.tmgp.pubgmhd");
}

bool isPUBGLite() {
    return isEqual(pkg, "com.tencent.iglite");
}

bool isBGMIndia() {
    return isEqual(pkg, "com.pubg.imobile");
}

bool isPUBGSeries() {
    return isEqual(pkg, "com.tencent.ig") ||
           isEqual(pkg, "com.tencent.igce") ||
           isEqual(pkg, "com.pubg.krmobile") ||
           isEqual(pkg, "com.vng.pubgmobile") ||
           isEqual(pkg, "com.rekoo.pubgm") || isPUBGLite() || isBGMIndia();
}

#endif
