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

#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct statistics_t {
    uint64_t n;
    double _mean;
    double _sumvar;  // sum of squared distances from the mean
    double M2;
    double M3;
    double M4;
} statistics_t;

#ifdef __cplusplus
}
#endif

void statistics_add_sample(statistics_t *stat, double value);
double statistics_variance(const statistics_t *stat);
double statistics_skewness(const statistics_t *stat);

#endif // STATISTICS_H
