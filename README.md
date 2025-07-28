# Producent – Konsument z dynamiczną listą (C + pthread)

Projekt wykonany w ramach zaliczenia przedmiotu **Programowanie systemowe i współbieżne** 

III Semestr Rok akademicki: 2024/2025

Projekt demonstrujący użycie **wielowątkowości** w języku **C** przy użyciu biblioteki `pthread`.  
Program implementuje synchronizację między wątkami producenta i konsumenta z użyciem dynamicznej listy, umożliwiając także dynamiczną zmianę jej rozmiaru i łączenie dwóch list.

## Opis

Program zawiera trzy główne testy:

1. **Producent czeka na konsumenta** – klasyczna implementacja wzorca producent–konsument.
2. **Dynamiczna zmiana rozmiaru listy w czasie działania** – wątek `resizer` zmienia maksymalny rozmiar listy w trakcie działania producenta i konsumenta.
3. **Łączenie dwóch list** – test działania funkcji `appendItems()`.
