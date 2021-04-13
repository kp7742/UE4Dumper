#ifndef GUOBJECTS_H
#define GUOBJECTS_H

#include "FNames.h"
#include "StructsSDK.h"

using namespace std;

int32 GetObjectCount(){
	if(isUE423) {
		return Read<int32>(getRealOffset(Offsets::GUObjectArray) +
						   Offsets::FUObjectArrayToTUObjectArray + Offsets::TUObjectArrayToNumElements);
	} else {
        if(deRefGUObjectArray){
			return Read<int32>(getPtr(getRealOffset(Offsets::GUObjectArray)) +
							   Offsets::FUObjectArrayToTUObjectArray + Offsets::TUObjectArrayToNumElements);
        } else {
			return Read<int32>(getRealOffset(Offsets::GUObjectArray) +
							   Offsets::FUObjectArrayToTUObjectArray + Offsets::TUObjectArrayToNumElements);
        }
	}
}

kaddr GetUObjectFromID(uint32 index) {
	if(isUE423) {
		kaddr TUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray) + Offsets::FUObjectArrayToTUObjectArray);
		kaddr Chunk = getPtr(TUObjectArray + ((index / 0x10000) * Offsets::PointerSize));

		return getPtr(Chunk + ((index % 0x10000) * Offsets::FUObjectItemSize));
	} else {
	    if(deRefGUObjectArray){
			kaddr FUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray));
			kaddr TUObjectArray = getPtr(FUObjectArray + Offsets::FUObjectArrayToTUObjectArray);

			return getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));
	    } else {
            kaddr FUObjectArray = getRealOffset(Offsets::GUObjectArray);
            kaddr TUObjectArray = getPtr(FUObjectArray + Offsets::FUObjectArrayToTUObjectArray);

            return getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));
	    }
	}
}

void DumpObjects(string out) {
	uint32 count = 0;
	ofstream obj(out + "/UObjects.txt", ofstream::out);
	if (obj.is_open()) {
		cout << "Dumping UObjects List" << endl;
		for (int32 i = 0; i < GetObjectCount(); i++) {
			kaddr uobj = GetUObjectFromID(i);
			if (UObject::isValid(uobj)) {
				obj << setbase(10) << "[" << i << "]:" << endl;
				obj << "Name: " << UObject::getName(uobj) << endl;
				obj << "Class: " << UStruct::getClassName(uobj) << endl;
				obj << "ObjectPtr: " << setbase(16) << uobj << endl;
				obj << "ClassPtr: " << setbase(16) << UObject::getClass(uobj) << endl;
				obj << endl;
				count++;
			}
		}
		obj.close();
		cout << count << " UObjects Dumped" << endl;
	}
}

#endif