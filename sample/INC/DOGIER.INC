* Biblioteka do gier tekstowych, wersja 2.0
* (wymaga etykiet :Start i :Koniec w programie)
*
* Procedury:
* KLAWISZ()     - czeka na naci�ni�cie klawisza a po naci�ni�ciu Esc
*                 skacze do etykiety :CzyKoniec?
* ENERGIA()     - wy�wietla na pozycji BL,BH pasek energii
*                 Wymaga zmiennej %Energia zdefiniowanej w programie
* UKRYJKURSOR() - chowa kursor
* B�YSK()       - b�ysk na ekranie

:CzyKoniec?
Czy��
PiszL (N)owa gra czy (k)oniec?
Klawisz
Por�wnaj AL,110
&Skok :Start
Por�wnaj AL,78
&Skok :Start
Por�wnaj AL,107
&Skok :Koniec
Por�wnaj AL,75
&Skok :Koniec
Skok :CzyKoniec?

Procedura KLAWISZ()
	Klawisz
	Por�wnaj AL,27
	&Skok :CzyKoniec?
Koniec KLAWISZ()

Procedura ENERGIA()
	Pozycja BL,BH
	PiszZnak 3,12,%Energia
Koniec ENERGIA()

Procedura UKRYJKURSOR()
	Pozycja 0,25
Koniec UKRYJKURSOR()

Procedura B�YSK()
	Czy�� 40
	Czekaj 1
	Czy�� 100
	Czekaj 1
	Czy�� 127
	Czekaj 1
Koniec B�YSK()
