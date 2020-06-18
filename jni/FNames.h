#ifndef FNAMES_H
#define FNAMES_H

using namespace std;

bool isUE423 = false;
uint32 GNameCount = 170000;

string getUEString(kaddr address) {
    unsigned int MAX_SIZE = 100;

    string uestring(ReadStr(address, MAX_SIZE));
    uestring.shrink_to_fit();

    return uestring;
}

string GetFNameFromID(uint32 index) {
	if(isUE423){
		uint32 chunkOffset = index >> 16;
		uint16 nameOffset = index;

		kaddr fNamePool = getRealOffset(Offsets::GNames) + 0x30;

		kaddr namePoolChunk = getPtr(fNamePool + ((chunkOffset + 2) * 8));
		kaddr entryOffset = namePoolChunk + (2 * nameOffset);

		int16 nameEntry = Read<int16>(entryOffset);
		int nameLength = nameEntry >> 6;

		string uestring(ReadStr(entryOffset + 0x2, nameLength));
		uestring.shrink_to_fit();

		return uestring;
	} else {
		kaddr TNameEntryArray = getPtr(getRealOffset(Offsets::GNames));

		kaddr FNameEntryArr = getPtr(TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
		kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));

		return getUEString(FNameEntry + Offsets::FNameEntryToNameString);
	}
}

void DumpStrings(string out) {
	uint32 count = 0;
	ofstream gname(out + "/Strings.txt", ofstream::out);
	if (gname.is_open()) {
		cout << "Dumping Strings" << endl;
		for (uint32 i = 0; i < GNameCount; i++) {
			string s = GetFNameFromID(i);
			if(!s.empty()) {
				gname << "[" << i << "]: " << s << endl;
				count++;
			}
		}
		gname.close();
		cout << count << " Strings Dumped" << endl;
	}
}

#endif