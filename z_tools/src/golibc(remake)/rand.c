/* C语言标准函数 生成随机数 */

unsigned int rand_seed = 1;

void srand(int seed) {
	rand_seed = seed;
}

int rand(void){
	rand_seed = rand_seed * 1103515245 + 12345;
	return (unsigned int) (rand_seed >> 16) & 32767;
}
