* Zdzich 4 Biblioteka do pisania Screen Saver'ow
* Jelcyn Rzeszow 1999
* Biblioteka ta wymaga zmiennych : %PX1, %PX2, PY1 %PY2 - pozycje kursora myszy
* Opis procedur
*      OPROZNIJBUFOR() - oproznia bufor klawiatury, umieszczac przed glowna 
*                        petla, aby zaden znak sie nie zaplatal i nie zakonczyl
*                        programu
*      MYSZUR() - Podaje pozycje myszy, umieszczac na samym poczatku SS,
*                 przed glowna petla
*      RUCHMYSZY() - Jesli uzytkownik poruszy mysza, to zakoncz program
*      KONIEC() - Konczy program, wlacza tryb tekstowy
*      CZYKLAWISZ() - Jesli uzytkownik nacisnie jakis klawisz, to konczy program

Zmienna %PX1
Zmienna %PX2
Zmienna %PY1
Zmienna %PY2

Procedura OPROZNIJBUFOR()
AH=$0C
Przerwanie $21
Koniec OPROZNIJBUFOR()

Procedura MYSZUR()
* Procedura zapisuje poczatkowa pozycje myszy 
StanPrzyciskow
%PX2=CX
%PY2=DX
Koniec MYSZUR()

Procedura RUCHMYSZY()
* Procedura konczy program po kliknieciu przyciskiem, lub ruszeniem myszy
StanPrzyciskow
Porownaj BX,0
<>Skok :Ruchm
* Ruch myszy
%PX1=CX
%PY1=DX
Porownaj %PX1,0
&Skok :Ruchm
Porownaj %PY1,0
&Skok :Ruchm
Skok :dal5
:Ruchm
KONIEC()
:dal5
%PY2=%PY1
%PX2=%PX1
Koniec RUCHMYSZY()

Procedura KONIEC()
* Procedura konczy program
TRYB $3
czekaj 3
OPROZNIJBUFOR()
KONIEC
Koniec KONIEC()

Procedura CZYKLAWISZ()
* Jesli nacisnoles klawisz, to ta procedura konczy program
  AH=$0B
  PRZERWANIE $21
  POROWNAJ AL,0
  <>SKOK :KON
  Skok :CD
  :KON
  KONIEC()
  :CD
Koniec CZYKLAWISZ()