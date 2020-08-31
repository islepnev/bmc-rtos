/*
**    Copyright 2019 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ad9548_loop_filter_design.h"

#include <math.h>

static double min(double x, double y)
{
    return (x < y) ? x : y;
}

static double max(double x, double y)
{
    return (x > y) ? x : y;
}

static void ad9548_quantize_a(double a, int *qa0, int *qa1, int *qa2, int *qa3)
{
    int w = (a < 1) ? -ceil(log2(a)) : 0;
    int a1 = (a < 1) ? min(63, max(0, w)) : 0;
    int x = (a > 1) ? ceil(log2(a)) : 0;
    double y = (a > 1) ? min(22, max(0, x)) : 0;
    int a2 = (y >= 8) ? 7 : y;
    int a3 = (y >= 8) ? y-7 : 0;
    int z = round(a*pow(2, 16+a1-a2-a3));
    int a0 = min(65535, max(1, z));
    // double aq = a0 * pow(2, -16-a1+a2+a3);
    // printf("alpha: %d, %d, %d, %d\n", a0, a1, a2, a3);
    // printf("  quantization error %e\n", (aq-a)/a);
    *qa0 = a0;
    *qa1 = a1;
    *qa2 = a2;
    *qa3 = a3;
}

static void ad9548_quantize_bg(double b, int *qb0, int *qb1)
{
    b = fabs(b);
    int x = -ceil(log2(b));
    int b1 = min(31, max(0, x));
    int y = round(b * pow(2, 17+b1));
    int b0 = min(131071, max(1, y));
    // printf("beta: %d, %d\n", b0, b1);
    // double bq = b0 * pow(2, -17-b1);
    // printf("  quantization error %e\n", (bq-b)/b);
    *qb0 = b0;
    *qb1 = b1;
}

static void ad9548_quantize_d(double d, int *qd0, int *qd1)
{
    int x = -ceil(log2(d));
    int d1 = min(31, max(0, x));
    int y = round(d * pow(2, 15+d1));
    int d0 = min(32767, max(1, y));
    // printf("delta: %d, %d\n", d0, d1);
    // double dq = d0 * pow(2, -15-d1);
    // printf("  quantization error %e\n", (dq-d)/d);
    *qd0 = d0;
    *qd1 = d1;
}

void ad9548_profile_set_iir(AD9548_Profile_TypeDef *p, const double coef[4])
{
    int a0, a1, a2, a3, b0, b1, g0, g1, d0, d1;
    ad9548_quantize_a(coef[0], &a0, &a1, &a2, &a3);
    ad9548_quantize_bg(coef[1], &b0, &b1);
    ad9548_quantize_bg(coef[2], &g0, &g1);
    ad9548_quantize_d(coef[3], &d0, &d1);
    p->b.filter_alpha_0_linear = a0;
    p->b.filter_alpha_1_exp = a1;
    p->b.filter_alpha_2_exp = a2;
    p->b.filter_alpha_3_exp = a3;
    p->b.filter_beta_0_linear = b0;
    p->b.filter_beta_1_exp = b1;
    p->b.filter_gamma_0_linear = g0;
    p->b.filter_gamma_1_exp = g1;
    p->b.filter_delta_0_linear = d0;
    p->b.filter_delta_1_exp = d1;
}
