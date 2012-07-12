typedef unsigned int	speed_t;

int hal_tcgetattr(int fd, struct termios *termios_p)
{
	return 0;
}

int hal_tcsetattr(int fd, int optional_actions, struct termios *termios_p)
{
	return 0;
}

speed_t hal_cfgetospeed(struct termios *termios_p)
{
	return 0;
}

int hal_cfsetospeed(struct termios *termios_p, speed_t speed)
{
	return 0;
}

int hal_cfsetispeed(struct termios *termios_p, speed_t speed)
{
	return 0;
}

int hal_tcflush(int fd, int queue_selector)
{
	return 0;
}

int hal_tcflow (int fd,int action)
{
	return 0;
}