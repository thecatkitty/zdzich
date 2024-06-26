# Zdzich
**Zdzich** to język programowania stworzony w 1995 roku przez [Pawła Piątkowskiego](https://github.com/cosi1), do którego w latach 1997-1999 dołączyli [Damian Daszkiewicz](https://github.com/DamianDaszkiewicz) i Piotr Niżyński.
Jego naczelną cechą jest wykorzystanie języka polskiego w słowach kluczowych.

*Matriksoft Zdzich 6* był moim projektem z 2013 r. mającym na celu zaimplementowanie kompilatora tego języka dla współczesnych systemów operacyjnych.
Projekt ten w używalnej formie został zaprezentowany na festiwalu naukowym „E(x)plory”, po którym zaś przepadł w wyniku awarii dysku twardego.

Niniejsze repozytorium ma na celu wskrzeszenie tego projektu - w szczególności zaimplementowanie kompilatora zgodnego ze ZDZICH4 dla systemu DOS, Windows i Linux, jak również dalszy rozwój języka Zdzich.

## Zgodność z korpusem
| Przykład | Lekser | Parser | ZD4 |
| -------- | ------ | ------ | --- |
| **KURS** | - | - | - |
| KURS01 | ✅ | ✅ | ✅ |
| KURS02 | ✅ | ✅ | ✅ |
| KURS03 | ✅ | ✅ | ✅ |
| KURS04 | ✅ | ✅ | ✅ |
| EFEKT | ✅ | ✅ | ✅ |
| LEKCJA01 | ✅ | ✅ | ✅ |
| LEKCJA02 | ✅ | ✅ | ✅ |
| LEKCJA03 | ✅ | ✅ | ✅ |
| LEKCJA04 | ✅ | ✅ | ✅ |
| LEKCJA05 | ✅ | ✅ | ✅ |
| LEKCJA06 | ✅ | ✅ | ✅ |
| LEKCJA07 | ✅ | ✅ | ✅ |
| LEKCJA08 | ✅ | ✅ | ✅ |
| LEKCJA09 | ✅ | ✅ | ✅ |
| LEKCJA10 | ✅ | ✅ | ✅ |
| LEKCJA11 | ✅ | ✅ | ✅ |
| ZADANIA | ✅ | ✅ | ✅ |
| **INC** | - | - | - |
| DOGIER | ✅ | ✅ | ✅ |
| DZWIEK | ✅ | ✅ | ✅ |
| DZWIEK2 | ✅ | ✅ | ✅ |
| KLAWIAT | ✅ | ✅ | ✅ |
| MYSZ | ✅ | ✅ | ✅ |
| SAVER | ✅ | ✅ | ⚠️[^1] |
| **PRZYKLAD** | - | - | - |
| DOWCIPY | ✅ | ✅ | ✅ |
| GRA | ✅ | ✅ | ✅ |
| KWADRAT | ✅ | ✅ | ✅ |
| LINIA | ✅ | ✅ | ✅ |
| PETLE | ✅ | ✅ | ✅ |
| ZC | ✅ | ✅ | ⚠️[^2] |
| !TLO | ✅ | ✅ | ✅ |
| ALARM | ✅ | ✅ | ✅ |
| ASCII | ✅ | ✅ | ✅ |
| COOL-GR1 | ✅ | ✅ | ✅ |
| DEMO | ✅ | ✅ | ⚠️[^3] |
| DZWIEK | ✅ | ✅ | ✅ |
| EFEKT | ✅ | ✅ | ✅ |
| LOSOWE | ✅ | ✅ | ✅ |
| MYSZ | ✅ | ✅ | ✅ |
| MYSZKA | ✅ | ✅ | ✅ |
| MYSZRYS | ✅ | ✅ | ✅ |
| PARAMETR | ✅ | ✅ | ✅ |
| PLIKI1 | ✅ | ✅ | ✅ |
| PLIKI2 | ✅ | ✅ | ✅ |
| SKOKI | ✅ | ✅ | ✅ |
| TESTMYSZ | ✅ | ✅ | ✅ |
| WSTAW | ✅ | ✅ | ✅ |

[^1]: Wymagane dostosowanie do skompilowanego przykładu dołączonego do ZDZICH5.
[^2]: Wiodące spacje utracone w nieregularnych ciągach tekstowych.
[^3]: Wymagana zmiana zduplikowanej nazwy.
