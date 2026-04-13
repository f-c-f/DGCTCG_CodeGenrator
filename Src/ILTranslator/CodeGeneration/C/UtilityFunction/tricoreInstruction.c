inline void qseed_f(float *inport, float *outport)
{
    __asm("qseed.f %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void cadd_i(int *condition, int *inport1, int *inport2, int *outport)
{
    __asm("cadd %0, %1, %2 \n" : "=d"(*outport) : "d"(*condtion), "d"(*inport1), "d"(*inport2));
}

inline void cadd_n(int *condition, int *inport1, int *inport2, int *outport)
{
    __asm("caddn %0, %1, %2 \n" : "=d"(*outport) : "d"(*condition), "d"(*inport1), "d"(*inport2));
}

inline void csub_i(int *condition, int *inport1, int *inport2, int *outport)
{
    __asm("csub %0, %1, %2 \n" : "=d"(*outport) : "d"(*condition), "d"(*inport1), "d"(*inport2));
}

inline void csub_n(int *condition, int *inport1, int *inport2, int *outport)
{
    __asm("csubn %0, %1, %2 \n" : "=d"(*outport) : "d"(*condition), "d"(*inport1), "d"(*inport2));
}

inline void cls(int *inport, int *outport)
{
    __asm("cls %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void clz(int *inport, int *outport)
{
    __asm("clz %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void clo(int *inport, int *outport)
{
    __asm("clo %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void adds_i(int *inport1, int *inport2, int *outport)
{
    __asm("adds %0, %1, %2 \n" : "=d"(*outport) : "d"(*inport1), "d"(*inport2));
}

inline void adds_u(unsigned int *inport1, unsigned int *inport2, unsigned int *outport)
{
    __asm("adds %0, %1, %2 \n" : "=d"(*outport) : "d"(*inport1), "d"(*inport2));
}

inline void subs_i(int *inport1, int *inport2, int *outport)
{
    __asm("subs %0, %1, %2 \n" : "=d"(*outport) : "d"(*inport1), "d"(*inport2));
}

inline void muls_i(int *inport1, int *inport2, int *outport)
{
    __asm("muls %0, %1, %2 \n" : "=d"(*outport) : "d"(*inport1), "d"(*inport2));
}

inline void abss_i(int *inport, int *outport)
{
    __asm("abss %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void parity(int *inport, int *outport)
{
    __asm("parity %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}

inline void popcnt(int *inport, int *outport)
{
    __asm("popcnt.w %0, %1 \n" : "=d"(*outport) : "d"(*inport));
}
