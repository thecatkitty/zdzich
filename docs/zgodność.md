# Zgodność z korpusem

Najnowsza wersja kompilatora poprawnie parsuje wszystkie niezmodyfikowane przykłady dołączone do pakietów ZDZICH4 i ZDZICH5,
jak również poprawnie kompiluje wszystkie przykłady poza trzema opisanymi niżej.

Zdzich 6.4 nie implementuje całości funkcjonalności opisanej przez dokumentację dołączoną do ZDZICH4 i ZDZICH5,
a jedynie jej podzbiór wymagany do skompilowania dołączonych do nich przykładów.
Zdzich 6.5 rozszerzy korpus o zbiór testów jednostkowych,
pisanych już na podstawie owej dokumentacji i mających na celu wykrycie wszelkich braków implementacyjnych.

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
| SAVER | ✅ | ✅ | ⚠️<sup>1</sup> |
| **PRZYKLAD** | - | - | - |
| DOWCIPY | ✅ | ✅ | ✅ |
| GRA | ✅ | ✅ | ✅ |
| KWADRAT | ✅ | ✅ | ✅ |
| LINIA | ✅ | ✅ | ✅ |
| PETLE | ✅ | ✅ | ✅ |
| ZC | ✅ | ✅ | ⚠️<sup>2</sup> |
| !TLO | ✅ | ✅ | ✅ |
| ALARM | ✅ | ✅ | ✅ |
| ASCII | ✅ | ✅ | ✅ |
| COOL-GR1 | ✅ | ✅ | ✅ |
| DEMO | ✅ | ✅ | ⚠️<sup>3</sup> |
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

## Różnice względem ZDZICH5

1. **INC/SAVER:**
   Kod źródłowy dołączony do pakietu ZDZICH5 był błędny.
   Nie odpowiadał plikowi wykonywalnemu znajdującemu się w tym samym pakiecie.
   Odtworzono poprawny algorytm zastosowany w pliku wykonywalnym:
```diff
--- a/sample/INC/SAVER/SAVER.INC
+++ b/sample/INC/SAVER/SAVER.INC
@@ -36,9 +36,9 @@ Porownaj BX,0
 * Ruch myszy
 %PX1=CX
 %PY1=DX
-Porownaj %PX1,%PX2
+Porownaj %PX1,0
 &Skok :Ruchm
-Porownaj %PY1,%PY2
+Porownaj %PY1,0
 &Skok :Ruchm
 Skok :dal5
 :Ruchm
@@ -58,8 +58,8 @@ Koniec KONIEC()
 
 Procedura CZYKLAWISZ()
 * Jesli nacisnoles klawisz, to ta procedura konczy program
-  czekaj 4
-  OPROZNIJBUFOR()
+  AH=$0B
+  PRZERWANIE $21
   POROWNAJ AL,0
   <>SKOK :KON
   Skok :CD
```

2. **PRZYKLAD/ZC:**
   Wiodące spacje utracone w nieregularnych ciągach tekstowych.
   Ze wględu na zupełnie inne podejście do przetwarzania kodu źródłowego,
   niniejsza implementacja nie jest w stanie odzworować dokładnego kształtu *nieregularnych ciągów tekstowych* (ciągów nierozpoczynających się literą).
   Obsługa takich ciągów zostanie jeszcze bardziej ograniczona w późniejszych wersjach,
   gdy zaimplementowana zostanie składnia grodzonych ciągów jawnych.
   W wyniku tych różnic przykład ZC po skompilowaniu zachowuje się,
   jak gdyby wprowadzona została następująca zmiana:

```diff
--- a/sample/PRZYKLAD/ZC/ZDC-DLL.INC
+++ b/sample/PRZYKLAD/ZC/ZDC-DLL.INC
@@ -105,14 +105,14 @@ czysc
 piszl
 piszl   OPCJE :
 piszl   --------
-piszl   1)   Zmien nazwe pliku
-piszl   2)   Usun plik
-piszl   3)   Nowy katalog
-piszl   4)   Usun pusty katalog
-piszl   5)   Przejdz do innego katalogu
-piszl   6)   Nowy plik
+piszl 1)   Zmien nazwe pliku
+piszl 2)   Usun plik
+piszl 3)   Nowy katalog
+piszl 4)   Usun pusty katalog
+piszl 5)   Przejdz do innego katalogu
+piszl 6)   Nowy plik
 piszl
-Piszl   0)   Koniec
+Piszl 0)   Koniec
 piszl
 pisz Wybieram opcje numer >> 
 klawisz
 ```

3. **PRZYKLAD/DEMO:**
   Niniejsza implementacja w przeciwieństwie do oryginalnej stosuje jedną przestrzeń nazw dla wszystkich obiektów niezależnie od ich rodzaju.
   Wymagane było zatem usunięcie konfliktu nazw zmiennej i procedury w przykładzie DEMO:

```diff
--- a/sample/PRZYKLAD/DEMO.ZDI
+++ b/sample/PRZYKLAD/DEMO.ZDI
@@ -27,7 +27,7 @@ Pozycja 7,20
 Pisz Nacisnij dowolny klawisz...
 Klawisz
 Czyść 31
-IMIĘ()
+CZYTAJIMIE()
 Klawisz
 Tryb 0
 Czyść 52
@@ -94,10 +94,10 @@ Zamknij #5
 PiszL Zobacz plik DEMO.TXT w tym katalogu!!!
 Koniec
 
-Procedura IMIĘ()
+Procedura CZYTAJIMIE()
 	PiszL Jak masz na imie?
 	Czytaj $Imię
 	Pisz $Tekst
 	Pisz $Imię
 	PiszL !!!
-Koniec IMIĘ()
+Koniec CZYTAJIMIE()
```
