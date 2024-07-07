# Zdzich
**Zdzich** to język programowania stworzony w 1995 roku przez [Pawła Piątkowskiego](https://github.com/cosi1), do którego w latach 1997-1999 dołączyli [Damian Daszkiewicz](https://github.com/DamianDaszkiewicz) i Piotr Niżyński.
Jego naczelną cechą jest wykorzystanie języka polskiego w słowach kluczowych.

*Matriksoft Zdzich 6* był moim projektem z 2013 r. mającym na celu zaimplementowanie kompilatora tego języka dla współczesnych systemów operacyjnych.
Projekt ten w używalnej formie został zaprezentowany na festiwalu naukowym „E(x)plory”, po którym zaś przepadł w wyniku awarii dysku twardego.

Niniejsze repozytorium ma na celu wskrzeszenie tego projektu - w szczególności zaimplementowanie kompilatora zgodnego ze ZDZICH4 dla systemu DOS, Linux, macOS i Windows, jak również dalszy rozwój języka Zdzich.

## Stan projektu

[![Build](https://github.com/thecatkitty/zdzich/actions/workflows/build.yml/badge.svg?branch=main&event=push)](https://github.com/thecatkitty/zdzich/actions/workflows/build.yml)

Program jest dostępny pod każdą z obsługiwanych platform w dwóch wariantach:
1. Rozwojowy kompilator [ZDC](docs/zdc.md).
2. Kompilator [ZD4](docs/zd4.md) zgodny ze ZDZICH4.

Najnowsza wersja poprawnie kompiluje wszystkie przykłady dołączone pierwotnie do pakietów ZDZICH4 i ZDZICH5 - szczegóły są zamieszczone w notatce o [zgodności z korpusem](docs/zgodność.md).
