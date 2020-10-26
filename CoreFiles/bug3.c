void api_putchar(int c);
void api_end(void);

void KaliMain(void)
{
	for (;;) {
		api_putchar('a');
	}
}
