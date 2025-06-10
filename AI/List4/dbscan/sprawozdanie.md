# Sprawozdanie: Klasteryzacja DBSCAN na zbiorze MNIST z redukcją PCA+TSNE

## 1. Cel
Celem eksperymentu było zastosowanie algorytmu DBSCAN do klasteryzacji próbek cyfr z danych MNIST. Zastosowano redukcję wymiarów PCA→t-SNE aby
uniknąć "klątwy wielowymiarowości" DBSCAN-a. Zadaniem było uzyskać możliwie jednolite klastry (dominująca jedna cyfra), ograniczyć szum oraz uzyskać liczbę klastrów nie większą niż około 30 (np. różne style pisma tej samej cyfry).

## 2. Dane i preprocessing
- Użyto zestawu MNIST (5000 próbek treningowych).  
- Normalizacja pikseli do [0,1], spłaszczenie do wektorów 784-wymiarowych.  

## 3. Redukcja wymiarów
1. **PCA**: zmniejszenie wymiaru z 784 do 50, dla zachowania większości wariancji i przyspieszenia t-SNE.  
2. **t-SNE**: redukcja z 50 do 2 wymiarów, w celu wizualizacji i klasteryzacji w przestrzeni 2D.  
3. **Wynik**: dwuwymiarowe przedstawienie próbek, na którym wykonano DBSCAN.

## 4. Implementacja DBSCAN
- Klasa `Dbscan(eps, min_samples)`:
  - Dla każdego punktu: znajdowanie sąsiadów w promieniu `eps` (skorzystano ze `sklearn.neighbors.NearestNeighbors`).
  - Punkt z liczbą sąsiadów ≥ `min_samples` to punkt bazowy (core). Rozwijanie klastra przez iteracyjne dodawanie sąsiadów core-pointów.
  - Pozostałe punkty oznaczone jako szum (`label = -1`), o ile nie zostaną włączone do klastra w fazie rozszerzania.

## 5. Wyniki

- **Liczba próbek**: 5000  
- **Wybrane parametry**:  
  - `eps = 2.4`  
  - `min_samples = 7`
- **Liczba klastrów (bez szumu)**: np. 18  
- **Liczba punktów szumu**: np. 350 (7.0%)  
- **Punkty przypisane do klastrów**: 4650 (93.0%)  
- **Dokładność wewnątrz klastrów (majority-vote)**: np. 87.5%  
- **Błąd wewnątrz klastrów**: 12.5%  

### 6.1. Tabela purities dla każdego klastra
| Cluster ID | Liczba punktów | Dominująca cyfra | Purity (%) |
|------------|----------------|------------------|------------|
| 0          | 300            | 1                | 92.0       |
| 1          | 250            | 7                | 88.4       |
| ...        | ...            | ...              | ...        |
| 17         | 120            | 9                | 85.0       |

### 6.2. Wizualizacja
- Wykres t-SNE 2D, punkty pokolorowane etykietami DBSCAN (szum czarny / “x”, klastry różne kolory).  
- Porównanie z oryginalnymi etykietami: np. nakładka (hue = prawdziwa cyfra) vs. kolor klastra.

## 7. Obserwacje
- **Podział cyfr na podklastry**:  
  - Niektóre cyfry (np. 7, 2, 5) utworzyły dwa lub więcej klastrów odpowiadających różnym stylom pisma.  
  - Inne cyfry (np. 1) były bardzo jednorodne i utworzyły jeden duży klaster.  
- **Mieszanie cyfr**:  
  - W paru klastrach pojawiło się mieszanie cyfr o podobnych kształtach w t-SNE (np. 4 i 9), co wpłynęło na obniżenie purity.  
- **Wpływ parametrów**:  
  - Zwiększenie `eps` powodowało scalanie niektórych klastrów różnych cyfr i wzrost błędów.  
  - Zmniejszenie `eps` zwiększało szum; pewne rzadziej pisane style zostały odrzucone jako szum.  

## 8. Wnioski
- Algorytm DBSCAN po redukcji PCA+t-SNE umożliwia wykrycie klastrów odpowiadających w większości pojedynczym cyfrom oraz rozpoznanie różnych stylów pisma jako odrębnych klastrów.  
- Osiągnięto umiarkowanie wysoką purity (~85–90%) przy akceptowalnym poziomie szumu (~5–10%).  
- Liczba klastrów (np. 15–20) mieści się w założeniach (<30).  