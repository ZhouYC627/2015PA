#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;
static Elf32_Ehdr *myelf;
void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	myelf=elf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
  		}
 		else if(sh[i].sh_type == SHT_STRTAB && 
 				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
 	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
 }
void match_sym(char* strexp){
	int i;
	//printf("nr_symtab:%d\n",nr_symtab_entry);
	for (i=0;i<nr_symtab_entry;i++){
		if (symtab[i].st_info==17){
			//printf("name:%x\tval:%x\n",symtab[i].st_name,symtab[i].st_value);
			char* name;
			//char* value[10];
			//sprint(value,"%x",symtab[i].st_value)
			//strcpy(name,strtab+symtab[i].st_name);
			name=strtab+symtab[i].st_name;

			//printf("%s\n",name);
			char* loc;
			loc=strstr(strexp,name);
			if (loc!=NULL){
				char new_expr[100];
				strcpy(new_expr,strexp);	
				char* pre=strncpy(new_expr,strexp,loc-strexp);
				printf("pre:%s\n",pre);
				if(pre!=NULL){
					sprintf(new_expr,"%s0x%08x",pre,symtab[i].st_value);	
				}
				else{
					sprintf(new_expr,"0x%08x",symtab[i].st_value);
				}
				//printf("%s\n",new_expr);
				strcat(new_expr,loc+strlen(name));
				strcpy(strexp,new_expr);
				//printf("%s\n",strexp);
			}
		}
		//printf("%d\n",symtab[i].st_info);
		
	}
	//printf("%d\n",STT_OBJECT);
}
