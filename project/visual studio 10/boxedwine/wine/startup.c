#include <windows.h>

int main( int argc, char *argv[] );
char args[10][256];
void __cdecl mainCRTStartup() {
	char* cmdline = GetCommandLineA();
	int in_quotes = 0;
	char* argv[10];
	int count = 0;
	int len = 0;
	args[count][0]=0;
	argv[0] = args[count];

	while (*cmdline)
    {
        if ((*cmdline == '\t' || *cmdline == ' ') && !in_quotes) {						
			count++;
			argv[count] = args[count];
			len = 0;
		}
        else if (*cmdline == '\"')
        {
            in_quotes = !in_quotes;
        } else {
			args[count][len++] = *cmdline;
		}
        cmdline++;
    }
    main(count, argv);
}