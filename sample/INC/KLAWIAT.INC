* Biblioteka obslugi klawiatury do ZDZICHa 4
* Jelcyn     Rzeszow 1999 
* Biblioteka ta nie wymaga zadnych etykiet
* Spis procedur
* SPACJA() - Czeka dopoki nie nacisniesz spacje
* ENTER() - Czeka dopoki nie nacisniesz enter
* SPACJAESC() - Jesli nacisniesz Spacje, to idziesz dalej a ESC konczy program
* ENTERESC() -   -       -       enter   -    -      -    -  -    -       -
* ESC() - Klawisz ESC konczy program. Inne klawisze wychodza z procedury
* Biblioteka powieksza program o 224 B

Procedura AUTOR()
Piszl Biblioteka obslugi klawiatury
Piszl Autor biblioteki : Jelcyn Rzeszow 1999
koniec AUTOR()



Procedura SPACJA()
:p1
klawisz
porownaj AL,32
&skok :sp
skok :p1
:sp
Koniec SPACJA()

Procedura ENTER()
:p2
klawisz
porownaj AL,13
&skok :EN
skok :p2
:En
koniec ENTER()

Procedura SPACJAESC()
:p3
klawisz
porownaj AL,27
&skok :koni
porownaj AL,32
&Skok :Dal
skok :p3
:koni
koniec
:Dal
koniec SPACJAESC()

Procedura ENTERESC()
:p4
klawisz
porownaj AL,27
&skok :koni2
porownaj AL,13
&Skok :Dal2
skok :p4
:koni2
koniec
:Dal2
koniec ENTERESC()

Procedura ESC()
:p5
Klawisz
Porownaj AL,27
&skok :ESCa
skok :dal3
:ESCa
koniec
:dal3
koniec ESC()


koniec