#ifndef __MATH_H__
#define __MATH_H__

#define M_PI_4 0.785398163397448309616

double winecrt_pow(double x, double y);
#define pow winecrt_pow

double winecrt_atan(double x);
#define atan winecrt_atan

double winecrt_ceil(double x);
#define ceil winecrt_ceil

double winecrt_cos(double x);
#define cos winecrt_cos

double winecrt_fabs(double x);
#define fabs winecrt_fabs

double winecrt_floor(double x);
#define floor winecrt_floor

double winecrt_log(double x);
#define log winecrt_log

double winecrt_sin(double x);
#define sin winecrt_sin

double winecrt_sqrt(double x);
#define sqrt winecrt_sqrt

double winecrt_tan(double x);
#define tan winecrt_tan

double winecrt_atan2(double y, double x);
#define atan2 winecrt_atan2

double winecrt_hypot(double x, double y);
#define hypot winecrt_hypot

#endif