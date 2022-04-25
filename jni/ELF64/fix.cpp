#define _CRT_SECURE_NO_WARNINGS
#include "fix.h"
#include "elf.h"

static const char* g_str = "..dynsym..dynstr..hash..rel.dyn..rel.plt..plt..text..ARM.exidx..fini_array..init_array..dynamic..got..data..bss..shstrtab..rela.dyn..rela.plt\0";
static const char* g_strtabcontent = "\0.dynsym\0.dynstr\0.hash\0.rel.dyn\0.rel.plt\0.plt\0.text\0.ARM.exidx\0.fini_array\0.init_array\0.dynamic\0.got\0.data\0.bss\0.shstrtab\0.rela.dyn\0.rela.plt\0";

static uint32_t _get_off_in_shstrtab(const char *name)
{
	return (uint32_t)(strstr(g_str, name) - g_str);
}


template <typename ElfHeaderType>
static void _get_elf_header(ElfHeaderType *pehdr, const char *buffer)
{
	int header_len = sizeof(ElfHeaderType);
	memcpy(pehdr, (void*)buffer, header_len);
}

static long _get_file_len(FILE *p)
{
	fseek (p, 0, SEEK_END);
	long fsize = ftell (p);
	rewind (p);
	return fsize;
}

template <typename Elf_Shdr_Type, typename Elf_Addr_Type, typename Elf_Rel_Type, bool isElf32>
static void _fix_relative_rebase(char *buffer, size_t bufSize, uint64_t imageBase, Elf_Shdr_Type *g_shdr)
{
    Elf_Addr_Type addr = g_shdr[RELDYN].sh_addr;
    size_t sz = g_shdr[RELDYN].sh_size;
    size_t n = sz / sizeof(Elf_Rel_Type);
    Elf_Rel_Type *rel = (Elf_Rel_Type*)(buffer+addr);
    const char *border = buffer+bufSize;
    for (size_t i = 0; i < n; ++i,++rel)
    {
        int type = 0;
        if (isElf32) {
			type = ELF32_R_TYPE(rel->r_info);
		}
        else {
        	type = ELF64_R_TYPE(rel->r_info);
        }
        //unsigned sym = (unsigned)ELF32_R_SYM(rel->r_info);
        if (type == R_ARM_RELATIVE)
        {
            //被Releative修正的地址需要减回装载地址才可以得出原本的Releative偏移
            Elf_Addr_Type off = rel->r_offset;
            unsigned *offIntBuf = (unsigned*)(buffer+off);
            if (border < (const char*)offIntBuf) {
            	uint64_t tmp = off;
                printf("relocation off %llx invalid, out of border...\n", tmp);
				continue;
            }
            unsigned addrNow = *offIntBuf;
            addrNow -= imageBase;
            (*offIntBuf) = addrNow;
        }
    }
}

template <typename Elf_Phdr_Type, typename Elf_Addr_Type>
uint32_t _get_mem_flag(Elf_Phdr_Type *phdr, size_t phNum, size_t memAddr) {
	for (int i = 0; i < phNum; i++) {
		Elf_Addr_Type begin = phdr[i].p_vaddr;
		Elf_Addr_Type end = begin + phdr[i].p_memsz;
		if (memAddr > begin && memAddr < end) {
			return phdr[i].p_flags;
		}
	}
	return 0;
}

template <typename Elf_Rel_Type, bool isElf32>
static void _fix_rel_bias(Elf_Rel_Type *relDyn, size_t relCount, size_t bias) {
	const int R_AARCH64_JUMP_SLOT = 1026;
	const int R_AARCH64_RELATIVE = 1027;
	for (int i = 0; i < relCount; i++) {
		unsigned type = 0;
		unsigned sym = 0;
		if (isElf32) {
			type = ELF32_R_TYPE(relDyn[i].r_info);
			sym = ELF32_R_SYM(relDyn[i].r_info);
		}
		else {
			type = ELF64_R_TYPE(relDyn[i].r_info);
			sym = ELF64_R_SYM(relDyn[i].r_info);
		}
		//这两种重定位地址都是相对于loadAddr的，所以要修正
		if (type == R_ARM_JUMP_SLOT || type == R_ARM_RELATIVE || type == R_AARCH64_JUMP_SLOT || type == R_AARCH64_RELATIVE) {
		    if (relDyn[i].r_offset > 0) {
				relDyn[i].r_offset -= bias;
			}
		}
	}
}

template <typename Elf_Sym_Type>
static void _fix_dynsym_bias(Elf_Sym_Type *dysym, size_t count, size_t bias) {
	for (int i = 0; i < count; ++i) {
		if (dysym[i].st_value > 0) {
			dysym[i].st_value -= bias;
		}
	}
}
static uint64_t paddup(uint64_t input, uint64_t align) {
    uint64_t pad = ~(align-1);
	return input % align ? (input + align) & pad : input;
}

template <typename Elf_Ehdr_Type, typename Elf_Shdr_Type, typename Elf_Phdr_Type,
		typename Elf_Word_Type, typename Elf_Addr_Type, typename Elf_Sym_Type,
		typename Elf_Dyn_Type, typename Elf_Rel_Type,
		bool isElf32>
static void _regen_section_header(const Elf_Ehdr_Type *pehdr, const char *buffer, size_t len, Elf_Shdr_Type g_shdr[SHDRS])
{
	Elf_Phdr_Type lastLoad = { 0 };
	Elf_Phdr_Type *phdr = (Elf_Phdr_Type*)(buffer + pehdr->e_phoff);
	int ph_num = pehdr->e_phnum;
	int dyn_size = 0, dyn_off = 0;

	//所有相对于module base的地址都要减去这个地址
    size_t bias = 0;
	for(int i = 0;i < ph_num;i++) {
        if (phdr[i].p_type == PT_LOAD) {
        	//see linker get_elf_exec_load_bias
            bias = phdr[i].p_vaddr;
			break;
		}
	}

	Elf_Word_Type maxLoad = 0;
	for(int i = 0;i < ph_num;i++) {
		if (phdr[i].p_type == PT_LOAD) {
		    //取得最后一个load，获得整个so加载大小
			maxLoad = phdr[i].p_vaddr + phdr[i].p_memsz - bias;
		}
	}
	if (maxLoad > len) {
		//加载的范围大于整个dump下来的so，有问题，先警告
		printf("warning load size [%u] is bigger than so size [%u], dump maybe incomplete!!!\n", maxLoad, len);
		//TODO:should we fix it???
	}

	int loadIndex = 0;
	int align = sizeof(Elf_Addr_Type);
	for(int i = 0;i < ph_num;i++) {
		phdr[i].p_vaddr -= bias;
		phdr[i].p_paddr = phdr[i].p_vaddr;
		//段在文件中的偏移修正，因为从内存dump出来的文件偏移就是在内存的偏移
		phdr[i].p_offset =  phdr[i].p_vaddr;
		phdr[i].p_filesz = phdr[i].p_memsz;
		Elf_Word_Type p_type = phdr[i].p_type;
		if (phdr[i].p_type == PT_LOAD) {
			loadIndex++;
			if (phdr[i].p_vaddr > 0x0 && loadIndex == 2) {
				lastLoad = phdr[i];
			}
		}
		else if(p_type == PT_DYNAMIC) {
			//动态表，动态表包括很多项，找到动态表位置可以恢复大部分结构,这个是恢复的突破口
			g_shdr[DYNAMIC].sh_name = _get_off_in_shstrtab(".dynamic");
			g_shdr[DYNAMIC].sh_type = SHT_DYNAMIC;
			g_shdr[DYNAMIC].sh_flags = SHF_WRITE | SHF_ALLOC;
			g_shdr[DYNAMIC].sh_addr = phdr[i].p_vaddr;
			g_shdr[DYNAMIC].sh_offset = phdr[i].p_vaddr;
			g_shdr[DYNAMIC].sh_size = phdr[i].p_memsz;
			g_shdr[DYNAMIC].sh_info = 0;
            g_shdr[DYNAMIC].sh_link = DYNSTR;
			if (isElf32) {
				g_shdr[DYNAMIC].sh_addralign = align;
				g_shdr[DYNAMIC].sh_entsize = 8;
			}
			else {
				g_shdr[DYNAMIC].sh_addralign = align;
				g_shdr[DYNAMIC].sh_entsize = 16;
			}

			dyn_size = phdr[i].p_memsz;
			dyn_off = phdr[i].p_vaddr;
		}

		else if(phdr[i].p_type == PT_LOPROC || phdr[i].p_type == PT_LOPROC + 1) {
			g_shdr[ARMEXIDX].sh_name = _get_off_in_shstrtab(".ARM.exidx");
			g_shdr[ARMEXIDX].sh_type = SHT_LOPROC;
			g_shdr[ARMEXIDX].sh_flags = SHF_ALLOC;
			g_shdr[ARMEXIDX].sh_addr = phdr[i].p_vaddr;
			g_shdr[ARMEXIDX].sh_offset = phdr[i].p_vaddr;
			g_shdr[ARMEXIDX].sh_size = phdr[i].p_memsz;
			g_shdr[ARMEXIDX].sh_link = 7;
			g_shdr[ARMEXIDX].sh_info = 0;
			g_shdr[ARMEXIDX].sh_addralign = align;
			g_shdr[ARMEXIDX].sh_entsize = 8;
		}
	}

	Elf_Dyn_Type *dyn = (Elf_Dyn_Type*)(buffer+dyn_off);
	int n = dyn_size / sizeof(Elf_Dyn_Type);

	Elf_Word_Type __global_offset_table = 0;
	int nDynSyms = 0;
	for (int i=0; i < n; i++) {
		int tag = dyn[i].d_tag;
		switch (tag) {
			case DT_SYMTAB:
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[DYNSYM].sh_name = _get_off_in_shstrtab(".dynsym");
				g_shdr[DYNSYM].sh_type = SHT_DYNSYM;
				g_shdr[DYNSYM].sh_flags = SHF_ALLOC;
				g_shdr[DYNSYM].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[DYNSYM].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[DYNSYM].sh_link = 2;
				g_shdr[DYNSYM].sh_info = 1;
				g_shdr[DYNSYM].sh_addralign = align;
				break;
			case DT_SYMENT:
				g_shdr[DYNSYM].sh_entsize = dyn[i].d_un.d_ptr;
				break;

			case DT_STRTAB:
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[DYNSTR].sh_name = _get_off_in_shstrtab(".dynstr");
				g_shdr[DYNSTR].sh_type = SHT_STRTAB;
				g_shdr[DYNSTR].sh_flags = SHF_ALLOC;
				g_shdr[DYNSTR].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[DYNSTR].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[DYNSTR].sh_addralign = 1;
				g_shdr[DYNSTR].sh_entsize = 0;
				break;

			case DT_STRSZ:
				g_shdr[DYNSTR].sh_size = dyn[i].d_un.d_val;
				break;

			case DT_HASH:
			{
				dyn[i].d_un.d_ptr -= bias;
				int nbucket = 0, nchain = 0;
				g_shdr[HASH].sh_name = _get_off_in_shstrtab(".hash");
				g_shdr[HASH].sh_type = SHT_HASH;
				g_shdr[HASH].sh_flags = SHF_ALLOC;
				g_shdr[HASH].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[HASH].sh_offset = dyn[i].d_un.d_ptr;
				memcpy(&nbucket, buffer + g_shdr[HASH].sh_offset, 4);
				memcpy(&nchain, buffer + g_shdr[HASH].sh_offset + 4, 4);
				g_shdr[HASH].sh_size = (nbucket + nchain + 2) * sizeof(int);
				g_shdr[HASH].sh_link = DYNSYM;
				g_shdr[HASH].sh_info = 0;
				g_shdr[HASH].sh_addralign = align;
				g_shdr[HASH].sh_entsize = 4;
				//linker源码，DT_HASH实际上是通过hashtable在加速动态符号的查找，所以hashtable的大小就是动态符号表的大小
				nDynSyms = nchain;
				break;
			}
			case DT_REL:
			case DT_RELA: {
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[RELDYN].sh_flags = SHF_ALLOC;
				g_shdr[RELDYN].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[RELDYN].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[RELDYN].sh_link = DYNSYM;
				g_shdr[RELDYN].sh_info = 0;
				g_shdr[RELDYN].sh_addralign = align;
				if (tag == DT_REL) {
					g_shdr[RELDYN].sh_name = _get_off_in_shstrtab(".rel.dyn");
					g_shdr[RELDYN].sh_type = SHT_REL;
				} else {
					g_shdr[RELDYN].sh_name = _get_off_in_shstrtab(".rela.dyn");
					g_shdr[RELDYN].sh_type = SHT_RELA;
				}
				break;
			}

			case DT_RELSZ:
			case DT_RELASZ:
				g_shdr[RELDYN].sh_size = dyn[i].d_un.d_val;
				break;

			case DT_RELENT:
			case DT_RELAENT:
				g_shdr[RELPLT].sh_entsize = dyn[i].d_un.d_val;
				g_shdr[RELDYN].sh_entsize = dyn[i].d_un.d_val;
				break;

			case DT_JMPREL:
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[RELPLT].sh_flags = SHF_ALLOC;
				g_shdr[RELPLT].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[RELPLT].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[RELPLT].sh_link = DYNSYM;
				g_shdr[RELPLT].sh_info = PLT;
				g_shdr[RELPLT].sh_addralign = align;
				if (isElf32) {
					g_shdr[RELPLT].sh_name = _get_off_in_shstrtab(".rel.plt");
					g_shdr[RELPLT].sh_type = SHT_REL;
				}
				else {
					g_shdr[RELPLT].sh_name = _get_off_in_shstrtab(".rela.plt");
					g_shdr[RELPLT].sh_type = SHT_RELA;
				}

				break;

			case DT_PLTRELSZ:
				g_shdr[RELPLT].sh_size = dyn[i].d_un.d_val;
				break;

			case DT_FINI_ARRAY:
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[FINIARRAY].sh_name = _get_off_in_shstrtab(".fini_array");
				g_shdr[FINIARRAY].sh_type = 15;
				g_shdr[FINIARRAY].sh_flags = SHF_WRITE | SHF_ALLOC;
				g_shdr[FINIARRAY].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[FINIARRAY].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[FINIARRAY].sh_addralign = align;
				g_shdr[FINIARRAY].sh_entsize = 0;
				break;

			case DT_FINI_ARRAYSZ:
				g_shdr[FINIARRAY].sh_size = dyn[i].d_un.d_val;
				break;

			case DT_INIT_ARRAY:
				dyn[i].d_un.d_ptr -= bias;
				g_shdr[INITARRAY].sh_name = _get_off_in_shstrtab(".init_array");
				g_shdr[INITARRAY].sh_type = 14;
				g_shdr[INITARRAY].sh_flags = SHF_WRITE | SHF_ALLOC;
				g_shdr[INITARRAY].sh_offset = dyn[i].d_un.d_ptr;
				g_shdr[INITARRAY].sh_addr = dyn[i].d_un.d_ptr;
				g_shdr[INITARRAY].sh_addralign = align;
				g_shdr[INITARRAY].sh_entsize = 0;
				break;

			case DT_INIT_ARRAYSZ:
				g_shdr[INITARRAY].sh_size = dyn[i].d_un.d_val;
				break;

			case DT_PLTGOT:
				dyn[i].d_un.d_ptr -= bias;
				__global_offset_table = dyn[i].d_un.d_ptr;
				g_shdr[GOT].sh_name = _get_off_in_shstrtab(".got");
				g_shdr[GOT].sh_type = SHT_PROGBITS;
				g_shdr[GOT].sh_flags = SHF_WRITE | SHF_ALLOC;
				//TODO:这里基于假设.got一定在.dynamic段之后，并不可靠，王者荣耀libGameCore.so就是例外
				g_shdr[GOT].sh_addr = g_shdr[DYNAMIC].sh_addr + g_shdr[DYNAMIC].sh_size;
				g_shdr[GOT].sh_offset = g_shdr[GOT].sh_addr;
				g_shdr[GOT].sh_addralign = align;

				break;
			case DT_INIT: {
				//找到init段代码，但是无法知道有多长，只好做一个警告，提醒使用者init段存在，脱壳代码可能存在这里
				uint64_t tmp = dyn[i].d_un.d_ptr;
				printf("warning .init exist at 0x%016llx\n", tmp);
				break;
			}
			case DT_TEXTREL:
				//地址相关的so，警告，暂时不做处理
				printf("warning DT_TEXTREL found, so is address depend.\n");
				break;
		}
	}
	size_t relpltCount = g_shdr[RELPLT].sh_size/g_shdr[RELPLT].sh_entsize;
	if (__global_offset_table)
	{
		Elf_Word_Type gotBase = g_shdr[GOT].sh_addr;

		//__global_offset_table里面成员个数等于RELPLT的成员数+3个固定成员
		Elf_Word_Type szGotEntry = 4;
		if (!isElf32) {
			szGotEntry = 8;
		}
		Elf_Word_Type gotEnd = __global_offset_table + szGotEntry * (relpltCount + 3);

		//上面那种方式计算不可靠，根据libGameCore.so分析，nRelPlt比数量比实际GOT数量多10个，暂时没发现这十个成员的特殊性
		//.got的结尾就是.data的开始，根据经验，data的地址总是与0x1000对齐。以此来修正地址
		Elf_Word_Type gotEndTry = gotEnd & ~0x0FFF;
		if (__global_offset_table < gotEndTry) {
		    gotEnd = gotEndTry;
		}

		g_shdr[DATA].sh_name = _get_off_in_shstrtab(".data");
		g_shdr[DATA].sh_type = SHT_PROGBITS;
		g_shdr[DATA].sh_flags = SHF_WRITE | SHF_ALLOC;
		g_shdr[DATA].sh_addr = paddup(gotEnd, 0x1000);
		g_shdr[DATA].sh_offset = g_shdr[DATA].sh_addr;
		g_shdr[DATA].sh_size = lastLoad.p_vaddr + lastLoad.p_memsz - g_shdr[DATA].sh_addr;
		g_shdr[DATA].sh_addralign = align;
		if (gotEnd > gotBase)
		{
			g_shdr[GOT].sh_size = gotEnd - gotBase;
		}
		else
		{
			//.got紧接着.dynamic的假设不成立
			//虽然算不准got段的真正的地址，但是可以用__global_offset_table的地址充当.got段的地址，__global_offset_table以上的地址全部为
			//数据段的修正地址，对分析关系不大。
			printf("warning .got is not after .dynamic use __global_offset_table as .got base\n");
			g_shdr[GOT].sh_addr = g_shdr[GOT].sh_offset = __global_offset_table;
			g_shdr[GOT].sh_size = gotEnd - __global_offset_table;
		}
	}

	const char *symbase = buffer + g_shdr[DYNSYM].sh_addr;
	//如果之前没有HASH表，无法确定符号表大小，只能靠猜测来获取符号表大小
	if (nDynSyms == 0)
	{
		printf("warning DT_HASH not found,try to detect dynsym size...\n");
		const char *strbase = buffer + g_shdr[DYNSTR].sh_addr;
		const char *strend = strbase + g_shdr[DYNSTR].sh_size;
		unsigned symCount = 0;
		Elf_Sym_Type *sym = (Elf_Sym_Type *) symbase;
		while (1) {
			//符号在符号表里面的偏移，不用考虑文件与内存加载之间bias
			size_t off = sym->st_name;
			const char *symName = strbase + off;
			size_t symOff = sym->st_value;
			//printf("symName=%p strbase=%p strend=%p\n", symName, strbase, strend);
			if ((size_t) symName < (size_t) strbase || (size_t) symName > (size_t) strend) {
				//动态表的符号偏移不在动态字符串表之内，说明非法，已经没有合法的动态符号了。
				//printf("break 1 symName=%s strbase");
				break;
			}
			symCount++;
			sym++;
		}
		nDynSyms = symCount;
	}

	Elf_Sym_Type *sym = (Elf_Sym_Type *) symbase;
	for (int i = 0; i < nDynSyms; i++) {
	    //发现某些so如饿了么libdeadpool通过将符号表里面的type设置成错误的值，从而使ida分析出错
	    //这里如果发现值是非法的，强制指定为FUNC类型，让ida分析
		unsigned char info = sym->st_info;
		unsigned int type = ELF_ST_TYPE(info);
		if (type > STT_FILE) {
			unsigned char c = (unsigned char)(info & 0xF0);
			unsigned newType = STT_OBJECT;
			if (sym->st_value == 0) {
				//当符号值为零说明是个外部符号，此时类型判断不准，给一个通常的就可
				newType = STT_FUNC;
			}
			else {
				//内存符号可以通过内存读写属性来判断是什么符号
				uint32_t flag = _get_mem_flag<Elf_Phdr_Type, Elf_Addr_Type>(phdr, ph_num, sym->st_value);
				if (flag & PF_X) {
					newType = STT_FUNC;
				}
			}
			sym->st_info = (unsigned char)(c | newType);
		}
		sym++;
	}

	//printf("size %d addr %08x\n", g_shdr[DYNSTR].sh_size, g_shdr[DYNSTR].sh_addr);
	g_shdr[DYNSYM].sh_size = nDynSyms * sizeof(Elf_Sym_Type);

	unsigned pltAlign = 4;
	if (!isElf32) {
		pltAlign = 16;
	}
	g_shdr[PLT].sh_name = _get_off_in_shstrtab(".plt");
	g_shdr[PLT].sh_type = SHT_PROGBITS;
	g_shdr[PLT].sh_flags = SHF_ALLOC | SHF_EXECINSTR;
	Elf_Addr_Type addr = g_shdr[RELPLT].sh_addr + g_shdr[RELPLT].sh_size;

	g_shdr[PLT].sh_addr = paddup(addr, pltAlign);
	//g_shdr[PLT].sh_addr = 0x0000000000031df0;
	g_shdr[PLT].sh_offset = g_shdr[PLT].sh_addr;
	//20=padding 12=每个plt的指令大小
	Elf_Word_Type szPltEntry = 12;
	if (!isElf32) {
		szPltEntry = 16;
	}
	g_shdr[PLT].sh_size = paddup(20 + szPltEntry * relpltCount, pltAlign);
	g_shdr[PLT].sh_addralign = pltAlign;

	if (g_shdr[ARMEXIDX].sh_addr != 0) {
		//text段的确定依赖ARMEXIDX的决定，ARMEXIDX没有的话，干脆不要text段了，因为text对ida分析没什么作用，ida对第一个LOAD的分析已经涵盖了text段的作用
		g_shdr[TEXT].sh_name = _get_off_in_shstrtab(".text");
		g_shdr[TEXT].sh_type = SHT_PROGBITS;
		g_shdr[TEXT].sh_flags = SHF_ALLOC | SHF_EXECINSTR;
		g_shdr[TEXT].sh_addr = g_shdr[PLT].sh_addr + g_shdr[PLT].sh_size;
		g_shdr[TEXT].sh_offset = g_shdr[TEXT].sh_addr;
		g_shdr[TEXT].sh_size = g_shdr[ARMEXIDX].sh_addr - g_shdr[TEXT].sh_addr;
	}

	g_shdr[STRTAB].sh_name = _get_off_in_shstrtab(".shstrtab");
	g_shdr[STRTAB].sh_type = SHT_STRTAB;
	g_shdr[STRTAB].sh_flags = SHT_NULL;
	g_shdr[STRTAB].sh_addr = 0;	//写文件的时候修正
	g_shdr[STRTAB].sh_size = (uint32_t)strlen(g_str) + 1;
	g_shdr[STRTAB].sh_addralign = 1;


	Elf_Rel_Type *relDyn = (Elf_Rel_Type*)(buffer + g_shdr[RELDYN].sh_addr);
	size_t relCount = g_shdr[RELDYN].sh_size/g_shdr[RELDYN].sh_entsize;
	_fix_rel_bias<Elf_Rel_Type, isElf32>(relDyn, relCount, bias);

	Elf_Rel_Type *relPlt = (Elf_Rel_Type*)(buffer + g_shdr[RELPLT].sh_addr);
	_fix_rel_bias<Elf_Rel_Type, isElf32>(relPlt, relpltCount, bias);

	Elf_Sym_Type *dynsym = (Elf_Sym_Type*)(buffer+g_shdr[DYNSYM].sh_addr);
	_fix_dynsym_bias<Elf_Sym_Type>(dynsym, nDynSyms, bias);
}

static bool is_elf32(const char *soPath) {
	FILE *f = fopen(soPath, "rb");
	fseek(f, 0x4, SEEK_SET);
	char buf[10] = {0};
	fread(buf, 1, 1, f);
	fclose(f);
	return buf[0] == 0x1;
}

template <typename Elf_Ehdr_Type, typename Elf_Shdr_Type,
		typename Elf_Phdr_Type, typename Elf_Word_Type,
		typename Elf_Addr_Type , typename Elf_Sym_Type, typename Elf_Dyn_Type,
		typename Elf_Rel_Type, bool isElf32>
static void _fix_elf(char *buffer, size_t flen, FILE *fw, uint64_t ptrbase) {
	Elf_Shdr_Type g_shdr[SHDRS] = { 0 };
	Elf_Ehdr_Type ehdr = {0};
	_get_elf_header<Elf_Ehdr_Type>(&ehdr, buffer);

	_regen_section_header<Elf_Ehdr_Type, Elf_Shdr_Type,
                        Elf_Phdr_Type, Elf_Word_Type, Elf_Addr_Type,
                        Elf_Sym_Type, Elf_Dyn_Type, Elf_Rel_Type, isElf32>
			(&ehdr, buffer, flen, g_shdr);

	_fix_relative_rebase<Elf_Shdr_Type, Elf_Addr_Type, Elf_Rel_Type, isElf32>(buffer, flen, ptrbase, g_shdr);

	size_t shstrtabsz = strlen(g_str)+1;
	ehdr.e_entry = ptrbase;
	ehdr.e_shnum = SHDRS;
	//倒数第一个为段名字符串段
	ehdr.e_shstrndx = SHDRS - 1;
	ehdr.e_shentsize = sizeof(Elf_Shdr_Type);

	//段表头紧接住段表最后一个成员--字符串段之后
	ehdr.e_shoff = (Elf_Addr_Type)(flen + shstrtabsz);

	//就在原来文件最后加上段名字符串段
	g_shdr[STRTAB].sh_offset = flen;
	size_t szEhdr = sizeof(Elf_Ehdr_Type);
	//Elf头
	fwrite(&ehdr, szEhdr, 1, fw);
	//除了Elf头之外的原文件内容
	fwrite(buffer+szEhdr, flen-szEhdr, 1, fw);
	//补上段名字符串段
	fwrite(g_strtabcontent, shstrtabsz, 1, fw);
	//补上段表头
	fwrite(&g_shdr, sizeof(g_shdr), 1, fw);
}

int fix_so(const char *openPath, const char *outPutPath, uint64_t ptrbase)
{
	FILE *fr = NULL, *fw = NULL;

	fr = fopen(openPath,"rb");
	
	if(fr == NULL) {
		printf("Open failed: \n");
        return -3;
	}
	bool isElf32 = is_elf32(openPath);
    char head[4] = {0};
    fread(head, 1, 4, fr);
    if (head[0] != 0x7f || head[1] != 'E' || head[2] != 'L' || head[3] != 'F') {
        printf("error header is not .ELF!!!\n");
        fclose(fr);
		return -5;
    }
	fseek(fr, 0, SEEK_SET);
	
	size_t flen = _get_file_len(fr);
	
	char *buffer = (char*)malloc(flen);
	if (buffer == NULL) {
		printf("Malloc error\n");
		fclose(fr);
        return -1;
	}
	
	unsigned long result = fread (buffer, 1, flen, fr);
	if (result != flen) {
		printf("Reading %s error\n", openPath);
        fclose(fr);
		free(buffer);
		return -2;
	}
	fw = fopen(outPutPath, "wb");
	if(fw == NULL) {
		printf("Open failed: %s\n", outPutPath);
		fclose(fr);
		free(buffer);
		return -4;
	}

	if (isElf32) {
		_fix_elf<Elf32_Ehdr, Elf32_Shdr, Elf32_Phdr, Elf32_Word, Elf32_Addr, Elf32_Sym, Elf32_Dyn, Elf32_Rel, true>(buffer, flen, fw, ptrbase);
	}
	else {
		_fix_elf<Elf64_Ehdr, Elf64_Shdr, Elf64_Phdr, Elf64_Word, Elf64_Addr, Elf64_Sym, Elf64_Dyn, Elf64_Rela, false>(buffer, flen, fw, ptrbase);
	}

	printf("fixed so has write to %s\n", outPutPath);
	if(fw != NULL)
		fclose(fw);
	if(fr != NULL)
		fclose(fr);
	free(buffer);
	return 0;
}
