# SynchronizerDaemon

Program synchronizujący ze sobą dwa katalogi.

### Funckjonalności

  - Synchronizacja plików zwykłych znajdujących się bezpośrednio w katalogu źródłowym.
  - Synchronizacja plików zwykłych znajdujących się w podkatalogach katalogu źródłowego.
  - Usuwanie plików zwykłych i katalogów z katalogu docelowego których nie ma w katalogu źródłowym.
  - Synchronizacja w tle z możliwością ustawienia odstępu pomiędzy kolejnymi synchronizacjami.
  - Duże pliki kopiowane są przy użyciu mapowania.
  - Możliwe jest ręczne ustawienie wielkości plików do mapowania.
  - Synchronizacja może wystąpić ad hoc po odebraniu sygnału _SIGUSR1_.
  - Przebieg synchronizacji zapisywany jest w logu systemowym.
  -- Rozpoczęcie i zakończenie synchronizacji,
  -- Skopiowanie/usunięcie pliku/katalogu,
  -- Informacja o błędzie uniemożliwiającym odczyt/zapis.

### Kompilacja

```sh
$ make
```

### Uruchomienie

```sh
$ ./mysync -s /katalog/źródłowy -d /katalog/docelowy [-R] [-t bajty] [-p sekundy]
```

Argumenty __-s__, __-d__ są wymagane i wskazują odpowiednio katalog źródłowy i katalog docelowy synchronizacji.
Argumenty __-R__, __-t bajty__, __-p sekundy__ są opcjonalne.

__-R__ -- wymusza synchronizację rekurencyjną. Podkatalogi i pliki zwykłe w nich zawarte również zostają skopiowane.
__-t bajty__ -- wyznacza maksymalną wielkość pliku który zostanie skopiowany przy użyciu funkcji _read()_.
Pliki powyżej tego rozmiaru zostaną skopiowane funkcją _mmap()_. 
__-p sekundy__ -- czas który będzie rozdzielał kolejne synchronizacje.

### Struktura programu

| Plik | Funkcjonalność|
| ------ | ----- |
| main.c | punkt startowy aplikacji |
| synchronizer.c | odpowiada za przeprowadzenie synchronizacji |
| myFileApi.c | zawiera funkcje odpowiedzialne za pracę z plikami |
| myDirectoryApi.c | zawiera funkcje odpowiedzialne za pracę z katalogami |
| myUtils.c | funkcje pomocnicze |
| logger.c | zapis do logu systemowego |
| config.c | przechowuje konfigurację synchronizacji |
| myList.c | definiuje listę i jej obsługę |
