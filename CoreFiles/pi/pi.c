#include "apilib.h"

void KaliMain(void)
{
    /* 圆周率计算pi=4arctan（1）=4（1-1/3+1/5-1/7+1/9-…） */
    double s = 0.0;
    int i, d;
    for (i = 1; i < 500000000; i += 4) { /* 小于5亿，值误差大 */
        s += 1.0 / i - 1.0 / (i + 2);
    }
    s *= 4.0;

    for (i = 0; i < 15; i++) { /* 显示小于10的正数 */
        d = (int) s;
        api_putchar('0' + d);
        s = (s - d) * 10.0;
        if (i == 0) {
            api_putchar('.');
        }
    }
    api_putchar('\n');
    api_end();
}
