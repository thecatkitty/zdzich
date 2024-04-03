#pragma once

#define Zdzich__Skok goto

#define Zdzich__SkokR                                                          \
    if (0 == __wynik)                                                          \
    goto

#define Zdzich__SkokNR(val)                                                    \
    if (0 != __wynik)                                                          \
    goto

#define Zdzich__SkokW(val)                                                     \
    if (0 < __wynik)                                                           \
    goto

#define Zdzich__SkokM(val)                                                     \
    if (0 > __wynik)                                                           \
    goto

#define Zdzich__SkokWR(val)                                                    \
    if (0 <= __wynik)                                                          \
    goto

#define Zdzich__SkokMR(val)                                                    \
    if (0 >= __wynik)                                                          \
    goto
