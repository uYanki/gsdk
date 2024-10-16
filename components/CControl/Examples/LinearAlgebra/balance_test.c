/*
 ============================================================================
 Name        : balance.c
 Author      : Daniel Mårtensson
 Version     : 1.0
 Copyright   : MIT
 Description : Balance a matrix A
 ============================================================================
 */

#include "ccontrol.h"

int CControl_Test()
{
    /* Matrix A */
    float A[3 * 3] = {1.0000e+00, 1.0000e+02, 1.0000e+04,
                      1.0000e-02, 1.0000e+00, 1.0000e+02,
                      1.0000e-04, 1.0000e-02, 1.0000e+00};

    clock_t start, end;
    float   cpu_time_used;
    start = clock();
    balance(A, 3);
    end           = clock();
    cpu_time_used = ((float)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTotal speed  was %f\n", cpu_time_used);

    /* Print balanced matrix A */
    printf("A:\n");
    print(A, 3, 3);

    return EXIT_SUCCESS;
}

/*
 * GNU Octave code:
 * A = [1.0000e+00   1.0000e+02   1.0000e+04
        1.0000e-02   1.0000e+00   1.0000e+02
        1.0000e-04   1.0000e-02   1.0000e+00];

   balance(A)
 */
