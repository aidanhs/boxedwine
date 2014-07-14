package wine.builtin.libm;

import wine.emulation.Memory;
import wine.system.WineThread;

public class Math {
    static public float acosf(float x) {
        return (float)java.lang.Math.acos(x);
    }

    static public double atan(double x) {
        return java.lang.Math.atan(x);
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

    static public double hypot(double x, double y) {
        return java.lang.Math.hypot(x, y);
    }

    static public double log(double x) {
        return java.lang.Math.log(x);
    }

    static public double log10 (double x) {
        return java.lang.Math.log10(x);
    }

    static public float logf(float x) {
        return (float)java.lang.Math.log((double)x);
    }

    static public double pow(double x, double y) {
        return java.lang.Math.pow(x, y);
    }

    static public double sin(double x) {
        return java.lang.Math.sin(x);
    }

    static public void sincos(double x, int psin, int pcos) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writeq(psin, Double.doubleToRawLongBits(sin(x)));
        memory.writeq(pcos, Double.doubleToRawLongBits(cos(x)));
    }

    static public double tan(double x) {
        return java.lang.Math.tan(x);
    }

    static public double sqrt(double x) {
        return java.lang.Math.sqrt(x);
    }

    static public float sqrtf(float x) {
        return (float)java.lang.Math.sqrt(x);
    }
}
