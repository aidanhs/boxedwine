typedef struct { int _sjb[9]; } sigjmp_buf[1];

int hal_sigsetjmp(sigjmp_buf buf, int savemask)
{
	return 0;
}

void hal_siglongjmp(sigjmp_buf buf, int val)
{
}