# ZDC - kompilator rozwojowy

Program ZDC jest rozwojowym kompilatorem języka Zdzich.
Jest on wyposażony w tryby diagnostyczne pozwalające na śledzenie przebiegu kolejnych etapów przetwarzania.

Docelowo kompilator ZDC ma zostać wyposażony w nowoczesny generator zdolny tworzyć pliki wykonywalne dla współczesnych systemów operacyjnych.
Obecnie współdzieli on generator z kompilatorem [ZD4](zd4.md),
tym samym wytwarzając pliki COM dla systemu DOS.

## Użycie

```
zdc [-C|-L|-P|-h] [-e:kodowanie] [-o:wyjście] [-I:katalog1 [-I:katalog2 [...]]] [--|wejście]
```

Domyślnym trybem pracy jest pełna kompilacja (`-C`),
ale można również dla celów diagnostycznych ograniczyć się do samego parsowania (`-P`)
lub samej analizy leksykalnej (`-L`).
Wybranie `-h` poskutkuje wyświetleniem krótkiej pomocy.

W przypadku niepodania nazwy pliku źródłowego (lub wpisania `--`) kompilator będzie przetwarzał standardowe wejście.

Jeżeli program ZDC nie jest w stanie poprawnie rozpoznać kodowania znaków zastosowanego w pliku źródłowym,
należy do wywołania programu dodać opcję `-e:`:
- `-e:dos` - strona kodowa IBM 852 (DOS: Europa Środkowa) 
- `-e:iso` - kodowanie znaków ISO 8859-2 (ISO Latin-2)
- `-e:maz` - kod ZN-92 MAZOVIA
- `-e:utf` - system kodowania UTF-8
- `-e:win` - strona kodowa Windows 1250 (Windows: Europa Środkowa) 

W trybie kompilacji wymagane jest podanie ścieżki do pliku wynikowego poprzez opcję `-o:`.
W pozostałych trybach jest to opcjonalne.

Wywołując program w trybie kompilacji lub parsowania,
można za pomocą opcji `-I:` podać ścieżki katalogów z dołączanymi plikami,
wpływając tym samym na działanie dyrektywy `#Wstaw`.
