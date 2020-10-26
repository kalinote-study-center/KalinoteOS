void api_putstr0(char *s);
void api_end(void);

void KaliMain(void)
{
	api_putstr0("hello, world\n");
	api_end();
}
