#ifndef FTOA_H
#define FTOA_H

#ifdef __cplusplus
 extern "C" {
#endif

// Converts a floating point number to string.
void ftoa(float n, char *str, int afterpoint);

#ifdef __cplusplus
}
#endif

#endif // FTOA_H
