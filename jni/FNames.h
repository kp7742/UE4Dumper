#ifndef FNAMES_H
#define FNAMES_H

using namespace std;

uint32 MAX_SIZE = 100;
uint32 GNameLimit = 170000;

struct WideStr {
    static int is_surrogate(UTF16 uc) {
        return (uc - 0xd800u) < 2048u;
    }

    static int is_high_surrogate(UTF16 uc) {
        return (uc & 0xfffffc00) == 0xd800;
    }

    static int is_low_surrogate(UTF16 uc) {
        return (uc & 0xfffffc00) == 0xdc00;
    }

    static wchar_t surrogate_to_utf32(UTF16 high, UTF16 low) {
        return (high << 10) + low - 0x35fdc00;
    }

    static wchar_t *w_str(kaddr str, size_t len) {
        wchar_t *output = new wchar_t[len + 1];

        UTF16 *source = ReadArr<UTF16>(str, len);

        for (int i = 0; i < len; i++) {
            const UTF16 uc = source[i];
            if (!is_surrogate(uc)) {
                output[i] = uc;
            } else {
                if (is_high_surrogate(uc) && is_low_surrogate(source[i]))
                    output[i] = surrogate_to_utf32(uc, source[i]);
                else
                    output[i] = L'?';
            }
        }

        output[len] = L'\0';
        return output;
    }

    static string getString(kaddr StrPtr, int StrLength) {
        wstring str = w_str(StrPtr, StrLength);

        string result(MAX_SIZE, '\0');

        wcstombs((char *) result.data(), str.c_str(), MAX_SIZE);

        return result;
    }
};

string GetFNameFromID(uint32 index) {
    if (isUE423) {
        uint32 Block = index >> 16;
        uint16 Offset = index & 65535;

        kaddr FNamePool = getRealOffset(Offsets::GNames) + Offsets::GNamesToFNamePool;

        kaddr NamePoolChunk = getPtr(
                FNamePool + Offsets::FNamePoolToBlocks + (Block * Offsets::PointerSize));
        kaddr FNameEntry = NamePoolChunk + (Offsets::FNameStride * Offset);

        int16 FNameEntryHeader = Read<int16>(FNameEntry);
        kaddr StrPtr = FNameEntry + Offsets::FNameEntryToString;
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryToLenBit;

        ///Unicode Dumping Not Supported Yet
        if (StrLength > 0 && StrLength < 250) {
            bool wide = FNameEntryHeader & 1;
            if (wide) {
                return WideStr::getString(StrPtr, StrLength);
            } else {
                return ReadStr2(StrPtr, StrLength);
            }
        } else {
            return "None";
        }
    } else {
        if (deRefGNames) {
            kaddr TNameEntryArray = getPtr(getRealOffset(Offsets::GNames));

            kaddr FNameEntryArr = getPtr(
                    TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
            kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));

            return ReadStr(FNameEntry + Offsets::FNameEntryToNameString, MAX_SIZE);
        } else {
            kaddr TNameEntryArray = getRealOffset(Offsets::GNames);

            kaddr FNameEntryArr = getPtr(
                    TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
            kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));

            return ReadStr(FNameEntry + Offsets::FNameEntryToNameString, MAX_SIZE);
        }
    }
}

void
DumpBlocks423(ofstream &gname, uint32 &count, kaddr FNamePool, uint32 blockId, uint32 blockSize) {
    kaddr It = getPtr(FNamePool + Offsets::FNamePoolToBlocks + (blockId * Offsets::PointerSize));
    kaddr End = It + blockSize - Offsets::FNameEntryToString;
    uint32 Block = blockId;
    uint16 Offset = 0;
    while (It < End) {
        kaddr FNameEntry = It;
        int16 FNameEntryHeader = Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryToLenBit;
        if (StrLength) {
            bool wide = FNameEntryHeader & 1;

            ///Unicode Dumping Not Supported
            if (StrLength > 0) {
                //String Length Limit
                if (StrLength < 250) {
                    string str;
                    uint32 key = (Block << 16 | Offset);
                    kaddr StrPtr = FNameEntry + Offsets::FNameEntryToString;

                    if (wide) {
                        str = WideStr::getString(StrPtr, StrLength);
                    } else {
                        str = ReadStr2(StrPtr, StrLength);
                    }

                    if (isVerbose) {
                        cout << (wide ? "Wide" : "") << dec << "{" << StrLength << "} [" << key << "]: " << str
                             << endl;
                    }

                    gname << "[" << key << "]: " << str << endl;
                    count++;
                }
            } else {
                StrLength = -StrLength;
            }

            //Next
            Offset += StrLength / Offsets::FNameStride;
            uint16 bytes = Offsets::FNameEntryToString +
                           StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            It += (bytes + Offsets::FNameStride - 1u) & ~(Offsets::FNameStride - 1u);
        } else {// Null-terminator entry found
            break;
        }
    }
}

void DumpStrings(string out) {
    uint32 count = 0;
    ofstream gname(out + "/Strings.txt", ofstream::out);
    if (gname.is_open()) {
        cout << "Dumping Strings" << endl;
        clock_t begin = clock();
        if (isUE423) {
            //cout << "String Dump for UE4.23+ Not Supported Yet" << endl;//No Longer Needed
            kaddr FNamePool = getRealOffset(Offsets::GNames) + Offsets::GNamesToFNamePool;

            uint32 BlockSize = Offsets::FNameStride * 65536;
            uint32 CurrentBlock = Read<uint32>(FNamePool + Offsets::FNamePoolToCurrentBlock);
            uint32 CurrentByteCursor = Read<uint32>(
                    FNamePool + Offsets::FNamePoolToCurrentByteCursor);

            //All Blocks Except Current
            for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx) {
                DumpBlocks423(gname, count, FNamePool, BlockIdx, BlockSize);
            }

            //Last Block
            DumpBlocks423(gname, count, FNamePool, CurrentBlock, CurrentByteCursor);
        } else {
            for (uint32 i = 0; i < GNameLimit; i++) {
                string s = GetFNameFromID(i);
                if (!s.empty()) {
                    gname << "[" << i << "]: " << s << endl;
                    count++;
                }
            }
        }
        gname.close();
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << count << " Strings Dumped in " << elapsed_secs << "S" << endl;
    }
}

#endif