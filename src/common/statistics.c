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

#include "statistics.h"

void statistics_add_sample(statistics_t *stat, double value)
{
    if (!stat)
        return;

    // Welford's online algorithm
    stat->n++;
    const uint64_t n = stat->n;
    const double _mean = stat->_mean;
    const double M2 = stat->M2;
    const double M3 = stat->M3;
    const double M4 = stat->M4;
    double delta = value - _mean;
    double delta_n = delta / n;
    double delta_n2 = delta_n * delta_n;
    double term1 = delta * delta_n * (n - 1);
    stat->_mean += delta_n;
    stat->M4 = M4 + term1 * delta_n2 * (n*n - 3*n + 3) + 6 * delta_n2 * M2 - 4 * delta_n * M3;
    stat->M3 = M3 + term1 * delta_n * (n - 2) - 3 * delta_n * M2;
    stat->M2 = M2 + term1;
    double delta2 = value - _mean;
    stat->_sumvar += delta * delta2;

}
