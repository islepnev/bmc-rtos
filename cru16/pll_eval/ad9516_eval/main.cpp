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

#include <iostream>
#include <cstdio>
#include <cmath>

using namespace std;
const double freq_ref = 20e6;
const double freq_out_required = 125e6;
const double eps = 1e-3;
int main() {

    bool found = false;
    for (int r=1; r <= 10; r++) {
        if (found)
            break;
        for (int p=8; p <= 32; p *= 2)
            for (int a=0; a <= 100; a++)
                for (int b=3; b <= 100; b++)
                {
                    if (a >= b)
                        continue;
                    const int n = p * b + a;
                    const double freq_rdiv = freq_ref / r;
                    const double freq_vco = freq_ref / r * n;
                    if (freq_vco < 1450e6 || freq_vco > 1800e6)
                        continue;
                    const double freq_prescaler = freq_vco / p;
                    if (freq_prescaler > 300e6)
                        continue;
                    const double freq_n = freq_vco / n;
                    if (abs(freq_rdiv - freq_n) > eps)
                        continue;
                    const double pfd = freq_rdiv;
                    for (int vco_div = 2; vco_div <= 6; vco_div++) {
                        const double freq_dist = freq_vco / vco_div;
                        for (int out_div = 1; out_div <= 32; out_div++) {
                            const double freq_out = freq_dist / out_div;
                            if (abs(freq_out - freq_out_required) > eps)
                                continue;
                            cout << "R " << r
                                 << ", P " << p
                                 << ", A " << a
                                 << ", B " << b
                                 << ", vco_div " << vco_div
                                 << ", out_div " << out_div
                                 << ", pfd " << pfd << endl;
                            found = true;
                        }
                    }
                }
    }
    return 0;
}
