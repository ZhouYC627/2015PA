#include "cpu/exec/template-start.h"
#define instr push

static void do_execute() {
	//Log("esp: %x, val: %x", cpu.esp, op_src->val);
	uint32_t n=4;
	if (ops_decoded.is_data_size_16) n=2; 
	cpu.esp-=n;
	//printf("push:%x(%d)\n",op_src->val,n);
	//MEM_W(cpu.esp,op_src->val);
	swaddr_write(cpu.esp,n,op_src->val, R_SS);
	//printf("+%x\n",MEM_R(cpu.esp));
	print_asm_template1();
}
make_instr_helper(i)
make_instr_helper(r)
make_instr_helper(rm)

/*
void push_l(uint32_t x){
	cpu.esp-=4;
	swaddr_write(cpu.esp, 4, x, R_SS);
}

make_helper(concat(pusha_, SUFFIX)) {
	swaddr_t temp = REG(R_ESP);
	int i;
	for (i = R_EAX; i <= R_EDI; i++){
		cpu.esp-=4;
		if (i == R_ESP) push_l(temp);
		else push_l(reg_l(i));
	print_asm("pusha"str(SUFFIX));
	return 1;
}
*/
#include "cpu/exec/template-end.h"
