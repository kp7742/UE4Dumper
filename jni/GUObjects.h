#ifndef GUOBJECTS_H
#define GUOBJECTS_H

#include "FNames.h"
#include "StructsSDK.h"

using namespace std;

int32 GetObjectCount(){
	if(isUE423) {
		return Read<int32>(getRealOffset(Offsets::GUObjectArray) +
						   Offsets::FUObjectArrayToTUObjectArray + Offsets::TUObjectArrayToNumElementsNew);
	} else {
        if(isEqual(pkg, "com.tencent.tmgp.pubgmhd")){
            return Read<int32>(getRealOffset(Offsets::GUObjectArray) + 0x58 + Offsets::TUObjectArrayToNumElements);
        } else {
            return Read<int32>(getPtr(getRealOffset(Offsets::GUObjectArray)) +
                               Offsets::FUObjectArrayToTUObjectArray + Offsets::TUObjectArrayToNumElements);
        }
	}
    //return 300000;
}

kaddr GetUObjectFromID(uint32 index) {
	if(isUE423) {
		kaddr TUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray) + Offsets::FUObjectArrayToTUObjectArray);
		kaddr Chunk = getPtr(TUObjectArray + ((index / 0x10000) * Offsets::PointerSize));

		return getPtr(Chunk + ((index % 0x10000) * Offsets::FUObjectItemSize));
	} else {
	    if(isEqual(pkg, "com.tencent.tmgp.pubgmhd")){
            kaddr FUObjectArray = getRealOffset(Offsets::GUObjectArray);
            kaddr TUObjectArray = getPtr(FUObjectArray + 0x58);

            return getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));
	    } else {
            kaddr FUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray));
            kaddr TUObjectArray = getPtr(FUObjectArray + Offsets::FUObjectArrayToTUObjectArray);

            return getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));
	    }
	}
}

void DumpObjects(string out) {
	uint32 count = 0;
	ofstream gobj(out + "/UObjects.txt", ofstream::out);
	if (gobj.is_open()) {
		cout << "Dumping UObjects List" << endl;
		for (int32 i = 0; i < GetObjectCount(); i++) {
			kaddr uobj = GetUObjectFromID(i);
			if (UObject::isValid(uobj)) {
				gobj << setbase(10) << "[" << i << "]:" << endl;
				gobj << "Name: " << UObject::getName(uobj) << endl;
				gobj << "Class: " << UStruct::getClassName(uobj) << endl;
				gobj << "ObjectPtr: " << setbase(16) << uobj << endl;
				gobj << "ClassPtr: " << setbase(16) << UObject::getClass(uobj) << endl;
				gobj << endl;
				count++;
			}
		}
		gobj.close();
		cout << count << " UObjects Dumped" << endl;
	}
}

#endif