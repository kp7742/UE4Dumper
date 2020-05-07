#ifndef GUOBJECTS_H
#define GUOBJECTS_H

#include "FNames.h"
#include "StructsSDK.h"

using namespace std;

uint32 GUObjectCount = 300000;

UObject GetUObjectFromID(uint32 index) {
	kaddr FUObjectArray = getPtr(getRealOffset(Offsets::GUObjectArray));
	kaddr TUObjectArray = getPtr(FUObjectArray + Offsets::FUObjectArrayToTUObjectArray);
	kaddr FUObjectItem = getPtr(TUObjectArray + (index * Offsets::FUObjectItemSize));

	return UObject(FUObjectItem);
}

void DumpObjects(string out) {
	uint32 count = 0;
	ofstream gobj(out + "/UObjects.txt", ofstream::out);
	if (gobj.is_open()) {
		cout << "Dumping UObjects List" << endl;
		for (uint32 i = 0; i < GUObjectCount; i++) {
			UObject uobj = GetUObjectFromID(i);
			if (uobj.isValid() && uobj.getName().size() > 1) {
				gobj << setbase(10) << "[" << i << "]:" << endl;
				gobj << "Name: " << uobj.getName() << endl;
				gobj << "ObjectPtr: " << setbase(16) << uobj.ptr << endl;
				gobj << "ClassPtr: " << setbase(16) << uobj.ClassPrivate << endl;
				gobj << endl;
				count++;
			}
		}
		gobj.close();
		cout << count << " UObjects Dumped" << endl;
	}
}

#endif