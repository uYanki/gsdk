

#include "q15/clarke.h"
#include "q15/park.h"
#include "q15/svpwm.h"
#include "q15/sintbl.c"

#include "stdlib.h"
#include "stdio.h"

int main()
{
    u16 a[40];

    clarke_t clarke = {
        .q15PhA_i   = &a[0],
        .q15PhB_i   = &a[1],
        .q15PhC_i   = &a[2],
        .q15Alpha_o = &a[3],
        .q15Beta_o  = &a[4],
    };

    park_t park = {
        .q15Alpha_i   = &a[5],
        .q15Beta_i    = &a[6],
        .q15D_o       = &a[7],
        .q15Q_o       = &a[8],
        .u16ElecAng_i = &a[9],
    };

    ipark_t revpark = {
        .q15D_i       = &a[20],
        .q15Q_i       = &a[21],
        .u16ElecAng_i = &a[22],
        .q15Alpha_o   = &a[23],
        .q15Beta_o    = &a[24],
    };

    iclarke_t revclarke = {
        .q15Alpha_i = revpark.q15Alpha_o,
        .q15Beta_i  = revpark.q15Beta_o,
        .q15PhA_o   = &a[22],
        .q15PhB_o   = &a[23],
        .q15PhC_o   = &a[24],
    };

    sector_e b;

    svpwm_t svpwm = {
        .q15Ua_i      = revclarke.q15PhA_o,
        .q15Ub_i      = revclarke.q15PhB_o,
        .q15Uc_i      = revclarke.q15PhC_o,
        .eSector_o    = (sector_e*)&b,
        .q15Udc_i     = &a[13],
        .u16PwmPeriod = &a[14],
        .u16PwmCmpA_o = &a[15],
        .u16PwmCmpB_o = &a[16],
        .u16PwmCmpC_o = &a[17],
    };

    *revpark.q15D_i       = 1000;
    *revpark.q15Q_i       = 0;
    *revpark.u16ElecAng_i = 0;
    *svpwm.q15Udc_i       = 8724;
    *svpwm.u16PwmPeriod   = 8000;

    FILE* f = fopen("./a.csv", "w");

    for (u32 i = 0; i < 100000; i += 256)
    {
        *revpark.u16ElecAng_i = i % 65536;

        RevPark(&revpark);
        RevClarke(&revclarke);
        Svpwm(&svpwm);

        fprintf(f, "%d,%d,%d,%d,%d,%d\n", *revclarke.q15PhA_o, *revclarke.q15PhB_o, *revclarke.q15PhC_o, *svpwm.u16PwmCmpA_o, *svpwm.u16PwmCmpB_o, *svpwm.u16PwmCmpC_o);
    }

    fclose(f);

    return 0;
}