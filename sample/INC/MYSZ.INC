* Biblioteka do obslugi MYSZY
* Jelcyn Rzeszow 1999
* Procedury nie wymagaja zadnych etykiet
* Oto ich opisy
* LEWY() - przerywa dzialanie programu dopoki nie klikniesz lewym p. myszy
* PRAWY() -    "         "       "       "     "      "     prawym p. myszy
* OBYDWA() -   "         "       "       "     "      "     dwoma 
*                 ( jednoczesnie ) przyciskami myszy
* 
* Biblioteka powieksza program o 180 B

Procedura AUTOR()
 PiszL Biblioteka MYSZ.INC
 Piszl autor biblioteki - Jelcyn Rzeszow 1999
Koniec AUTOR()

Procedura LEWY()
PokazMysz
:LW
StanPrzyciskow
Porownaj BX,1
&Skok :LWOK
Skok :LW
:LWOK
Koniec LEWY()


Procedura PRAWY()
PokazMysz
:PW
StanPrzyciskow
Porownaj BX,2
&Skok :PWOK
Skok :PW
:PWOK
Koniec PRAWY()


Procedura OBYDWA()
PokazMysz
:OBA
StanPrzyciskow
Porownaj BX,3
&Skok :OBAOK
Skok :OBA
:OBAOK
Koniec OBYDWA()

Koniec