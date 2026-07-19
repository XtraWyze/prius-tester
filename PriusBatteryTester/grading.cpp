#include "grading.h"
#include "config.h"

char calculateGrade(float capacity)
{
    if (capacity >= GRADE_A_MAH)
        return 'A';

    if (capacity >= GRADE_B_MAH)
        return 'B';

    if (capacity >= GRADE_C_MAH)
        return 'C';

    if (capacity >= GRADE_D_MAH)
        return 'D';

    return 'F';
}

float calculateSOH(float capacity)
{
    if (NOMINAL_CAPACITY_MAH <= 0.0f)
        return 0.0f;

    float soh = (capacity / NOMINAL_CAPACITY_MAH) * 100.0f;

    if (soh > 100.0f)
        soh = 100.0f;

    if (soh < 0.0f)
        soh = 0.0f;

    return soh;
}
