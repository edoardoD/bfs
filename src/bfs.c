/****************************************************************************
 *
 * bfs.c -- Visita in ampiezza di grafi orientati
 *
 * Copyright (C) 2021--2023 Moreno Marzolla
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

/***
% LabASD - Visita in ampiezza (BFS)
% Moreno Marzolla <moreno.marzolla@unibo.it>
% Ultimo aggiornamento: 2023-04-13

[![BFS Demo](bfs-demo.png "Demo visita BFS")](bfs-demo.html)

Realizziamo l'algoritmo di visita in ampiezza (_Breadth-First Search_,
BFS) di un grafo orientato $G=(V,E)$ con $n$ nodi e $m$
archi. L'algoritmo da implementare accetta un nodo sorgente $s$, e
deve visitare esclusivamente i nodi raggiungibili da $s$.  La visita
in ampiezza inizia da un nodo sorgente $s$ e procede "per livelli":
prima si visitano tutti i nodi adiacenti a $s$, poi tutti i nodi
adiacenti a quelli adiacenti, e così via.  Cliccando sull'immagine in
cima alla pagina è possibile vedere l'algoritmo BFS in azione
([fonte](https://bl.ocks.org/mbostock/11337835)).

Al termine della visita, l'algoritmo riempie due array `p[]` e `d[]`,
entrambi di $n$ elementi, con le seguenti informazioni:

- `p[v]` è il predecessore del nodo $v$ durante la visita (il
   predecessore del nodo sorgente è $-1$; è possibile usare il simbolo
   `NODE_UNDEF` il cui valore è $-1$ per evitare di fare ricorso a
   "costanti magiche");

- `d[v]` è la distanza del nodo $v$ dalla sorgente, dove per distanza
  si intende il numero di archi attraversati. Per ogni nodo
  raggiungibile dalla sorgente, l'algoritmo BFS determina il minimo
  numero di archi che è necessario attraversare per raggiungerlo. Se
  un nodo $v$ non è raggiungibile dalla sorgente poniamo `d[v] = -1`

L'algoritmo BFS richiede l'uso di una coda FIFO in cui inserire i nodi
da visitare. In una coda FIFO il primo elemento inserito è anche il
primo ad essere estratto. A tale scopo si può usare la struttura dati
`List` già vista in una [precedente esercitazione](list.html), che
supporta inserimento e cancellazione da entrambi gli estremi in tempo
$O(1)$. Una coda FIFO può quindi essere simulata inserendo sempre da
una estremità, e rimuovendo sempre dall'altra.

Per compilare:

        gcc -std=c90 -Wall -Wpedantic list.c graph.c bfs.c -o bfs

Per eseguire in ambiente Linux/MacOSX:

        ./bfs 0 graph100.in

Per eseguire in ambiente Windows:

        ./bfs 0 graph100.in

dove al posto di 0 si può indicare l'indice di qualunque altro nodo da
usare come sorgente.

## Curiosità

La visita in ampiezza può essere applicata all'analisi del grafo delle
mosse di un gioco da tavolo, per determinare il numero minimo di mosse
necessarie a vincere o se esiste una strategia vincente.  Un
risultato interessante è stato ottenuto nel 2007 quando un gruppo di
ricercatori ha [completato l'analisi del gioco della
dama](https://www.researchgate.net/publication/231216842_Checkers_Is_Solved),
che ha circa $5 \times 10^{20}$ configurazioni valide (che
corrispondono a nodi del grafo). È stato così dimostrato che, se
entrambi i giocatori giocano in modo "perfetto", il risultato è un
pareggio.  _"The Atlantic"_ ha un [articolo non
tecnico](https://www.theatlantic.com/technology/archive/2017/07/marion-tinsley-checkers/534111/)
che discute questo risultato.

Usando la visita in ampiezza dell'albero delle mosse, è possibile
rispondere alle questioni sull'esercizio [shooting
stars](shooting-stars.html) che sono rimaste in sospeso: qual è il
minimo numero di mosse necessarie per vincere? qual è il minimo numero
di mosse necessarie per perdere? esistono configurazioni della griglia
di gioco che non possono essere ottenute da quella iniziale?

## File

- [bfs.c](bfs.c)
- [graph.c](graph.c)
- [graph.h](graph.h)
- [list.c](../solutions/list.c)
- [list.h](../solutions/list.h)
- [graph10.in](graph10.in) (risultato della visita in ampiezza usando [il nodo 0 come sorgente](graph10-bfs-0.out) oppure [il nodo 5 come sorgente](graph10-bfs-5.out); i cammini potrebbero risultare diversi dalla propria soluzione in base all'ordine con cui si esplorano i vicini, le distanze minime per&ograve; devono essere le stesse)
- [graph100.in](graph100.in)
- [graph1000.in](graph1000.in)

***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "header\graph.h"
#include "header\list.h"

/* Si può usare il simbolo NODE_UNDEF per indicare che il predecessore
   della lista dei padri non esiste. */
const int NODE_UNDEF = -1;

typedef enum
{
    WHITE,
    GREY,
    BLACK
} Color; /* colori dei nodi */

/***
 * Visita il grafo g usando l'algoritmo di visita in ampiezza (BFS)
   partendo dal nodo sorgente s. Restituisce il numero di nodi
   visitati (incluso s).
    @param g grafo da visitare
    @param s nodo sorgente
    @param d al termine dell'esecuzione, d[i] e' la distanza
                    del nodo `i` dalla sorgente `s`, dove la distanza
                    e' intesa come minimo numero di archi che
                    collegano la sorgente al nodo `i`. Questo array
                    deve essere allocato dal chiamante e contenere `n`
                    elementi (uno per ogni nodo del grafo)
    @param p al termine dell'esecuzione, `p[i]` è il
                    predecessore del nodo `i` lungo il cammino minimo
                    da `s` a `i`. Nel caso in cui `i` non sia
                    raggiungibile dalla sorgente, `p[i] = -1`. Anche
                    questo array deve essere allocato dal chiamante e
                    avere lunghezza `n`
     @return numero di nodi visitati
 ***/
int bfs(const Graph *g, int s, int *d, int *p)
{
    const int n = graph_n_nodes(g);
    int v /*rappresenta il primo elemento della coda che viene rimosso e restituito */,
        u, counter = 0;
    Color *color;
    List *Q;
    Edge *e;

    assert(g != NULL);
    assert((s >= 0) && (s < n));
    assert(d != NULL);
    assert(p != NULL);

    /* Inizializza i colori di tutti i nodi a WHITE, la distanza di
       tutti i nodi dalla sorgente a -1, e il predecessore di tutti i
       nodi a NODE_UNDEF */
    color = (Color *)malloc(n * sizeof(*color));
    assert(color != NULL);
    for (v = 0; v < n; v++)
    {
        color[v] = WHITE;
        d[v] = -1;
        p[v] = NODE_UNDEF;
    }
    d[s] = 0;
    /* Inizializza la coda Q con il solo nodo sorgente */

    Q = list_create();
    list_add_first(Q, s);

    /* Finché la coda non è vuota, estraiamo un nodo dalla coda e
       visitiamo tutti i suoi vicini */
    while (!list_is_empty(Q))
    {
        v = list_remove_last(Q);
        e = graph_adj(g, v);

        while (e != NULL)
        {
            u = e->dst;
            if (color[u] == WHITE)
            {
                color[u] = GREY;
                p[u] = v;
                d[u] = d[v] + 1;
                list_add_first(Q, u);
                list_print(Q);
            }
            e = e->next;
        }
        counter++;

        color[v] = BLACK;
    }
    free(color);
    list_destroy(Q);

    graph_print(g);
    return counter;
}

/* Stampa il cammino che va da `s` a `d` prodotto dalla visita in
   ampiezza; se `d` non è raggiungibile da `s`, stampa "Non
   raggiungibile". La stampa del cammino deve avere la forma:

   s->n1->n2->...->d

   dove n1, n2... sono gli interi che individuano i nodi
   attraversati. */
void print_path(int s, int d, const int *p)
{
    if (s == d)
        printf("%d", s);
    else if (p[d] < 0)
        printf("Non raggiungibile");
    else
    {
        print_path(s, p[d], p);
        printf("->%d", d);
    }
}

/* Stampa le distanze di tutti i nodi dal nodo `src`, e i cammini per
   raggiungere tutti i nodi del grafo da `src`. I cammini sono quelli
   prodotti dalla visita in ampiezza. L'array `p[]` indica l'array dei
   predecessori, cioè `p[i]` è il predecessore del nodo `i`
   nell'albero corrispondente alla visita BFS. */
void print_bfs(const Graph *g, int src, const int *d, const int *p)
{
    const int n = graph_n_nodes(g);
    int v;

    assert(p != NULL);
    assert(d != NULL);

    printf("  src | dest | distanza | path\n");
    printf("------+------+----------+-------------------------\n");
    for (v = 0; v < n; v++)
    {
        printf(" %4d | %4d | %8d | ", src, v, d[v]);
        print_path(src, v, p);
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    Graph *G;
    int nvisited; /* n. di nodi raggiungibili dalla sorgente */
    int *p, *d;
    FILE *filein = stdin;
    int src = 0, n;

    if (argc != 3)
    {
        fprintf(stderr, "Invocare il programma con: %s nodo_sorgente file_grafo\n", argv[0]);
        return EXIT_FAILURE;
    }

    src = atoi(argv[1]);

    if (strcmp(argv[2], "-") != 0)
    {
        filein = fopen(argv[2], "r");
        if (filein == NULL)
        {
            fprintf(stderr, "Can not open %s\n", argv[2]);
            return EXIT_FAILURE;
        }
    }

    G = graph_read_from_file(filein);
    n = graph_n_nodes(G);
    assert((src >= 0) && (src < n));
    p = (int *)malloc(n * sizeof(*p));
    assert(p != NULL);
    d = (int *)malloc(n * sizeof(*d));
    assert(d != NULL);
    nvisited = bfs(G, src, d, p);
    print_bfs(G, src, d, p);
    printf("# %d nodi su %d raggiungibili dalla sorgente %d\n", nvisited, n, src);
    graph_destroy(G);
    free(p);
    free(d);
    if (filein != stdin)
        fclose(filein);

    return EXIT_SUCCESS;
}
