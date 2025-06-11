# Sprawozdanie: Klasteryzacja DBSCAN na zbiorze MNIST z redukcją PCA+TSNE

## 1. Cel
Celem eksperymentu było zastosowanie algorytmu **DBSCAN** do klasteryzacji próbek cyfr z danych MNIST. Zastosowano redukcję wymiarów PCA oraz t-SNE aby
uniknąć "klątwy wielowymiarowości" DBSCAN-a. Zadaniem było uzyskać możliwie jednolite klastry (dominująca jedna cyfra), ograniczyć szum oraz uzyskać liczbę klastrów nie większą niż około 30 (np. różne style pisma tej samej cyfry).

## 2. Dane i preprocessing
- Użyto zestawu danych MNIST.  
- Normalizacja pikseli do [0,1], spłaszczenie do wektorów 784-wymiarowych.  

## 3. Redukcja wymiarów
1. **PCA**: zmniejszenie wymiaru z 784 do 50, dla zachowania większości wariancji i przyspieszenia t-SNE.  
2. **t-SNE**: redukcja z 50 do 2 wymiarów, w celu wizualizacji i klasteryzacji w przestrzeni 2D.  
3. **Wynik**: dwuwymiarowe przedstawienie próbek, na którym wykonano DBSCAN.

## 4. Implementacja DBSCAN
- Klasa `Dbscan(eps, min_samples)`:
  - Dla każdego punktu: znajdowanie sąsiadów w promieniu `eps` (skorzystano ze `sklearn.neighbors.NearestNeighbors`).
  - Punkt z liczbą sąsiadów $>=$ `min_samples` to punkt bazowy (core). Rozwijanie klastra przez iteracyjne dodawanie sąsiadów core-pointów.
  - Pozostałe punkty oznaczone jako szum (`label = -1`), o ile nie zostaną włączone do klastra w fazie rozszerzania.
  - W celu znalezienia odpowiedniej wartości eps wykonano wykres odległości do 
  k-tego najbliższego sąsiada.

## 5. Wyniki

- **Liczba próbek**: 60000  
- **Wybrane parametry**:  
  - `eps = 2.0`  
  - `min_samples = 11`
- **Liczba klastrów (bez szumu)**: 26
- **Liczba punktów szumu**: 478 (0.8%)  
- **Punkty przypisane do klastrów**: 59522 (99.2%)  
- **Dokładność wewnątrz klastrów (majority-vote)**: np. 88.2%

### 6.1. Tabela purities dla każdego klastra
|   Cluster ID | No. points |   Dominating digit |   Purity (%) |
|-------------:|-----------:|-------------------:|-------------:|
|            0 |       5486 |                  5 |        95.01 |
|            1 |       5920 |                  0 |        98.95 |
|            2 |      11634 |                  4 |        49.54 |
|            3 |       6596 |                  1 |        98.23 |
|            4 |       5814 |                  2 |        98.37 |
|            5 |       5712 |                  3 |        98.23 |
|            6 |       5966 |                  6 |        98.01 |
|            7 |       6238 |                  7 |        97.42 |
|            8 |       5658 |                  8 |        97.74 |
|            9 |        146 |                  1 |        83.56 |
|           10 |         22 |                  3 |        81.82 |
|           11 |         15 |                  2 |       100    |
|           12 |         11 |                  7 |        81.82 |
|           13 |         80 |                  3 |        76.25 |
|           14 |         11 |                  1 |        72.73 |
|           15 |         42 |                  1 |       100    |
|           16 |         26 |                  2 |        88.46 |
|           17 |         15 |                  3 |        73.33 |
|           18 |         16 |                  9 |        81.25 |
|           19 |         28 |                  9 |        78.57 |
|           20 |         16 |                  7 |        87.5  |
|           21 |         12 |                  5 |       100    |
|           22 |         25 |                  3 |        44    |
|           23 |         11 |                  8 |        90.91 |
|           24 |         11 |                  5 |        90.91 |
|           25 |         11 |                  8 |       100    |
### 6.2. Wizualizacja
- Wykres t-SNE 2D, punkty odpowiadające danym cyfrom z MNIST są pokolorowane na różne kolory
- Wykres t-SNE 2D, punkty pokolorowane etykietami DBSCAN (szum czarny / “x”, klastry różne kolory).  

## 7. Obserwacje
- **Podział cyfr na podklastry**:  
  - Niektóre cyfry (np. 1, 3, 2, 5) utworzyły dwa lub więcej klastrów, które mogą odpowiadać różnym stylom pisma.  
  - Inne cyfry (np. 0) były bardzo jednorodne i utworzyły jeden duży klaster.  
- **Mieszanie cyfr**:  
  - W klastrze nr. 2 pojawiło się mieszanie cyfr 9 i 4, które mają podobne kształty w t-SNE,
  wpłynęło to na niską purity w tym klastrze (49.5%).  
- **Wpływ parametrów**:  
  - Zwiększenie `eps` powodowało scalanie niektórych klastrów różnych cyfr i wzrost błędów.  
  - Zmniejszenie `eps` zwiększało szum; pewne rzadziej pisane style zostały odrzucone jako szum.  

## 8. Wnioski
- Algorytm DBSCAN po redukcji PCA+t-SNE umożliwia wykrycie klastrów odpowiadających w większości pojedynczym cyfrom oraz rozpoznanie różnych stylów pisma jako odrębnych klastrów.  
- Osiągnięto umiarkowanie wysoką purity (88%) przy akceptowalnym poziomie szumu (mniej niż 1%).  
- Liczba klastrów mieści się w założeniach (<30).  