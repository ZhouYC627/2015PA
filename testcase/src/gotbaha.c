#include "trap.h"

int is_prime(int n) {
	if(n < 2) return 0;
	
	int i;
	for(i = 2; i < n; i ++) {
		if(n % i == 0) {
			return 0;
		}
		//if (n==3)set_bp();
	}

	return 1;
}

int gotbaha(int n) {
	int i;
	//set_bp();
	for(i = 2; i < n; i ++) {
		if(is_prime(i) && is_prime(n - i)) {
			return 1;
			
		}
		//if (n==8) set_bp();
	}

	return 0;
}

int main() {
	int n;
	for(n = 4; n <= 100; n += 2) {
		//set_bp();
		nemu_assert(gotbaha(n) == 1);
	}

	HIT_GOOD_TRAP;

	return 0;
}
