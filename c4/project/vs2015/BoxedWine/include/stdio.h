#ifndef __STDIO_H__
#define __STDIO_H__

#include <sys/types.h>

typedef void* FILE;

#define EOF (-1)

#define 	SEEK_SET   0
#define 	SEEK_CUR   1
#define 	SEEK_END   2

#define _IOFBF		0x00	/* full buffering */
#define _IOLBF		0x01	/* line buffering */
#define _IONBF		0x02	/* no buffering */
#define __MODE_BUF	0x03	/* Modal buffering dependent on isatty */

#define __MODE_FREEBUF	0x04	/* Buffer allocated with malloc, can free */
#define __MODE_FREEFIL	0x08	/* FILE allocated with malloc, can free */

#define __MODE_READ	0x10	/* Opened in read only */
#define __MODE_WRITE	0x20	/* Opened in write only */
#define __MODE_RDWR	0x30	/* Opened in read/write */

#define __MODE_READING	0x40	/* Buffer has pending read data */
#define __MODE_WRITING	0x80	/* Buffer has pending write data */

#define __MODE_EOF	0x100	/* EOF status */
#define __MODE_ERR	0x200	/* Error status */
#define __MODE_UNGOT	0x400	/* Buffer has been polluted by ungetc */

#define __MODE_IOTRAN	0

FILE* winecrt_getStdin();
FILE* winecrt_getStdout();
FILE* winecrt_getStderr();

#define stdin winecrt_getStdin()
#define stdout winecrt_getStdout()
#define stderr winecrt_getStderr()

#define putc(c, stream)	\
    (((stream)->bufpos >= (stream)->bufwrite) ? fputc((c), (stream))	\
                          : (unsigned char) (*(stream)->bufpos++ = (c))	)

#define getc(stream)	\
  (((stream)->bufpos >= (stream)->bufread) ? fgetc(stream):		\
    (*(stream)->bufpos++))

#define putchar(c) putc((c), stdout)  
#define getchar() getc(stdin)

#define ferror(fp)	(((fp)->mode&__MODE_ERR) != 0)
#define feof(fp)   	(((fp)->mode&__MODE_EOF) != 0)
#define clearerr(fp)	((fp)->mode &= ~(__MODE_EOF|__MODE_ERR),0)
#define fileno(fp)	((fp)->fd)

int winecrt_fgetc(FILE*);
#define fgetc winecrt_fgetc

int winecrt_fputc(int, FILE*);
#define fputc winecrt_fputc

int winecrt_fclose(FILE *stream);
#define fclose winecrt_fclose

int winecrt_fflush(FILE* stream);
#define fflush winecrt_fflush

int winecrt_fread(char* buf, int size, int nelm, FILE* fp);
#define fread winecrt_fread

int winecrt_fwrite(const void * ptr, int size, int nitems, FILE* stream);
#define fwrite winecrt_fwrite

char *winecrt_fgets(char * s, size_t n, FILE * stream);
#define fgets winecrt_fgets

FILE *winecrt_fdopen(int fildes, const char *mode);
#define fdopen winecrt_fdopen

FILE *winecrt_fopen(const char *path, const char *mode);
#define fopen winecrt_fopen

int winecrt_fseek(FILE*, long, int);
#define fseek winecrt_fseek

long winecrt_ftell(FILE*);
#define ftell winecrt_ftell

void winecrt_rewind(FILE*);
#define rewind winecrt_rewind

int winecrt_fputs(char*, FILE*);
#define fputs winecrt_fputs

int winecrt_puts(char*);
#define puts winecrt_puts

void winecrt_setbuf(FILE * stream, char * buf);
#define setbuf winecrt_setbuf

int winecrt_setvbuf(FILE * stream, char * buf, int type, size_t size);
#define setvbuf winecrt_setvbuf

int winecrt_printf(const char*, ...);
#define printf winecrt_printf

int winecrt_fprintf(FILE*, const char*, ...);
#define fprintf winecrt_fprintf

int winecrt_sprintf(char*, const char*, ...);
#define sprintf winecrt_sprintf

int winecrt_vfprintf(FILE * stream, const char * format, va_list ap);
#define vfprintf winecrt_vfprintf

int winecrt_ungetc(int c, FILE * stream);
#define ungetc winecrt_ungetc

#define stdio_pending(fp) ((fp)->bufread>(fp)->bufpos)


int winecrt_snprintf(char *s, size_t n, const char *format, /* args */ ...);
#define snprintf winecrt_snprintf

int winecrt_vsnprintf(char * s, size_t n, const char * format, va_list ap);
#define vsnprintf winecrt_vsnprintf

int winecrt_rename(const char *old, const char *new);
#define rename winecrt_rename


int winecrt_vsscanf(const char * s, const char * format, va_list arg);
#define vsscanf winecrt_vsscanf

int winecrt_sscanf(const char * s, const char * format, ... );
#define sscanf winecrt_sscanf

void winecrt_perror(const char *s);
#define perror winecrt_perror

int winecrt_fscanf(FILE * stream, const char * format, ... );
#define fscanf winecrt_fscanf

#endif