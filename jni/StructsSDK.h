#ifndef STRUCTSSDK_H
#define STRUCTSSDK_H

#include "FNames.h"

using namespace std;

//--------------SDK-----------------//

struct UObject {
    static int32 getIndex(kaddr object) {
        return Read<int32>(object + Offsets::UObjectToInternalIndex);
    }

    static kaddr getClass(kaddr object) {//UClass*
        return getPtr(object + Offsets::UObjectToClassPrivate);
    }

    static uint32 getNameID(kaddr object) {
        return Read<uint32>(object + Offsets::UObjectToFNameIndex);
    }

    static kaddr getOuter(kaddr object) {//UObject*
        return getPtr(object + Offsets::UObjectToOuterPrivate);
    }

    static string getName(kaddr object) {
        return GetFNameFromID(getNameID(object));
    }

    static string getClassName(kaddr object) {
        return getName(getClass(object));
    }

    static bool isValid(kaddr object) {
        return (object != 0 && getNameID(object) != 0 && getClass(object) != 0);
    }
};

struct UField {
    static kaddr getNext(kaddr field) {//UField*
        return getPtr(field + Offsets::UFieldToNext);
    }
};

struct FField {
    static string getName(kaddr fField) {
        return GetFNameFromID(Read<uint32>(fField + Offsets::FFieldToName));
    }

    static string getClassName(kaddr fField) {
        return GetFNameFromID(Read<uint32>(getPtr(fField + Offsets::FFieldToClass)));
    }

    static kaddr getNext(kaddr fField) {//UField*
        return getPtr(fField + Offsets::FFieldToNext);
    }
};

struct UStruct {
    static kaddr getSuperClass(kaddr structz) {//UStruct*
        return getPtr(structz + Offsets::UStructToSuperStruct);
    }

    static kaddr getChildren(kaddr structz) {//UField*
        return getPtr(structz + Offsets::UStructToChildren);
    }

    static kaddr getChildProperties(kaddr structz) {//UField*
        return getPtr(structz + Offsets::UStructToChildProperties);
    }

    static string getClassName(kaddr clazz) {
        return UObject::getName(clazz);
    }

    static string getClassPath(kaddr object) {
        kaddr clazz = UObject::getClass(object);
        string classname = UObject::getName(clazz);

        kaddr superclass = getSuperClass(clazz);
        while (superclass) {
            classname += ".";
            classname += UObject::getName(superclass);

            superclass = getSuperClass(superclass);
        }

        return classname;
    }

    static string getStructClassPath(kaddr clazz) {
        string classname = UObject::getName(clazz);

        kaddr superclass = getSuperClass(clazz);
        while (superclass) {
            classname += ".";
            classname += UObject::getName(superclass);

            superclass = getSuperClass(superclass);
        }

        return classname;
    }

    static bool isValid(kaddr clazz) {
        return (clazz > 0 && UObject::getNameID(clazz) > 0 && UObject::getClass(clazz) > 0);
    }
};

struct UFunction {
    static int32 getFunctionFlags(kaddr func) {
        return Read<int32>(func + Offsets::UFunctionToFunctionFlags);
    }

    static kaddr getFunc(kaddr func) {
        return getPtr(func + Offsets::UFunctionToFunc);
    }
};

struct UProperty {
    static int32 getElementSize(kaddr prop) {
        return Read<int32>(prop + Offsets::UPropertyToElementSize);
    }

    static uint64 getPropertyFlags(kaddr prop) {
        return Read<uint64>(prop + Offsets::UPropertyToPropertyFlags);
    }

    static int32 getOffset(kaddr prop) {
        return Read<int32>(prop + Offsets::UPropertyToOffsetInternal);
    }
};

struct UBoolProperty {
    static uint8 getFieldSize(kaddr prop) {
        return Read<uint8>(prop + Offsets::UBoolPropertyToFieldSize);
    }

    static uint8 getByteOffset(kaddr prop) {
        return Read<uint8>(prop + Offsets::UBoolPropertyToByteOffset);
    }

    static uint8 getByteMask(kaddr prop) {
        return Read<uint8>(prop + Offsets::UBoolPropertyToByteMask);
    }

    static uint8 getFieldMask(kaddr prop) {
        return Read<uint8>(prop + Offsets::UBoolPropertyToFieldMask);
    }
};

struct UObjectProperty {
    static kaddr getPropertyClass(kaddr prop) {//class UClass*
        return getPtr(prop + Offsets::UObjectPropertyToPropertyClass);
    }
};

struct UClassProperty {
    static kaddr getMetaClass(kaddr prop) {//class UClass*
        return getPtr(prop + Offsets::UClassPropertyToMetaClass);
    }
};

struct UInterfaceProperty {
    static kaddr getInterfaceClass(kaddr prop) {//class UClass*
        return getPtr(prop + Offsets::UInterfacePropertyToInterfaceClass);
    }
};

struct UArrayProperty {
    static kaddr getInner(kaddr prop) {//UProperty*
        return getPtr(prop + Offsets::UArrayPropertyToInnerProperty);
    }
};

struct UMapProperty {
    static kaddr getKeyProp(kaddr prop) {//UProperty*
        return getPtr(prop + Offsets::UMapPropertyToKeyProp);
    }

    static kaddr getValueProp(kaddr prop) {//UProperty*
        return getPtr(prop + Offsets::UMapPropertyToValueProp);
    }
};

struct USetProperty {
    static kaddr getElementProp(kaddr prop) {//UProperty*
        return getPtr(prop + Offsets::USetPropertyToElementProp);
    }
};

struct UStructProperty {
    static kaddr getStruct(kaddr prop) {//UStruct*
        return getPtr(prop + Offsets::UStructPropertyToStruct);
    }
};

#endif
