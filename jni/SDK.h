#ifndef SDK_H
#define SDK_H

#include "StructsSDK.h"
#include "FNames.h"
#include "GUObjects.h"

using namespace std;

uint32 objcount = 0;

vector<uint32> writtenmap;

bool isMapped(uint32 data) {
	for (int i = 0; i < writtenmap.size(); i++) {
		if (writtenmap[i] == data) { return true; }
	}
	return false;
}

void writeStructs(ofstream& sdk, kaddr clazz);

string getFunctionParamType(kaddr prop){
    if(prop){
        UProperty innerprop(prop);
        UClass innerclass(innerprop.ClassPrivate);

        string cname = innerclass.getClassName();

        if (isEqual(cname, "ObjectProperty") || isEqual(cname, "WeakObjectProperty") || isEqual(cname, "LazyObjectProperty")
            || isEqual(cname, "AssetObjectProperty") || isEqual(cname, "SoftObjectProperty")) {
            UObjectProperty objectProp(prop);
            UClass propertyClass(objectProp.PropertyClass);
            return propertyClass.getName() + "*";
        }
        else if (isEqual(cname, "ClassProperty") || isEqual(cname, "AssetClassProperty") || isEqual(cname, "SoftClassProperty")) {
            UClassProperty classProp(prop);
            UClass metaClass(classProp.MetaClass);
            return "class " + metaClass.getName();
        }
        else if (isEqual(cname, "InterfaceProperty")) {
            UInterfaceProperty interfaceProp(prop);
            UClass interfaceClass(interfaceProp.InterfaceClass);
            return "interface class" + interfaceClass.getName();
        }
        else if (isEqual(cname, "StructProperty")) {
            UStructProperty structProp(prop);
            UStruct Struct(structProp.Struct);
            return Struct.getName();
        }
        else if (isEqual(cname, "ArrayProperty")) {
            UArrayProperty arrProp(prop);
            return getFunctionParamType(arrProp.Inner) + "[]";
        }
        else if (isEqual(cname, "SetProperty")) {
            USetProperty setProp(prop);
            return "<" + getFunctionParamType(setProp.ElementProp) + ">";
        }
        else if (isEqual(cname, "MapProperty")) {
            UMapProperty mapProp(prop);
            return "<" + getFunctionParamType(mapProp.KeyProp) + "," + getFunctionParamType(mapProp.ValueProp) + ">";
        }
        else if (isEqual(cname, "BoolProperty")) {
            return "bool";
        }
        else if (isEqual(cname, "ByteProperty")) {
            return "byte";
        }
        else if (isEqual(cname, "IntProperty")) {
            return "int";
        }
        else if (isEqual(cname, "Int8Property")) {
            return "int8";
        }
        else if (isEqual(cname, "Int16Property")) {
            return "int16";
        }
        else if (isEqual(cname, "Int64Property")) {
            return "int64";
        }
        else if (isEqual(cname, "UInt16Property")) {
            return "uint16";
        }
        else if (isEqual(cname, "UInt32Property")) {
            return "uint32";
        }
        else if (isEqual(cname, "UInt64Property")) {
            return "uint64";
        }
        else if (isEqual(cname, "DoubleProperty")) {
            return "double";
        }
        else if (isEqual(cname, "FloatProperty")) {
            return "float";
        }
        else if (isEqual(cname, "EnumProperty")) {
            return "enum";
        }
        else if (isEqual(cname, "StrProperty")) {
            return "FString";
        }
        else if (isEqual(cname, "TextProperty")) {
            return "FText";
        }
        else if (isEqual(cname, "NameProperty")) {
            return "FName";
        }
        else if (isEqual(cname, "DelegateProperty") || isEqual(cname, "MulticastDelegateProperty")) {
            return "delegate";
        }
        else {
            return innerprop.getName() + "(" + innerclass.getName() + ")";
        }
    }
    return "NULL";
}

//---------------------------------------------------------------------------------------------------------------------------//

string resolveInnerProp(list<kaddr>& recurrce, kaddr prop){
    if(prop){
        UProperty innerprop(prop);
        UClass innerclass(innerprop.ClassPrivate);

        string cname = innerclass.getClassName();

        if (isEqual(cname, "ObjectProperty") || isEqual(cname, "WeakObjectProperty") || isEqual(cname, "LazyObjectProperty")
            || isEqual(cname, "AssetObjectProperty") || isEqual(cname, "SoftObjectProperty")) {
            UObjectProperty objectProp(prop);
            UClass propertyClass(objectProp.PropertyClass);
            recurrce.push_back(propertyClass.ptr);
            return propertyClass.getName() + "*";
        }
        else if (isEqual(cname, "ClassProperty") || isEqual(cname, "AssetClassProperty") || isEqual(cname, "SoftClassProperty")) {
            UClassProperty classProp(prop);
            UClass metaClass(classProp.MetaClass);
            recurrce.push_back(metaClass.ptr);
            return "class " + metaClass.getName();
        }
        else if (isEqual(cname, "InterfaceProperty")) {
            UInterfaceProperty interfaceProp(prop);
            UClass interfaceClass(interfaceProp.InterfaceClass);
            recurrce.push_back(interfaceClass.ptr);
            return "interface class" + interfaceClass.getName();
        }
        else if (isEqual(cname, "StructProperty")) {
            UStructProperty structProp(prop);
            UStruct Struct(structProp.Struct);
            recurrce.push_back(Struct.ptr);
            return Struct.getName();
        }
        else if (isEqual(cname, "ArrayProperty")) {
            UArrayProperty arrProp(prop);
            return resolveInnerProp(recurrce, arrProp.Inner) + "[]";
        }
        else if (isEqual(cname, "SetProperty")) {
            USetProperty setProp(prop);
            return "<" + resolveInnerProp(recurrce, setProp.ElementProp) + ">";
        }
        else if (isEqual(cname, "MapProperty")) {
            UMapProperty mapProp(prop);
            return "<" + resolveInnerProp(recurrce, mapProp.KeyProp) + "," + resolveInnerProp(recurrce, mapProp.ValueProp) + ">";
        }
        else if (isEqual(cname, "BoolProperty")) {
            return "bool";
        }
        else if (isEqual(cname, "ByteProperty")) {
            return "byte";
        }
        else if (isEqual(cname, "IntProperty")) {
            return "int";
        }
        else if (isEqual(cname, "Int8Property")) {
            return "int8";
        }
        else if (isEqual(cname, "Int16Property")) {
            return "int16";
        }
        else if (isEqual(cname, "Int64Property")) {
            return "int64";
        }
        else if (isEqual(cname, "UInt16Property")) {
            return "uint16";
        }
        else if (isEqual(cname, "UInt32Property")) {
            return "uint32";
        }
        else if (isEqual(cname, "UInt64Property")) {
            return "uint64";
        }
        else if (isEqual(cname, "DoubleProperty")) {
            return "double";
        }
        else if (isEqual(cname, "FloatProperty")) {
            return "float";
        }
        else if (isEqual(cname, "EnumProperty")) {
            return "enum";
        }
        else if (isEqual(cname, "StrProperty")) {
            return "FString";
        }
        else if (isEqual(cname, "TextProperty")) {
            return "FText";
        }
        else if (isEqual(cname, "NameProperty")) {
            return "FName";
        }
        else if (isEqual(cname, "DelegateProperty") || isEqual(cname, "MulticastDelegateProperty")) {
            return "delegate";
        }
        else {
            return innerprop.getName() + "(" + innerclass.getName() + ")";
        }
    }
    return "NULL";
}

//---------------------------------------------------------------------------------------------------------------------------//

list<kaddr> writeStructChildrens(ofstream& sdk, kaddr childprop) {
    list<kaddr> recurrce;
	kaddr child = childprop;
	while (child) {
		UField field(child);
		UClass fclass(field.ClassPrivate);

		string oname = field.getName();
		string cname = fclass.getClassName();

        if (isEqual(oname, "None") || isEqual(cname, "None")) {
            break;
        }

		if (isEqual(cname, "ObjectProperty") || isEqual(cname, "WeakObjectProperty") || isEqual(cname, "LazyObjectProperty")
			|| isEqual(cname, "AssetObjectProperty") || isEqual(cname, "SoftObjectProperty")) {

			UObjectProperty objectProp(child);
			UClass propertyClass(objectProp.PropertyClass);

			sdk << "\t" << propertyClass.getName() << "* " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << objectProp.Offset << " , " << "Size: " << setbase(10) << objectProp.ElementSize << "]" << endl;
			
			recurrce.push_back(propertyClass.ptr);
		}
		else if (isEqual(cname, "ClassProperty") || isEqual(cname, "AssetClassProperty") || isEqual(cname, "SoftClassProperty")) {
			UClassProperty classProp(child);
			UClass metaClass(classProp.MetaClass);

			sdk << "\tclass " << metaClass.getName() << "* " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << classProp.Offset << " , " << "Size: " << setbase(10) << classProp.ElementSize << "]" << endl;
		
			recurrce.push_back(metaClass.ptr);
		}
		else if (isEqual(cname, "InterfaceProperty")) {
			UInterfaceProperty interfaceProp(child);
			UClass interfaceClass(interfaceProp.InterfaceClass);

			sdk << "\tinterface class " << interfaceClass.getName() << "* " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << interfaceProp.Offset << " , " << "Size: " << setbase(10) << interfaceProp.ElementSize << "]" << endl;

			//recurrce.push_back(interfaceClass.ptr);
		}
		else if (isEqual(cname, "StructProperty")) {
			UStructProperty structProp(child);
			UStruct Struct(structProp.Struct);

			sdk << "\t" << Struct.getName() << " " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << structProp.Offset << " , " << "Size: " << setbase(10) << structProp.ElementSize << "]" << endl;

			recurrce.push_back(Struct.ptr);
		}
		else if (isEqual(cname, "ArrayProperty")) {
			UArrayProperty arrProp(child);

            sdk << "\t" << resolveInnerProp(recurrce, arrProp.Inner) <<"[] " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << arrProp.Offset << " , " << "Size: " << setbase(10) << arrProp.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "SetProperty")) {
			USetProperty setProp(child);

			sdk << "\t<" << resolveInnerProp(recurrce, setProp.ElementProp) << "> " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << setProp.Offset << " , " << "Size: " << setbase(10) << setProp.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "MapProperty")) {
			UMapProperty mapProp(child);

			sdk << "\t<" << resolveInnerProp(recurrce, mapProp.KeyProp) << ",";
			sdk << resolveInnerProp(recurrce, mapProp.ValueProp) << "> " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << mapProp.Offset << " , " << "Size: " << setbase(10) << mapProp.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "BoolProperty")) {
			UBoolProperty prop(child);

			sdk << "\tbool " << oname << ";";
			sdk << setbase(10) <<"//(ByteOffset: " << (int)prop.ByteOffset << ", ByteMask: " << (int)prop.ByteMask << ", FieldMask: " << (int)prop.FieldMask << ")";
			sdk << "[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "ByteProperty")) {
			UProperty prop(child);

			sdk << "\tbyte " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "IntProperty")) {
			UProperty prop(child);

			sdk << "\tint " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "Int8Property")) {
			UProperty prop(child);

			sdk << "\tint8 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "Int16Property")) {
			UProperty prop(child);

			sdk << "\tint16 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "Int64Property")) {
			UProperty prop(child);

			sdk << "\tint64 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "UInt16Property")) {
			UProperty prop(child);

			sdk << "\tuint16 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "UInt32Property")) {
			UProperty prop(child);

			sdk << "\tuint32 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "UInt64Property")) {
			UProperty prop(child);

			sdk << "\tuint64 " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "DoubleProperty")) {
			UProperty prop(child);

			sdk << "\tdouble " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "FloatProperty")) {
			UProperty prop(child);

			sdk << "\tfloat " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "EnumProperty")) {
			UProperty prop(child);

			sdk << "\tenum " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "StrProperty")) {
			UProperty prop(child);

			sdk << "\tFString " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "TextProperty")) {
			UProperty prop(child);

			sdk << "\tFText " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "NameProperty")) {
			UProperty prop(child);

			sdk << "\tFName " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isEqual(cname, "DelegateProperty") || isEqual(cname, "MulticastDelegateProperty")) {
			UProperty prop(child);

			sdk << "\tdelegate " << oname << ";";
			sdk << "//[Offset: 0x" << setbase(16) << prop.Offset << " , " << "Size: " << setbase(10) << prop.ElementSize << "]" << endl;
		}
		else if (isStartWith(cname, "Function") || isEqual(cname, "DelegateFunction")) {
			UFunction func(child);

            string returnVal = "void";
            string params = "";

            UStruct Func = UStruct(child);
            kaddr funcParam = Func.Children;
            while (funcParam) {
                UProperty param(funcParam);

                if((param.PropertyFlags & 0x0000000000000400) == 0x0000000000000400){
                    returnVal = getFunctionParamType(funcParam);
                } else {
                    if((param.PropertyFlags & 0x0000000000000100) == 0x0000000000000100){
                        params += "out ";
                    }
                    /*if((param.PropertyFlags & 0x0000000008000000) == 0x0000000008000000){
                        params += "ref ";
                    }*/
                    if((param.PropertyFlags & 0x0000000000000002) == 0x0000000000000002){
                        params += "const ";
                    }
                    params += getFunctionParamType(funcParam);
                    params += " ";
                    params += param.getName();
                    params += ", ";
                }

                funcParam = param.Next;
            }

            if(!params.empty()){
                params.pop_back();
                params.pop_back();
            }

            sdk << "\t";

            if((func.FunctionFlags & 0x00002000) == 0x00002000){
                sdk << "static" << " ";
            }
            /*if((func.FunctionFlags & 0x00000001) == 0x00000001){
                sdk << "final" << " ";
            }
            if((func.FunctionFlags & 0x00020000) == 0x00020000){
                sdk << "public" << " ";
            }
            if((func.FunctionFlags & 0x00040000) == 0x00040000){
                sdk << "private" << " ";
            }
            if((func.FunctionFlags & 0x00080000) == 0x00080000){
                sdk << "protected" << " ";
            }*/

            sdk << returnVal << " " << oname << "(" << params << ");";
			sdk << "// 0x" << setbase(16) << (func.Func - libbase) << endl;
		}
		else {
			sdk << "\t" << cname << " " << oname << ";" << endl;
		}

		child = field.Next;
	}
	return recurrce;
}

void writeStructs(ofstream &sdk, kaddr clazz) {
	list<kaddr> recurrce;

	kaddr currstruct = clazz;
	while (currstruct) {
		UStruct Struct(currstruct);

		uint32 Nameindex = Struct.FNameID;
		if (!isMapped(Nameindex) && !Struct.getClassName().empty() && !isEqual(Struct.getClassName(), "None")) {
			writtenmap.push_back(Nameindex);
			sdk << "Class: " << Struct.getClassPath() << endl;
			recurrce.merge(writeStructChildrens(sdk, Struct.Children));
			sdk << "\n--------------------------------" << endl;
			objcount++;
		}

		currstruct = Struct.SuperStruct;
	}

	for (auto it = recurrce.begin(); it != recurrce.end(); ++it)
		writeStructs(sdk, *it);
}

void DumpSDK(string out) {
	ofstream sdk(out + "/SDK.txt", ofstream::out);
	if (sdk.is_open()) {
		cout << "Dumping SDK List" << endl;
		clock_t begin = clock();
		for (uint32 i = 0; i < GUObjectCount; i++) {
			UObject uobj = GetUObjectFromID(i);
			if (uobj.isValid()) {
				writeStructs(sdk, uobj.ClassPrivate);
			}
		}
		sdk.close();
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		cout << objcount << " Items Dumped in SDK in " << elapsed_secs << "S" << endl;
	}
}

void TestDump(UObject uobj);

void DumpSDK2(string out) {
	ofstream sdk(out + "/SDK.txt", ofstream::out);
	if (sdk.is_open()) {
		cout << "Dumping SDK List" << endl;
		clock_t begin = clock();
		UObject gworld = UObject(getPtr(getRealOffset(Offsets::GWorld)));
		if (gworld.ptr) {
			//Iterate World
			writeStructs(sdk, gworld.ClassPrivate);
			//Iterate Entity
			kaddr level = getPtr(gworld.ptr + Offsets::UWorldToPersistentLevel);
			kaddr actorList = getPtr(level + Offsets::ULevelToAActors);
			int actorsCount = Read<int>(level + Offsets::ULevelToAActorsCount);
			for (int i = 0; i < actorsCount; i++) {
				UObject actor = UObject(getPtr(actorList + (i * sizeof(kaddr))));
				if (actor.ptr) {
					writeStructs(sdk, actor.ClassPrivate);
				}
			}
		}
		sdk.close();
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		cout << objcount << " Items Dumped in SDK in " << elapsed_secs << "S" << endl;
	}
}

void TestDump(UObject uobj){
    cout << "Name: " << uobj.getName() << endl;
    cout << "ObjectPtr: " << setbase(16) << uobj.ptr << endl;
    cout << "ClassPtr: " << setbase(16) << uobj.ClassPrivate << "\n" << endl;

    UStruct Struct(0);
    kaddr currstruct = uobj.ClassPrivate;
    while (currstruct) {
        Struct = UStruct(currstruct);
        currstruct = Struct.SuperStruct;
    }
    cout << "Class: " << Struct.getClassPath() << "\n" << endl;

    kaddr child = Struct.Children;
    while (child) {
        UField field(child);
        UClass fclass(field.ClassPrivate);

        string oname = field.getName();
        string cname = fclass.getClassName();

        cout << setbase(16) << child << " " << oname << " " << cname << ";" << endl;

        HexDump(child, 30);

        child = field.Next;
    }
}

#endif