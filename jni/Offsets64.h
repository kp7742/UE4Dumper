#ifndef OFFSETS64_H
#define OFFSETS64_H

#include "Mem.h"

namespace Offsets {
	kaddr GWorld = 0;
	kaddr GNames = 0;
	kaddr GUObjectArray = 0;

	enum Offsets {
        //Global
        PointerSize = 0x8,
        FUObjectItemSize = 0x18,

        //---------SDK-----------
        //Class: FNameEntry
        FNameEntryToNameString = 0xC,
        //Class: FUObjectArray
        FUObjectArrayToTUObjectArray = 0x10,
        //Class: TUObjectArray
        TUObjectArrayToNumElements = 0xC,
        TUObjectArrayToNumElementsNew = 0x14,
        //Class: UObject
        UObjectToInternalIndex = 0xC,
        UObjectToClassPrivate = 0x10,
        UObjectToFNameIndex = 0x18,
        UObjectToOuterPrivate = 0x20,
        //Class: UField
        UFieldToNext = 0x28,
        //Class: UStruct
        UStructToSuperStruct = 0x30,
        UStructToChildren = 0x38,
        //Class: UFunction
        UFunctionToFunctionFlags = 0x88,
        UFunctionToFunc = 0xB0,
        //Class: UProperty
        UPropertyToElementSize = 0x34,
        UPropertyToPropertyFlags = 0x38,
        UPropertyToOffsetInternal = 0x44,
        //Class: UBoolProperty
        UBoolPropertyToFieldSize = 0x70,
        UBoolPropertyToByteOffset = 0x71,
        UBoolPropertyToByteMask = 0x72,
        UBoolPropertyToFieldMask = 0x73,
        //Class: UObjectProperty
        UObjectPropertyToPropertyClass = 0x70,
        //Class: UClassProperty
        UClassPropertyToMetaClass = 0x78,
        //Class: UInterfaceProperty
        UInterfacePropertyToInterfaceClass = 0x78,
        //Class: UArrayProperty
        UArrayPropertyToInnerProperty = 0x70,
        //Class: UMapProperty
        UMapPropertyToKeyProp = 0x70,
        UMapPropertyToValueProp = 0x78,
        //Class: USetProperty
        USetPropertyToElementProp = 0x70,
        //Class: UStructProperty
        UStructPropertyToStruct = 0x70,
        //Class: UWorld
        UWorldToPersistentLevel = 0x30,
        //Class: ULevel
        ULevelToAActors = 0xA0,
        ULevelToAActorsCount = 0xA8,
	};
}

#endif
