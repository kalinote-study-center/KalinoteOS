/* C语言标准函数库 ldexp */

double ldexp(double x, int n)
{
	if (n > 0) {
		do {
			x *= 2.0;
		} while (--n);
	}
	if (n < 0) {
		do {
			x *= 0.5;
		} while (++n);
	}
	return x;
}
