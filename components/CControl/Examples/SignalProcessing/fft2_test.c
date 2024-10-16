/*
 ============================================================================
 Name        : fft2.c
 Author      : <Your Name Here>
 Version     : 1.0
 Copyright   : MIT
 Description : FFT2 on a matrix
 ============================================================================
 */

#include "ccontrol.h"

#define row    3
#define column 3

int CControl_Test()
{
    clock_t start, end;
    float   cpu_time_used;
    start = clock();

    /* Create XR and XI */
    float XR[row * column] = {-0.6485, 0.2851, 1.3475,
                              -0.6743, -0.1499, -1.5549,
                              1.4951, 0.4504, -0.4982};
    float XI[row * column];

    /* Do FFT2 */
    fft2(XR, XI, row, column);

    /* Print */
    print(XR, row, column);
    print(XI, row, column);

    end           = clock();
    cpu_time_used = ((float)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTotal speed  was %f\n", cpu_time_used);

    return EXIT_SUCCESS;
}