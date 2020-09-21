#include <math.h>
#include <stdbool.h>
#include <stdio.h>

double min(double x, double y)
{
    return (x < y) ? x : y;
}

double max(double x, double y)
{
    return (x > y) ? x : y;
}

static double sqr(double x)
{
    return pow(x, 2);
}

void ad9548_quantize_a(double a, int *qa0, int *qa1, int *qa2, int *qa3)
{
    int w = (a < 1) ? -ceil(log2(a)) : 0;
    int a1 = (a < 1) ? min(63, max(0, w)) : 0;
    int x = (a > 1) ? ceil(log2(a)) : 0;
    double y = (a > 1) ? min(22, max(0, x)) : 0;
    int a2 = (y >= 8) ? 7 : y;
    int a3 = (y >= 8) ? y-7 : 0;
    int z = round(a*pow(2, 16+a1-a2-a3));
    int a0 = min(65535, max(1, z));
    double aq = a0 * pow(2, -16-a1+a2+a3);
    printf("alpha: %d, %d, %d, %d\n", a0, a1, a2, a3);
    printf("  quantization error %e\n", (aq-a)/a);
    *qa0 = a0;
    *qa1 = a1;
    *qa2 = a2;
    *qa3 = a3;
}

void ad9548_quantize_bg(double b, int *qb0, int *qb1)
{
    b = fabs(b);
    int x = -ceil(log2(b));
    int b1 = min(31, max(0, x));
    int y = round(b * pow(2, 17+b1));
    int b0 = min(131071, max(1, y));
    printf("beta: %d, %d\n", b0, b1);
    double bq = b0 * pow(2, -17-b1);
    printf("  quantization error %e\n", (bq-b)/b);
    *qb0 = b0;
    *qb1 = b1;
}

void ad9548_quantize_d(double d, int *qd0, int *qd1)
{
    int x = -ceil(log2(d));
    int d1 = min(31, max(0, x));
    int y = round(d * pow(2, 15+d1));
    int d0 = min(32767, max(1, y));
    printf("delta: %d, %d\n", d0, d1);
    double dq = d0 * pow(2, -15-d1);
    printf("  quantization error %e\n", (dq-d)/d);
    *qd0 = d0;
    *qd1 = d1;
}

/**
 * @brief ad9548_design_iir
 * @param fs system clock frequency, Hz
 * @param D PLL feedback divide ratio
 * @param fp open loop bandwidth, Hz
 * @param theta phase margin, radians
 * @param foffset frequency offset, Hz
 * @param atten desired attenuation in dB at offset from PLL output frequency
 */
bool ad9548_design_iir(double fs, double D, double fp, double theta,
    double foffset, double atten, double coef[4])
{
    double wp = 2 * M_PI * fp;
    double T1 = (1-sin(theta)) / (wp * cos(theta));
    double woffset = 2 * M_PI * foffset;
    double T3 = sqrt(pow(10, atten/10) - 1) / woffset;
    if (T3 > 1/(5*fp)) {
        printf("error\n");
        return false;
    }

    double T1T3 = T1*T3 + sqr(T1+T3);
    double wc = (T1+T3)*tan(theta) / T1T3 *
                (sqrt(1+T1T3/sqr((T1+T3)*tan(theta))) - 1);

    double T2 = 1./(sqr(wc) * (T1+T3));
    double K = 30517578125.0 / pow(2, 33) * fs;

    coef[0] = (sqr(wc) * T2 * D) / (T1*K) *
              sqrt((1+sqr(wc*T1)) * (1+sqr(wc*T3)) / (1+sqr(wc * T2)));
    coef[1] = -32. / fs * (1/T1 - 1/T2);
    coef[2] = -32. / (fs * T1);
    coef[3] = 32. / (fs * T3);

    printf("T1 %e\n", T1);
    printf("T2 %e\n", T2);
    printf("T3 %e\n", T3);
    printf("1/5fp %e\n", 1./5/fp);
    printf("fp %e\n", wp / 2 / M_PI);
    printf("fc %e\n", wc / 2 / M_PI);
    return true;
}

int main()
{
//    double a = 0.012735446;
//    double b = -6.98672e-5;
//    double g = -7.50373e-5;
//    double d = 0.002015399;
    double a =  0.106401443481445;
    double b = -0.000266179442406;
    double g = -0.000299416482449;
    double d =  0.002159833908081;

    int a0, a1, a2, a3, b0, b1, g0, g1, d0, d1;
    ad9548_quantize_a(a, &a0, &a1, &a2, &a3);
    ad9548_quantize_bg(b, &b0, &b1);
    ad9548_quantize_bg(g, &g0, &g1);
    ad9548_quantize_d(d, &d0, &d1);

    double fs = 933.12e6;
    double D = 1500;
    double fp = 500;
    double theta = 88./180. * M_PI;
    double atten = 3;
    double foffset = 2500;
    double coef[4];
    ad9548_design_iir(fs, D, fp, theta, foffset, atten, coef);
    printf("coef:\n  %.15f\n  %.15f\n  %.15f\n  %.15f\n",
           coef[0], coef[1], coef[2], coef[3]);
}
