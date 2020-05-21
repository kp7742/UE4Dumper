#ifndef STRUCTSSDK_H
#define STRUCTSSDK_H

#include "FNames.h"

using namespace std;

//--------------SDK-----------------//

struct Base {
	kaddr ptr;

	Base(kaddr address) {
		if (address) {
			ptr = address;
		}
	}

	bool isValid(){
		return ptr > 0;
	}
};

struct UObject : Base {
	kaddr ClassPrivate;
	uint32 FNameID;

	UObject(kaddr address) : Base(address) {
		if (isValid()) {
			ClassPrivate = getPtr(address + Offsets::UObjectToClassPrivate);
			FNameID = Read<uint32>(address + Offsets::UObjectToFNameindex);
		}
	}

	string getName() {
		return GetFNameFromID(FNameID);
	}
};

struct UField : UObject {
	kaddr Next;//UField*

	UField(kaddr address) : UObject(address){
		if (isValid()) {
			Next = getPtr(address + Offsets::UFieldToNext);
		}
	}
};

struct UStruct : UField {
	kaddr SuperStruct;//UStruct*
	kaddr Children;//UField*

	UStruct(kaddr address) : UField(address) {
		if (isValid()) {
			SuperStruct = getPtr(address + Offsets::UStructToSuperStruct);
			Children = getPtr(address + Offsets::UStructToChildren);
		}
	}

	string getClassName() {
		return getName();
	}

	string getClassPath() {
		string classname = getName();

		kaddr superclass = SuperStruct;
		while (superclass) {
			UStruct clazz(superclass);

			classname += ".";
			classname += clazz.getName();
			
			superclass = clazz.SuperStruct;
		}

		return classname;
	}
};

struct UClass : UStruct {
	UClass(kaddr address) : UStruct(address) {}
};

struct UFunction : UStruct {
    int32 FunctionFlags;
    kaddr Func;

    UFunction(kaddr address) : UStruct(address) {
        if (isValid()) {
            FunctionFlags = Read<int32>(address + Offsets::UFunctionToFunctionFlags);
            Func = getPtr(address + Offsets::UFunctionToFunc);
        }
    }
};

struct UProperty : UField {
	int32 ElementSize;
    uint64 PropertyFlags;
	int32 Offset;

	UProperty(kaddr address) : UField(address) {
		if (isValid()) {
			ElementSize = Read<int32>(address + Offsets::UPropertyToElementSize);
            PropertyFlags = Read<uint64>(address + Offsets::UPropertyToPropertyFlags);
			Offset = Read<int32>(address + Offsets::UPropertyToOffsetInternal);
		}
	}
};

struct UBoolProperty : UProperty {
	/** Size of the bitfield/bool property. Equal to ElementSize but used to check if the property has been properly initialized (0-8, where 0 means uninitialized). */
	uint8 FieldSize;
	/** Offset from the memeber variable to the byte of the property (0-7). */
	uint8 ByteOffset;
	/** Mask of the byte byte with the property value. */
	uint8 ByteMask;
	/** Mask of the field with the property value. Either equal to ByteMask or 255 in case of 'bool' type. */
	uint8 FieldMask;

	UBoolProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			FieldSize = Read<uint8>(address + Offsets::UBoolPropertyToFieldSize);
			ByteOffset = Read<uint8>(address + Offsets::UBoolPropertyToByteOffset);
			ByteMask = Read<uint8>(address + Offsets::UBoolPropertyToByteMask);
			FieldMask = Read<uint8>(address + Offsets::UBoolPropertyToFieldMask);
		}
	}
};

struct UObjectProperty : UProperty {
	kaddr PropertyClass;//class UClass*

	UObjectProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			PropertyClass = getPtr(address + Offsets::UObjectPropertyToPropertyClass);
		}
	}
};

struct UClassProperty : UProperty {
	kaddr MetaClass;//class UClass*

	UClassProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			MetaClass = getPtr(address + Offsets::UClassPropertyToMetaClass);
		}
	}
};

struct UInterfaceProperty : UProperty {
	kaddr InterfaceClass;//class UClass*

	UInterfaceProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			InterfaceClass = getPtr(address + Offsets::UInterfacePropertyToInterfaceClass);
		}
	}
};

struct UArrayProperty : UProperty {
	kaddr Inner;//UProperty*

	UArrayProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			Inner = getPtr(address + Offsets::UArrayPropertyToInnerProperty);
		}
	}
};

struct UMapProperty : UProperty {
	kaddr KeyProp;//UProperty*
	kaddr ValueProp;//UProperty*

	UMapProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			KeyProp = getPtr(address + Offsets::UMapPropertyToKeyProp);
			ValueProp = getPtr(address + Offsets::UMapPropertyToValueProp);
		}
	}
};

struct USetProperty : UProperty {
	kaddr ElementProp;//UProperty*

	USetProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			ElementProp = getPtr(address + Offsets::USetPropertyToElementProp);
		}
	}
};

struct UStructProperty : UProperty {
	kaddr Struct;//UStruct*

	UStructProperty(kaddr address) : UProperty(address) {
		if (isValid()) {
			Struct = getPtr(address + Offsets::UStructPropertyToStruct);
		}
	}
};

#endif
