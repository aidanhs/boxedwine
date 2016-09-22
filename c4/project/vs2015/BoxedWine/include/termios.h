#ifndef __TERMIOS_H__
#define __TERMIOS_H__

struct winsize {
  unsigned short ws_row;
  unsigned short ws_col;
  unsigned short ws_xpixel;
  unsigned short ws_ypixel;
};

#define NCC 8
struct termio {
  unsigned short c_iflag;		/* input mode flags */
  unsigned short c_oflag;		/* output mode flags */
  unsigned short c_cflag;		/* control mode flags */
  unsigned short c_lflag;		/* local mode flags */
  unsigned char c_line;		/* line discipline */
  unsigned char c_cc[NCC];	/* control characters */
};

/* modem lines */
#ifdef __mips__
#define TIOCM_LE	0x001
#define TIOCM_DTR	0x002
#define TIOCM_RTS	0x004
#define TIOCM_ST	0x010
#define TIOCM_SR	0x020
#define TIOCM_CTS	0x040
#define TIOCM_CAR	0x100
#define TIOCM_RNG	0x200
#define TIOCM_DSR	0x400
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG
#define TIOCM_OUT1	0x2000
#define TIOCM_OUT2	0x4000
#define TIOCM_LOOP	0x8000
#else
#define TIOCM_LE	0x001
#define TIOCM_DTR	0x002
#define TIOCM_RTS	0x004
#define TIOCM_ST	0x008
#define TIOCM_SR	0x010
#define TIOCM_CTS	0x020
#define TIOCM_CAR	0x040
#define TIOCM_RNG	0x080
#define TIOCM_DSR	0x100
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG
#define TIOCM_OUT1	0x2000
#define TIOCM_OUT2	0x4000
#define TIOCM_LOOP	0x8000
#endif

/* line disciplines */
#define N_TTY		0
#define N_SLIP		1
#define N_MOUSE		2
#define N_PPP		3
#define N_STRIP		4
#define N_AX25		5
#define N_X25		6	/* X.25 async */
#define N_6PACK		7
#define N_MASC		8	/* Reserved for Mobitex module <kaz@cafe.net> */
#define N_R3964		9	/* Reserved for Simatic R3964 module */
#define N_PROFIBUS_FDL	10	/* Reserved for Profibus <Dave@mvhi.com> */
#define N_IRDA		11	/* Linux IR - http://irda.sourceforge.net/ */
#define N_SMSBLOCK	12	/* SMS block mode - for talking to GSM data cards about SMS messages */
#define N_HDLC		13	/* synchronous HDLC */
#define N_SYNC_PPP	14	/* synchronous PPP */
#define N_HCI		15	/* Bluetooth HCI UART */

typedef unsigned char	cc_t;
typedef unsigned int	speed_t;
typedef unsigned int	tcflag_t;

#define _POSIX_VDISABLE '\0'

#define NCCS	19
struct termios {
  tcflag_t c_iflag;		/* input mode flags */
  tcflag_t c_oflag;		/* output mode flags */
  tcflag_t c_cflag;		/* control mode flags */
  tcflag_t c_lflag;		/* local mode flags */
  cc_t c_line;			/* line discipline */
  cc_t c_cc[NCCS];		/* control characters */
};

/* c_cc characters */
#if defined(__alpha__)
#define VEOF	0
#define VEOL	1
#define VEOL2	2
#define VERASE	3
#define VWERASE 4
#define VKILL	5
#define VREPRINT 6
#define VSWTC	7
#define VINTR	8
#define VQUIT	9
#define VSUSP	10
#define VSTART	12
#define VSTOP	13
#define VLNEXT	14
#define VDISCARD 15
#define VMIN	16
#define VTIME	17
#elif defined(__mips__)
#define VINTR	0
#define VQUIT	1
#define VERASE	2
#define VKILL	3
#define VMIN	4
#define VTIME	5
#define VEOL2	6
#define VSWTC	7
#define VSWTCH	VSWTC
#define VSTART	8
#define VSTOP	9
#define VSUSP	10
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT	15
#define VEOF	16
#define VEOL	17
#elif defined(powerpc) || defined(__powerpc64__)
#define VINTR	0
#define VQUIT	1
#define VERASE	2
#define VKILL	3
#define VEOF	4
#define VMIN	5
#define VEOL	6
#define VTIME	7
#define VEOL2	8
#define VSWTC	9
#define VWERASE 10
#define VREPRINT 11
#define VSUSP	12
#define VSTART	13
#define VSTOP	14
#define VLNEXT	15
#define VDISCARD 16
#elif defined(__sparc__)
#define VINTR	0
#define VQUIT	1
#define VERASE	2
#define VKILL	3
#define VEOF	4
#define VEOL	5
#define VEOL2	6
#define VSWTC	7
#define VSTART	8
#define VSTOP	9
#define VSUSP	10
#define VDSUSP	11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT	15
#define VMIN	16
#define VTIME	17
#else			/* arm, i386, parisc, s390, x86_64 */
#define VINTR	0
#define VQUIT	1
#define VERASE	2
#define VKILL	3
#define VEOF	4
#define VTIME	5
#define VMIN	6
#define VSWTC	7
#define VSTART	8
#define VSTOP	9
#define VSUSP	10
#define VEOL	11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT	15
#define VEOL2	16
#endif

/* c_iflag bits */
#define IGNBRK	0000001
#define BRKINT	0000002
#define IGNPAR	0000004
#define PARMRK	0000010
#define INPCK	0000020
#define ISTRIP	0000040
#define INLCR	0000100
#define IGNCR	0000200
#define ICRNL	0000400
#define IUCLC	0001000
#define IXON	0002000
#define IXANY	0004000
#define IXOFF	0010000
#define IMAXBEL	0020000

/* c_oflag bits */
#define OPOST	0000001
#define OLCUC	0000002
#define ONLCR	0000004
#define OCRNL	0000010
#define ONOCR	0000020
#define ONLRET	0000040
#define OFILL	0000100
#define OFDEL	0000200
#define NLDLY	0000400
#define   NL0	0000000
#define   NL1	0000400
#define CRDLY	0003000
#define   CR0	0000000
#define   CR1	0001000
#define   CR2	0002000
#define   CR3	0003000
#define TABDLY	0014000
#define   TAB0	0000000
#define   TAB1	0004000
#define   TAB2	0010000
#define   TAB3	0014000
#define   XTABS	0014000
#define BSDLY	0020000
#define   BS0	0000000
#define   BS1	0020000
#define VTDLY	0040000
#define   VT0	0000000
#define   VT1	0040000
#define FFDLY	0100000
#define   FF0	0000000
#define   FF1	0100000

/* c_cflag bit meaning */
#define CBAUD	0010017
#define  B0	0000000		/* hang up */
#define  B50	0000001
#define  B75	0000002
#define  B110	0000003
#define  B134	0000004
#define  B150	0000005
#define  B200	0000006
#define  B300	0000007
#define  B600	0000010
#define  B1200	0000011
#define  B1800	0000012
#define  B2400	0000013
#define  B4800	0000014
#define  B9600	0000015
#define  B19200	0000016
#define  B38400	0000017
#define EXTA B19200
#define EXTB B38400
#define CSIZE	0000060
#define   CS5	0000000
#define   CS6	0000020
#define   CS7	0000040
#define   CS8	0000060
#define CSTOPB	0000100
#define CREAD	0000200
#define PARENB	0000400
#define PARODD	0001000
#define HUPCL	0002000
#define CLOCAL	0004000
#define CBAUDEX 0010000
#define    B57600 0010001
#define   B115200 0010002
#define   B230400 0010003
#define   B460800 0010004
#define   B500000 0010005
#define   B576000 0010006
#define   B921600 0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017
#define CIBAUD	  002003600000	/* input baud rate (not used) */
#define CMSPAR	  010000000000		/* mark or space (stick) parity */
#define CRTSCTS	  020000000000		/* flow control */

/* c_lflag bits */
#define ISIG	0000001
#define ICANON	0000002
#define XCASE	0000004
#define ECHO	0000010
#define ECHOE	0000020
#define ECHOK	0000040
#define ECHONL	0000100
#define NOFLSH	0000200
#define ECHOCTL	0001000
#define ECHOPRT	0002000
#define ECHOKE	0004000
#ifdef __mips__
#define IEXTEN	0000400
#define FLUSHO	0020000
#define TOSTOP	0100000
#else
#define TOSTOP	0000400
#define FLUSHO	0010000
#define IEXTEN	0100000
#endif
#define PENDIN	0040000

/* tcflow() and TCXONC use these */
#define TCOOFF		0
#define TCOON		1
#define TCIOFF		2
#define TCION		3

/* tcflush() and TCFLSH use these */
#define TCIFLUSH	0
#define TCOFLUSH	1
#define TCIOFLUSH	2

/* tcsetattr uses these */
#ifdef __mips__
#define TCSANOW		0x540e
#define TCSADRAIN	0x540f
#define TCSAFLUSH	0x5410
#else
#define TCSANOW		0
#define TCSADRAIN	1
#define TCSAFLUSH	2
#endif

int winecrt_tcgetattr(int fd, struct termios *termios_p);
#define tcgetattr winecrt_tcgetattr

int winecrt_tcsetattr(int fd, int optional_actions, struct termios *termios_p);
#define tcsetattr winecrt_tcsetattr

speed_t winecrt_cfgetospeed(struct termios *termios_p);
#define cfgetospeed winecrt_cfgetospeed

int winecrt_cfsetospeed(struct termios *termios_p, speed_t speed);
#define cfsetospeed winecrt_cfsetospeed

speed_t winecrt_cfgetispeed(struct termios *termios_p);
#define cfgetispeed winecrt_cfgetispeed

int winecrt_cfsetispeed(struct termios *termios_p, speed_t speed);
#define cfsetispeed winecrt_cfsetispeed

void cfmakeraw(struct termios *t);

int winecrt_tcflush(int fd, int queue_selector);
#define tcflush winecrt_tcflush

int tcdrain(int fd);
int winecrt_tcflow (int fd,int action);
#define tcflow winecrt_tcflow

int tcsendbreak (int fd,int duration);

#endif