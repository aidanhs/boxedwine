#include <windows.h>

char args[10][256];

void getArgs(int* argc, char* argv[10]) {
	char* cmdline = GetCommandLineA();
	int in_quotes = 0;
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
	if (len>0) {
		count++;
		argv[count] = args[count];
	}
	*argc = count;
}