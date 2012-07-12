#ifndef __STDIO_H__
#define __STDIO_H__

#include <sys/types.h>

#define BUFSIZ 500

struct __stdio_file {
  unsigned char *bufpos;   /* the next byte to write to or read from */
  unsigned char *bufread;  /* the end of data returned by last read() */
  unsigned char *bufwrite; /* highest address writable by macro */
  unsigned char *bufstart; /* the start of the buffer */
  unsigned char *bufend;   /* the end of the buffer; ie the byte after the last
                              malloc()ed byte */

  int fd; /* the file descriptor associated with the stream */
  int mode;

  unsigned char unbuf[8];	   /* The buffer for 'unbuffered' streams */

  struct __stdio_file * next;
};

typedef struct __stdio_file FILE;

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

WINECRT FILE stdin[1];
WINECRT FILE stdout[1];
WINECRT FILE stderr[1];

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

WINECRT int fgetc(FILE*);
WINECRT int fputc(int, FILE*);

WINECRT int fclose(FILE *stream);
WINECRT int fflush(FILE* stream);
WINECRT int fread(char* buf, int size, int nelm, FILE* fp);
WINECRT char *fgets(char * s, size_t n, FILE * stream);
WINECRT FILE *__fopen(char*, int, FILE*, char*);

#define fopen(__file, __mode)         __fopen((__file), -1, (FILE*)0, (__mode))
#define freopen(__file, __mode, __fp) __fopen((__file), -1, (__fp), (__mode))
#define fdopen(__file, __mode)  __fopen((char*)0, (__file), (FILE*)0, (__mode))
WINECRT int fseek(FILE*, long, int);
WINECRT long ftell(FILE*);
WINECRT void rewind(FILE*);

WINECRT int fputs(char*, FILE*);
WINECRT int puts(char*);
WINECRT void setbuf(FILE * stream, char * buf);

WINECRT int printf(const char*, ...);
WINECRT int fprintf(FILE*, const char*, ...);
WINECRT int sprintf(char*, const char*, ...);
WINECRT int vfprintf(FILE * stream, const char * format, va_list ap);
WINECRT int ungetc(int c, FILE * stream);

#define stdio_pending(fp) ((fp)->bufread>(fp)->bufpos)


WINECRT int snprintf(char *s, size_t n, const char *format, /* args */ ...);
WINECRT int vsnprintf(char * s, size_t n, const char * format, va_list ap);

DOSHAL int rename(const char *old, const char *new);


WINECRT int vsscanf(const char * s, const char * format, va_list arg);
WINECRT int sscanf(const char * s, const char * format, ... );
WINECRT void perror(const char *s);
WINECRT int fscanf(FILE * stream, const char * format, ... );

#endif