* Biblioteka dzwiekowa do jezyka programowania ZDZICH 4
* Jelcyn     Rzeszow 1999
* Biblioteka nie wymaga zadnych etykiet
* Oto opisy procedur
* GLOS() - Wlacza sygnal dzwiekowy ( trzeba go wylaczyc procedura CISZA() 
* Oto przyklad :
*                   DZWIEK()
*                   CZEKAJ n ( dowolna liczba naturalna - n/10 sekund)
*                   CISZA()
* CISZA() - Wylacza sygnaj dzwiekowy
* DZWIEKn - Wlacza glos na n sekund
* Jak dolaczyc ta biblioteke do programu ?
* Na koncu programu napiszmy #WSTAW DZWIEK.INC ( biblioteka ta musi by w
* biezacym katalogu jesli nie jest, to piszemy sciezke dostepu np. 
*  #WSTAW c:\INC\DZWIEK.INC )
* Aby wywolac procedure piszemy jej nazwe wielkimi literami z nawiasami ()
* np GLOS()
* czekaj 11
* CISZA()
* Po tych zabiegach biblioteka zostanie dolaczona do pliku COM ( nie bedzie 
* potem potrzebna )
* Biblioteka powieksza program o 359 B


Procedura AUTOR()
PiszL Biblioteka - DZWIEK.INC
PiszL autor biblioteki - Jelcyn Rzeszow 1999
koniec AUTOR()

Procedura GLOS()
        DX=$61
        AL=$FF
        DoPortu
Koniec GLOS()

Procedura CISZA()
        DX=$61
        AL=0
        DoPortu
Koniec CISZA()

procedura DZWIEK1()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 1
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK1()

procedura DZWIEK2()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 2
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK2()

procedura DZWIEK3()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 3
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK3()

procedura DZWIEK4()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 4
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK4()

procedura DZWIEK5()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 5
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK5()

procedura DZWIEK6()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 6
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK6()

procedura DZWIEK7()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 7
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK7()

procedura DZWIEK8()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 8
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK8()

procedura DZWIEK9()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 9
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK9()

procedura DZWIEK10()
        DX=$61
        AL=$FF
        DoPortu
        czekaj 10
        DX=$61
        AL=0
        DoPortu
Koniec DZWIEK10()

Koniec