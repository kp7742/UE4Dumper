#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"

#define SHDRS 16
/*
.dynsym .dynstr .hash .rel.dyn .rel.plt
.plt .text .ARM.extab .ARM.exidx .fini_array 
.init_array .dynamic .got .data
*/
#define NONE 0
#define DYNSYM 1
#define DYNSTR 2
#define HASH 3
#define RELDYN 4
#define RELPLT 5
#define PLT 6
#define TEXT 7
#define ARMEXIDX 8
#define FINIARRAY 9
#define INITARRAY 10
#define DYNAMIC 11
#define GOT 12
#define DATA 13
#define BSS 14
#define STRTAB 15
//

int fix_so(const char *openPath, const char *outPutPath, uint64_t ptrbase);
