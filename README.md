# Gra Makao
Projekt zespołowy na UML w C++.

Gra zrealizowana jest w C++ z wykorzystaniem bibliotek Qt. Jest to karciana gra 2D. Gra używa połączenia TCP/IP - jeden gracz (host) tworzy grę, a pozostali się dołączają. W grę może grać od 2 do 4 osób.  
Opis założeń gry https://pl.wikipedia.org/wiki/Makao_%28gra_karciana%29.

### Autorzy:
1. Jan Gwizdowski (Elleander, Odyseja)
2. Tadeusz Kleszcz (Elimas)
3. Aleksander Krzeszowski (akpl)
4. Sebastian Lipski (SirLipen)

### Wymagane oprogramowanie
1. Qt Creator (zalecana wersja Qt Creator - 3.6.1, based on Qt 5.6.0).  
https://www.qt.io/download-open-source/
2. Kompilator MSCV 2015 14.0 z Visual Studio.   
https://www.visualstudio.com/

### Instrukcja kompilacji
1. Sklonować repozytorium.
2. W programie Qt Creator otworzyć plik projektu `makao.pro` (File > Open file or project).
3. Skonfigurować kompilator (zalecany MSCV), ścieżki kompilowania i dać na przycisk Configure Project.
4. Uruchomić Build > Run QMake.
5. Uruchomić Build > Build Project "Makao".

Po kompilacji plik exe zostanie utworzony w folderze skonfigurowanym w kroku 2. Domyślnie jest to `.\build-makao-Desktop_Qt_5_6_0_MSVC2015_64bit-Debug\debug`.

Aby uruchomić aplikację z poziomu folderu z buildem, trzeba zrobić deploy bibliotek Qt do tego folderu. Można to zrobić komendą `windeployqt [ścieżka do folderu z buildem]` np:
```
d:
cd D:\Qt\5.6\msvc2015_64\bin
windeployqt D:\(...)\build-makao-Desktop_Qt_5_6_0_MSVC2015_64bit-Debug\debug
```
W celu testów gry należy uruchomić aplikację wielokrotnie (minimalna liczba graczy: 2, max: 4). W trakcie testów trzeba też wyłączyć zaporę systemu windows i inne zapory jeżeli są obecne.
