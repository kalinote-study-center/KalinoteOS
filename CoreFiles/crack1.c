void api_end(void);

void KaliMain(void)
{
	*((char *) 0x00102600) = 0;
	api_end();
}
