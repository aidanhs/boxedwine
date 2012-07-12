#include <math.h>

int _fltused = 1;

double hal_pow(double x, double y)
{
	return pow(x, y);
}

double hal_atan(double x)
{
	return atan(x);
}

double hal_ceil(double x)
{
	return ceil(x);
}

double hal_cos(double x)
{
	return cos(x);
}

double hal_fabs(double x)
{
	return fabs(x);
}

double hal_floor(double x)
{
	return floor(x);
}

double hal_log(double x)
{
	return log(x);
}

double hal_sin(double x)
{
	return sin(x);
}

double hal_sqrt(double x)
{
	return sqrt(x);
}

double hal_tan(double x)
{
	return tan(x);
}

double hal_atan2(double y, double x)
{
	return atan2(y, x);
}

double hal_hypot(double x, double y)
{
	return hypot(x, y);
}