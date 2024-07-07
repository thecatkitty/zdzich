# ZD4 - kompilator zgodny ze ZDZICH4

Program ZD4 stanowi jak najbliższe odzwierciedlenie kompilatora [ZDZICH4](https://www.jelcyn.com/dos/zdzich.htm) z 1997 roku.
Wytwarza pliki COM dla systemu DOS.

## Użycie

```
zd4 [plik.zdi|*] [/Ikatalog1 [/Ikatalog2 [...]]]
```

Nazwę pliku wyjściowego stanowi nazwa pliku wejściowego z rozszerzeniem zamienionym na `.com`.
W razie niepodania nazwy pliku źródłowego kompilator poprosi o jej wprowadzenie poprzez standardowe wejście.

W przypadku podania asterysku (`*`) program wyświetli listę plików `.zdi` w katalogu roboczym.

Za pomocą argumentu `/I` można podać ścieżki katalogów z dołączanymi plikami,
wpływając tym samym na działanie dyrektywy `#Wstaw`.

Argumenty wywołania programu można również umieścić w pliku `zd.cfg` w katalogu roboczym,
pojedynczo w kolejnych wierszach.
