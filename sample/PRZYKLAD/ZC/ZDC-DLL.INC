*             Biblioteka do programu ZDZICH COMMANDER v 1.0
*******************************************************************************








* Extra efekt !!!
Procedura COOL()
Zmienna %Kw1
Zmienna %Kw2
Zmienna %Col
%Kw1=0
%Kw2=0
%Col=3
Tryb $12

:Yk
%Kw1=0
:Xk
punkt %kw1,%Kw2,%Col
zwieksz %Kw1,1
KOL()
Porownaj %Kw1,639
&Skok :nast
skok :Xk
:nast
nic
Zwieksz %Kw2,1
Porownaj %Kw2,479
&Skok :nest
Skok :Yk

* Zmien kolor
Procedura KOL()
Zwieksz %Col,1
Porownaj %COL,16
&Skok :min
skok :ll
:min
%COL=0
:ll
Koniec KOL()

:nest
Koniec COOL()


* Pokaz napis ZDZICH COMMANDER
Procedura LOGOZC()
pozycja 26,14
czekaj 1
pisz Z
czekaj 2
pisz d
czekaj 2
pisz z
czekaj 2
pisz i
czekaj 2
pisz c
czekaj 2
pisz h
czekaj 2
pisz  
czekaj 2
pisz C
czekaj 2
pisz o
czekaj 2
pisz m
czekaj 2
pisz m
czekaj 2
pisz a
czekaj 2
pisz n
czekaj 2
pisz d
czekaj 2
pisz e
czekaj 2
pisz r
czekaj 2
pisz  
czekaj 5
pisz v
czekaj 2
pisz  
czekaj 2
pisz 1
czekaj 2
pisz .
czekaj 2
pisz 0
czekaj 9
Koniec LOGOZC()

*Menu
Procedura MENUZC()
czysc
piszl
piszl   OPCJE :
piszl   --------
piszl   1)   Zmien nazwe pliku
piszl   2)   Usun plik
piszl   3)   Nowy katalog
piszl   4)   Usun pusty katalog
piszl   5)   Przejdz do innego katalogu
piszl   6)   Nowy plik
piszl
Piszl   0)   Koniec
piszl
pisz Wybieram opcje numer >> 
klawisz

* Skok do wybranych opcji
porownaj AL,27
&Skok :Opcja0

porownaj AL,48
&Skok :Opcja0

porownaj AL,49
&Skok :Opcja1

porownaj AL,50
&Skok :Opcja2

porownaj AL,51
&Skok :Opcja3

porownaj AL,52
&Skok :Opcja4

porownaj AL,53
&Skok :Opcja5

porownaj AL,54
&Skok :Opcja6
Skok :Start
Koniec MENUZC()
