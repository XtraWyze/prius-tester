#include "grading.h"

char calculateGrade(float capacity)
{
    if (capacity >= 5500)
        return 'A';

    if (capacity >= 5000)
        return 'B';

    if (capacity >= 4500)
        return 'C';

    if (capacity >= 4000)
        return 'D';

    return 'F';
}