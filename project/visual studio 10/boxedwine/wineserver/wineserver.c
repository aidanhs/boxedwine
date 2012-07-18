int _fltused=1;

__declspec(dllexport) void getArgs(int* argc, char * pargv[10]);

int main( int argc, char *argv[] );

void startup() {
	int argc;
	char* argv[10];
	getArgs(&argc, argv);
	main(argc, argv);
}


int mkstemps(char *template, int suffix_len) {
	return 0;
}