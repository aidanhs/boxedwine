#include <Windows.h>
void msvcrt_set_errno(int err);
char buffer[1024];
int hal_open(const char *path, int oflag, ... )
{

	if (oflag == 0) { // O_RDONLY 		
		int result = (int)CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		msvcrt_set_errno(result<0?GetLastError():0);
		return result;
	}
	return -1;
}