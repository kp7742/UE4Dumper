#ifndef GUOBJECTS_H
#define GUOBJECTS_H

#include "FNames.h"
#include "StructsSDK.h"

using namespace std;

int32 GetObjectCount() {
    if (isUE423) {
        return Read<int32>(getRealOffset(Offsets::GUObjectArray) +
                           Offsets::FUObjectArrayToTUObjectArray +
                           Offsets::TUObjectArrayToNumElements);
    } else {
        if (deRefGUObjectArray) {
            return Read<int32>(getPtr(getRealOffset(Offsets::GUObjectArray)) +
                               Offsets::FUObjectArrayToTUObjectArray +
                               Offsets::TUObjectArrayToNumElements);
        } else {
            return Read<int32>(getRealOffset(Offsets::GUObjectArray) +
                               Offsets::FUObjectArrayToTUObjectArray +
                               Offsets::TUObjectArrayToNumElements);
        }
    }
}

kaddr GetUObjectFromID(uint32 index) {
    if (isUE423) {
        kaddr TUObjectArray = getPtr(
                getRealOffset(Offsets::GUObjectArray) + Offsets::FUObjectArrayToTUObjectArray);
        kaddr Chunk = getPtr(TUObjectArray + ((index / 0x10000) * Offsets::PointerSize));

        return getPtr(Chunk + Offsets::FUObjectItemPadd +
                      ((index % 0x10000) * Offsets::FUObjectItemSize));
    } else {
        kaddr FUObjectArray;
        if (deRefGUObjectArray) {
            FUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray));
        } else {
            FUObjectArray = getRealOffset(Offsets::GUObjectArray);
        }
        kaddr TUObjectArray = getPtr(FUObjectArray + Offsets::FUObjectArrayToTUObjectArray);

        return getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));
    }
}

void DumpObjects(string out) {
    uint32 count = 0;
    ofstream obj(out + "/Objects.txt", ofstream::out);
    if (obj.is_open()) {
        cout << "Dumping Objects List: " << GetObjectCount() << endl;
        clock_t begin = clock();
        int32 ocount = GetObjectCount();
        cout << "Objects Counts: " << setbase(10) << ocount << endl;
        if (ocount < 10 || ocount > 999999) {
            ocount = 300000;
        }
        for (int32 i = 0; i < ocount; i++) {
            kaddr uobj = GetUObjectFromID(i);
            if (UObject::isValid(uobj)) {
                if (isVerbose) {
                    cout << setbase(16) << "[0x" << i << "]: " << UObject::getName(uobj) << endl;
                }
                obj << setbase(16) << "[0x" << i << "]:" << endl;
                obj << "Name: " << UObject::getName(uobj).c_str() << endl;
                obj << "Class: " << UObject::getClassName(uobj).c_str() << endl;
                obj << "ObjectPtr: 0x" << setbase(16) << uobj << endl;
                obj << "ClassPtr: 0x" << setbase(16) << UObject::getClass(uobj) << endl;
                obj << endl;
                count++;
            }
        }
        obj.close();
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << setbase(10) << count << " Valid Objects Dumped in " << elapsed_secs << "S" << endl;
    }
}

#endif