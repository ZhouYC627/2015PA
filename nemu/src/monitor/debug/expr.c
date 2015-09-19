#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
//#define DEBUG

enum {
	NOTYPE = 256, EQ,HEX,DEC

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},					//minus
	{"\\*", '*'},					//mulitipy
	{"\\/", '/'},					//divide
	{"==", EQ},					// equal
	{"0[xX][0-9a-fA-F]+",HEX},			//hex
	{"[0-9]+",DEC},					//decimal
	{"\\(",'('},					//left bracket
	{"\\)",')'}					//right bracket
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE: break;
					case DEC:case HEX:{
						tokens[nr_token].type=rules[i].token_type;
						strncpy(tokens[nr_token].str,e+position-substr_len,substr_len);
						nr_token++;
						break;
					}
					case '(':case ')':
					case '+':case '-':
					case '*':case '/':
					case EQ :{
						tokens[nr_token].type=rules[i].token_type;
						nr_token++;
						break; 
					}
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}
bool check_parentheses(int p,int q)
{
	bool result=true;
	if (tokens[p].type!='('||tokens[q].type!=')') 
		result=false;
	int count=0,i;
	for(i=0;i<=q;i++){
		if (tokens[i].type=='(')
			count++;
		else if(tokens[i].type==')')
			count--;
		if (count==0 && i<q)result=false;
		if (count<0) {
			printf("Brackets not match.");
			return false;
		}
	}
	if (count>0) {
		printf("Brackets not match");
		return false;
	}
	return result;
}

static struct Operator{
	int oper;
	int prec;
}Operators[]={
	{'+',0},
	{'-',0},
	{'*',1},
	{'/',1}
};
#define NR_OP (sizeof(Operators) / sizeof(Operators[0]) )


int locate_domin(int p,int q)
{
	int loc=p;
	int now_prec=32767;
	int ct_par=0;
	int i;
	for (i=p;i<=q;i++){
		if (tokens[i].type=='(') ct_par++;
		else if (tokens[i].type==')')ct_par--;
		else {
			int ii;
			if (ct_par!=0) continue;
			for (ii=0;ii<NR_OP;ii++){
				if (Operators[ii].oper==tokens[i].type){
					if(Operators[ii].prec<=now_prec){
						now_prec=Operators[ii].prec;
						loc=i;
					}
					continue;

				}
			}
		}
	}
	if (now_prec==32767) {printf("Invaild Expression!\nCannot locate dominant operator.\n");return 0;}
	return loc;
}

uint32_t eval(int p, int q)
{
	printf("p=%d\tq=%d\n",p,q);
	if (p>q){
		/* Bad expression */
		Assert(p<q,"Bad expression");


	}
	else if (p==q){
		/*Single token
		 *For now this token should be a number.
		 *Return the value of the number.
		 */
		int num=0;
		if (tokens[p].type==DEC){
			num=atoi(tokens[p].str);
		}else if (tokens[p].type==HEX){
			sscanf(tokens[p].str,"%x",&num);
		}else{
			printf("Invaild Expression!\nNot a number.\n");
			assert(0);
		}
	}
	else if (check_parentheses(p,q)==true){
		printf("ok\n");
		return eval(p+1,q-1);
	}
	else{
		int op=0;
		op=locate_domin(p,q);
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);

		switch (tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':if (val2==0) {printf("Devided by zero!");return 0;}
				 return val1/val2;
		}
	
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
#ifdef DEBUG
	int i;
	printf("%d\n",nr_token);
	for (i=0;i<nr_token;i++)
		printf("%d\t",tokens[i].type);
	
#endif
	/* TODO: Insert codes to evaluate the expression. */
	//int p=0,q=nr_token;
	return eval(0,nr_token-1);
	panic("please implement me");
	return 0;
}
