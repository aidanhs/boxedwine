package wine.builtin.libm;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class Math {
    static public double acos(double x) {
        return java.lang.Math.acos(x);
    }

    static public float acosf(float x) {
        return (float)java.lang.Math.acos(x);
    }

    static public double asin(double x) {
        return java.lang.Math.asin(x);
    }

    static public float asinf(float x) {
        return (float)java.lang.Math.asin(x);
    }

    static public double atan(double x) {
        return java.lang.Math.atan(x);
    }

    static public float atanf(float x) {
        return (float)java.lang.Math.atan(x);
    }

    static public double atan2(double y, double x) {
        return java.lang.Math.atan2(y, x);
    }

    static public double cbrt(double x) {
        return java.lang.Math.cbrt(x);
    }

    static public double ceil(double x) {
        return java.lang.Math.ceil(x);
    }

    static public float ceilf(float x) {
        return (float)java.lang.Math.ceil(x);
    }

    static public double cos(double x) {
        return java.lang.Math.cos(x);
    }

    static public float cosf(float x) {
        return (float)java.lang.Math.cos(x);
    }

    static public double cosh(double x) {
        return java.lang.Math.cosh(x);
    }

    static public double exp(double x) {
        return java.lang.Math.exp(x);
    }

    static public float expf(float x) {
        return (float)java.lang.Math.exp(x);
    }

    static public double fabs(double x) {
        return java.lang.Math.abs(x);
    }

    static public double floor(double x) {
        return java.lang.Math.floor(x);
    }

    static public float floorf(float x) {
        return (float)java.lang.Math.floor(x);
    }

    static public double fmod(double numer, double denom) {
        return numer % denom;
    }

    static public int __fpclassify(float x) {
        Log.panic("__fpclassify not implemented");
        return 0;
    }

    static public double frexp(double num, int exp) {
        Log.panic("frexp not implemented");
        return 0;
    }

    static public double hypot(double x, double y) {
        return java.lang.Math.hypot(x, y);
    }

    static public float hypotf(float x, float y) {
        return (float)java.lang.Math.hypot(x, y);
    }

    static public int __isinf(double f) {
        if (Double.isInfinite(f))
            return 1;
        return 0;
    }

    static public int __isinff(float f) {
        if (Float.isInfinite(f))
            return 1;
        return 0;
    }

    static public int __isnan(double f) {
        if (Double.isNaN(f))
            return 1;
        return 0;
    }

    static public int __isnanf(float f) {
        if (Float.isNaN(f))
            return 1;
        return 0;
    }

    static public double j0(double x) {
        Log.panic("j0 not implemented");
        return 0;
    }

    static public double j1(double x) {
        Log.panic("j1 not implemented");
        return 0;
    }

    static public double jn(int n, double x) {
        Log.panic("jn not implemented");
        return 0;
    }

    static public double ldexp(double x, int exp) {
        Log.panic("ldexp not implemented");
        return x*Math.exp(exp);
    }

    static public float ldexpf(float x, int exp) {
        return (float)ldexp(x, exp);
    }

    static public double log(double x) {
        return java.lang.Math.log(x);
    }

    static public double log10 (double x) {
        return java.lang.Math.log10(x);
    }

    static public float log10f (float x) {
        return (float)java.lang.Math.log10(x);
    }

    static public double logb(double x) {
        Log.panic("logb not implemented");
        return java.lang.Math.log(x) / java.lang.Math.log(2);
    }

    static public float logf(float x) {
        return (float)java.lang.Math.log((double)x);
    }

    static public double modf(double x, int iptr) {
        int intVal = (int)x;
        if (iptr!=0)
            WineThread.getCurrent().process.memory.writeq(iptr, Double.doubleToRawLongBits((double)intVal));
        return x - intVal;
    }

    static public double nextafter(double x, double y) {
        return java.lang.Math.nextAfter(x, y);
    }

    static public double pow(double x, double y) {
        return java.lang.Math.pow(x, y);
    }

    static public float powf(float x, float y) {
        return (float)java.lang.Math.pow(x, y);
    }

    static public double sin(double x) {
        return java.lang.Math.sin(x);
    }

    static public float sinf(float x) {
        return (float)java.lang.Math.sin(x);
    }

    static public double sinh(double x) {
        return java.lang.Math.sinh(x);
    }

    static public void sincos(double x, int psin, int pcos) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writeq(psin, Double.doubleToRawLongBits(sin(x)));
        memory.writeq(pcos, Double.doubleToRawLongBits(cos(x)));
    }

    static public double sqrt(double x) {
        return java.lang.Math.sqrt(x);
    }

    static public float sqrtf(float x) {
        return (float)java.lang.Math.sqrt(x);
    }

    static public double tan(double x) {
        return java.lang.Math.tan(x);
    }

    static public float tanf(float x) {
        return (float)java.lang.Math.tan(x);
    }

    static public double tanh(double x) {
        return java.lang.Math.tanh(x);
    }

    static public double y0(double x) {
        Log.panic("y0 not implemented");
        return 0;
    }

    static public double y1(double x) {
        Log.panic("y1 not implemented");
        return 0;
    }

    static public double yn(int n, double x) {
        Log.panic("yn not implemented");
        return 0;
    }
}
