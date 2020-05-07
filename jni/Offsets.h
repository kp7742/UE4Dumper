#ifndef OFFSETS_H
#define OFFSETS_H

#include "Mem.h"

namespace Offsets {
	kaddr GWorld = 0;
	kaddr GNames = 0;
	kaddr GUObjectArray = 0;

	enum Offsets {
		//Global
		PointerSize = 0x4,
		FUObjectItemSize = 0x10,

		//---------SDK-----------
		//Class: FNameEntry
		FNameEntryToNameString = 0x8,
		//Class: FUObjectArray
		FUObjectArrayToTUObjectArray = 0x10,
		//Class: UObject
		UObjectToClassPrivate = 0xC,
		UObjectToFNameindex = 0x10,
		//Class: UField
		UFieldToNext = 0x1C,
		//Class: UStruct
		UStructToSuperStruct = 0x20,
		UStructToChildren = 0x24,
		//Class: UFunction
		UFunctionToFunc = 0x74,
		//Class: UProperty
		UPropertyToElementSize = 0x24,
		UPropertyToOffsetInternal = 0x34,
		//Class: UBoolProperty
		UBoolPropertyToFieldSize = 0x50,
		UBoolPropertyToByteOffset = 0x51,
		UBoolPropertyToByteMask = 0x52,
		UBoolPropertyToFieldMask = 0x53,
		//Class: UObjectProperty
		UObjectPropertyToPropertyClass = 0x50,
		//Class: UClassProperty
		UClassPropertyToMetaClass = 0x54,
		//Class: UInterfaceProperty
		UInterfacePropertyToInterfaceClass = 0x54,
		//Class: UArrayProperty
		UArrayPropertyToInnerProperty = 0x50,
		//Class: UMapProperty
		UMapPropertyToKeyProp = 0x50,
		UMapPropertyToValueProp = 0x54,
		//Class: USetProperty
		USetPropertyToElementProp = 0x50,
		//Class: UStructProperty
		UStructPropertyToStruct = 0x50,
		//Class: UWorld
		UWorldToPersistentLevel = 0x20,
		//Class: ULevel
		ULevelToAActors = 0x70,
		ULevelToAActorsCount = 0x74,
	};
}

#endif
