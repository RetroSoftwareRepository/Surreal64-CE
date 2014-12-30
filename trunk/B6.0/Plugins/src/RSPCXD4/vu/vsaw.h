/******************************************************************************\
* Authors:  Iconoclast                                                         *
* Release:  2013.11.26                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide. This software is distributed without any warranty.                *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/
#include "vu.h"

#ifdef VU_EMULATE_SCALAR_ACCUMULATOR_READ
static void VSAR(int vd, int vs, int vt, int e)
{
    short oldval[N];
    register int i;

    for (i = 0; i < N; i++)
        oldval[i] = VR[vs][i];
    vt = 0;
/* Even though VT is ignored in VSAR, according to official sources as well
 * as reversing, lots of games seem to specify it as non-zero, possibly to
 * avoid register stalling or other VU hazards.  Not really certain why yet.
 */
    e ^= 0x8;
/* Or, for exception overrides, should this be `e &= 0x7;` ?
 * Currently this code is safer because &= is less likely to catch oddities.
 * Either way, documentation shows that the switch range is 0:2, not 8:A.
 */
    if (e > 2)
    {
        message("VSAR\nInvalid mask.", 2);
        for (i = 0; i < N; i++)
            VR[vd][i] = 0x0000; /* override behavior (zilmar) */
    }
    else
        for (i = 0; i < N; i++)
            VR[vd][i] = VACC[e][i];
    for (i = 0; i < N; i++)
        VACC[e][i] = oldval[i]; /* ... = VS */
    return;
}
#endif

static void VSAW(int vd, int vs, int vt, int e)
{
    register int i;

    vs = 0; /* unused--old VSAR algorithm */
    vt = 0; /* unused but mysteriously set many times */
    if (vs | vt)
        return;
    e ^= 0x8; /* &= 7 */

    if (e > 0x2)
    { /* branch very unlikely...never seen a game do VSAW illegally */
        message("VSAW\nIllegal mask.", 2);
        for (i = 0; i < N; i++)
            VR[vd][i] = 0x0000; /* override behavior (zilmar) */
        return;
    }
    vector_copy(VR[vd], VACC[e]);
    return;
}
