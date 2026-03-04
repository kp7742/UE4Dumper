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
        auto *output = new wchar_t[len + 1];

        auto *source = ReadArr<UTF16>(str, len);

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
        wchar_t *str = w_str(StrPtr, StrLength);
        //
        // string result(MAX_SIZE, '\0');
        //
        // wcstombs((char *) result.data(), str, MAX_SIZE);
        //
        // free(str);
        //
        // return result;
        // auto *source = ReadArr<char16_t>(StrPtr, StrLength);
        //
        // std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        // return convert.to_bytes(std::u16string(source, source + StrLength));
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
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

        auto FNameEntryHeader = Read<int16>(FNameEntry);
        kaddr StrPtr = FNameEntry + Offsets::FNameEntryToString;
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryToLenBit;

        ///Unicode Dumping Not Supported Yet
        if (StrLength > 0 && StrLength < 250) {
            bool wide = FNameEntryHeader & 1;
            if (wide) {
                return WideStr::getString(StrPtr, StrLength);
            } else {
                return ReadStr(StrPtr, StrLength);
            }
        } else {
            return "None";
        }
    } else {
        static kaddr TNameEntryArray;

        if(TNameEntryArray){
            goto gotGName;
        }

        if (isPtrDec && !isPUBGCN) {
            if(isPGLite) {
                auto modeSel = Read<uint32>(getRealOffset(Offsets::PGLEncSelect));
                if (modeSel) {
                    kaddr blockSlice = getPtr(getRealOffset(Offsets::PGLBlockSlice1));
                    if (blockSlice) {
                        kaddr block = getPtr(blockSlice + (Offsets::PointerSize * 5));

                        auto shift = Read<uint8>(getRealOffset(Offsets::PGLBlockShift));
                        kaddr offset = Offsets::PointerSize * (shift + 5);

                        kaddr encGName = getPtr(block + offset);

#if defined(__LP64__)
                        TNameEntryArray = encGName ^ 0x7878787878787878;
#else
                        TNameEntryArray = encGName ^ 0x78787878;
#endif
                    } else {
                        return "None";
                    }
                } else {
                    kaddr blockSlice = getRealOffset(Offsets::PGLBlockSlice2);
                    if (blockSlice && Read<int>(blockSlice + 0x4)) {
                        kaddr block = getPtr(blockSlice + 0x8);

                        auto shift = Read<uint32>(blockSlice);
                        uint32 offset = (Offsets::PointerSize * 2) * (((shift - 0x64) / 0x3) - 1);

                        TNameEntryArray = getPtr(block + offset);
                    } else {
                        return "None";
                    }
                }
            } else {
                kaddr blockSlice = getRealOffset(Offsets::GNames);
                if (blockSlice) {
                    kaddr block = getPtr(blockSlice + 0x8);

                    auto shift = Read<uint32>(blockSlice);
                    uint32 offset = (Offsets::PointerSize * 2) * (((shift - 0x64) / 0x3) - 1);

                    TNameEntryArray = getPtr(block + offset);
                } else {
                    return "None";
                }
            }
        } else {
            if (deRefGNames) {
                TNameEntryArray = getPtr(getRealOffset(Offsets::GNames));
            } else {
                TNameEntryArray = getRealOffset(Offsets::GNames);
            }
        }

gotGName:
        kaddr FNameEntryArr = getPtr(
                TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
        kaddr FNameEntry = getPtr(
                FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));

        return ReadStr(FNameEntry + Offsets::FNameEntryToNameString, 64);
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
        auto FNameEntryHeader = Read<int16>(FNameEntry);
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
                        str = ReadStr(StrPtr, StrLength);
                    }

                    if (isVerbose) {
                        cout << (wide ? "Wide" : "") << dec << "{" << StrLength << "} " << hex
                             << "[" << key << "]: " << str
                             << endl;
                    }

                    gname << (wide ? "Wide" : "") << dec << "{" << StrLength << "} " << hex << "["
                          << key << "]: " << str << endl;
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

void DumpStrings(const string& out) {
    uint32 count = 0;
    ofstream gname(out + "/Strings.txt", ofstream::out);
    if (gname.is_open()) {
        cout << "Dumping Strings" << endl;
        clock_t begin = clock();
        if (isUE423) {
            kaddr FNamePool = getRealOffset(Offsets::GNames) + Offsets::GNamesToFNamePool;

            uint32 BlockSize = Offsets::FNameStride * 65536;
            auto CurrentBlock = Read<uint32>(FNamePool + Offsets::FNamePoolToCurrentBlock);
            auto CurrentByteCursor = Read<uint32>(
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
                    if (isVerbose) {
                        cout << "[" << i << "]: " << s << endl;
                    }
                    count++;
                }
            }
        }
        gname.close();
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << count << " Strings Dumped in " << elapsed_secs << "S" << endl;
    }

// uint32 index = 2;
//
// kaddr TNameEntryArray = getPtr(getRealOffset(Offsets::GNames));
// printf("TNameEntryArray: %llx\n", (uint64) TNameEntryArray);
//
// HexDump(TNameEntryArray, 10);
//
// kaddr FNameEntryArr = getPtr(TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
// printf("FNameEntryArr: %llx\n", (uint64) FNameEntryArr);
//
// HexDump(FNameEntryArr, 10);
//
// kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));
// printf("FNameEntry: %llx\n", (uint64) FNameEntry);
//
// HexDump(FNameEntry, 10);
//
// printf("Name: %s", ReadStr(FNameEntry + Offsets::FNameEntryToNameString, MAX_SIZE).c_str());

//    uint32 modeSel = Read<uint32>(getRealOffset(Offsets::PGLEncSelect));
//    printf("ModeSel: %x\n\n", modeSel);
//
//    if(modeSel) {
//        printf("XOR Pointer Method:-\n");
//
//        kaddr blockSlice = getPtr(getRealOffset(Offsets::PGLBlockSlice1));
//        printf("BlockSlice: %lx\n", blockSlice);
//
//        if(blockSlice){
//            kaddr block = getPtr(blockSlice + (Offsets::PointerSize * 5));
//            printf("Block: %lx\n", block);
//
//            uint8 shift = Read<uint8>(getRealOffset(Offsets::PGLBlockShift));
//            printf("Shift: %d\n", shift);
//
//            kaddr offset = Offsets::PointerSize * (shift + 5);
//            printf("Offset: %lx\n", offset);
//
//            kaddr encGName = getPtr(block + offset);
//            printf("EncGName: %lx\n", encGName);
//
//            int index = 1;
//
//#if defined(__LP64__)
//            kaddr TNameEntryArray = encGName ^ 0x7878787878787878;
//#else
//            kaddr TNameEntryArray = encGName ^ 0x78787878;
//#endif
//
//            kaddr FNameEntryArr = getPtr(TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
//            kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));
//
//            printf("\nTNameEntryArray: %lx\n", TNameEntryArray);
//            printf("FNameEntryArr: %lx\n", FNameEntryArr);
//            printf("FNameEntry: %lx\n", FNameEntry);
//
//            string s = ReadStr(FNameEntry + Offsets::FNameEntryToNameString, MAX_SIZE);
//
//            printf("ReadStr: %s\n", s.c_str());
//        }
//    } else {
//        printf("NOP Chain Method:-\n");
//
//        kaddr blockSlice = getRealOffset(Offsets::PGLBlockSlice2);
//        printf("BlockSlice: %lx\n", blockSlice);
//
//        if(Read<int>(blockSlice + 0x4)){
//            kaddr block = getPtr(blockSlice + 0x8);
//            printf("Block: %lx\n", block);
//
//            uint32 shift = Read<uint32>(blockSlice);
//            printf("Shift: %x\n", shift);
//
//            uint32 offset = (Offsets::PointerSize * 2) * (((shift - 0x64) / 0x3) - 1);
//            printf("Offset: %x\n", offset);
//
//            int index = 1;
//
//            kaddr TNameEntryArray = getPtr(block + offset);
//
//            kaddr FNameEntryArr = getPtr(TNameEntryArray + ((index / 0x4000) * Offsets::PointerSize));
//            kaddr FNameEntry = getPtr(FNameEntryArr + ((index % 0x4000) * Offsets::PointerSize));
//
//            printf("\nTNameEntryArray: %lx\n", TNameEntryArray);
//            printf("FNameEntryArr: %lx\n", FNameEntryArr);
//            printf("FNameEntry: %lx\n", FNameEntry);
//
//            string s = ReadStr(FNameEntry + Offsets::FNameEntryToNameString, MAX_SIZE);
//
//            printf("ReadStr: %s\n", s.c_str());
//        }
//    }
}

#endif