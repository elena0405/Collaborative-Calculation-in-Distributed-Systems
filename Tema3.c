#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    FILE* f;
    int numar_taskuri;
    int x, offset, numarValori;
    int v0[100], v1[100], v2[100];
    int eroare_comunicatie, valoare_primita;
    int rang, rang_coordonator, rang_subtask;
    int nr_workeri_proces_0, nr_workeri_proces_1, nr_workeri_proces_2;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numar_taskuri);
    MPI_Comm_rank(MPI_COMM_WORLD,&rang);

    // Daca in linia de comanda primesc mai putin de 3 argumente, afisez un mesaj de eroare.
    if (argc < 3) {
        if (rang == 0) {
            printf("Usage: mpirun -np <numar_procese> ./tema3 <dimensiune_vector> <eroare_comunicatie>\n");
        }
    }
    else {
        eroare_comunicatie = (int)strtol(argv[2], NULL, 10);
        // Daca nu am eroare de comunicatie.
        if (eroare_comunicatie == 0) {
            if (rang == 0) {
                char s[1000000];

                // Coordonatorul 0 citeste din fisierul aferent acestuia, pentru
                // a afla care sunt workerii sai.
                f = fopen("cluster0.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_0);

                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v0[i] = rang_subtask;
                }

                fclose(f); // Inchid fisierul de citire.

                // Procesul 0 trimite catre procesul coordonator 1 rangurile worker-ilor sai.
                MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");
                MPI_Send(v0, nr_workeri_proces_0, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");

                // Procesul 0 trimite catre procesul coordonator 2 rangurile worker-ilor sai.
                MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(v0, nr_workeri_proces_0, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");

                // Procesul 0 primeste de la procesul coordonator 1 rangurile worker-ilor acestuia.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                MPI_Recv(vec1, nr_workeri_proces_1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 0 primeste de la procesul coordonator 2 rangurile worker-ilor acestuia.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                MPI_Recv(vec2, nr_workeri_proces_2, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul 0 stie toata topologia si o afiseaza.
                printf("0 -> ");
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", v0[i]);
                    }
                    else {
                        printf("%d,", v0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul 0 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri proprii.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai rangurile worker-ilor proprii.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(v0, nr_workeri_proces_0, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri ai procesului 1.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai rangurile worker-ilor procesului 1.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(vec1, nr_workeri_proces_1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri ai procesului 2.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre toti workerii sai rangurile worker-ilor procesului 2.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(vec2, nr_workeri_proces_2, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Dupa ce a terminat de trimits topologia, construieste vectorul de valori
                numarValori = (int)strtol(argv[1], NULL, 10);
                int V[numarValori];

                for (int i = 0; i < numarValori; i++) {
                    V[i] = i;
                }

                // Determina cate elemente ar trebui asignate fiecarui proces worker.
                x = numarValori / (numar_taskuri - 3);
                // Calculez restul ramas.
                int rest = numarValori % (numar_taskuri - 3);
                int y = x;
                offset = 0;

                // Trimite catre workerii sai numarul de elemente pe care trebuie sa le prelucreze,
                // offset-ul (indexul de inceput al bucatii de vector pe care o prelucreaza) si
                // numarul total de valori ale vectorului ce trebuie prelucrat. Distribuie restul 
                // ramas in mod egal catre workerii sai.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    MPI_Send(&y, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(&offset, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(V, numarValori, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);

                    rest--;

                    offset += y;
                }

                // Trimite catre procesul 1 bucata de vector care va fi
                // prelucrata de workerii acestuia.
                MPI_Send(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");
                MPI_Send(&rest, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");
                MPI_Send(&offset, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");
                MPI_Send(&numarValori, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");
                MPI_Send(V, numarValori, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(0,1)\n");

                // Primeste restul ramas, dupa ce procesul 1 a distribuit 
                // bucata respectiva de vector workerilor sai, in mod echilibrat.
                MPI_Recv(&rest, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // In caz ca a mai ramas ceva din restul initial, il aduna la 
                // workerii procesului 2.
                if (rest >= 0) {
                    offset += (x + 1) * nr_workeri_proces_1;
                }
                else if (rest < 0) {
                    offset += x * nr_workeri_proces_1;
                }

                // Trimite catre procesul 2 numarul de elemente ce trebuie prelucrate de
                // workerii acestuia, restul ramas, offset-ul, numarul de elemente al
                // vectorului initial si vectorul initial.
                MPI_Send(&x, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&numarValori, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");

                // Declar o variabila noua, in care asamblez rezultatul final.
                int rezultat[11000];

                // Primeste de la fiecare proces worker: offset-ul de unde incepe bucata
                // de vector prelucrata, dimensiunea bucatii de vector 
                // prelucrata si vectorul de elemente, ce contine modificarile necesare
                // doar in bucata de vector asociata worker-ului respectiv.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Adauga valorile prelucrate in vectorul final de elemente.
                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Primeste de la procesul 1 valorile prelucrate de workerii acestuia,
                // cu atributele explicate mai sus.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Adauga valorile prelucrate in vectorul final de elemente.
                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Primeste de la procesul 2 valorile prelucrate de workerii acestuia,
                // cu atributele explicate mai sus.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Adauga valorile prelucrate in vectorul final de elemente.
                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Retine afisarile necesare pentru a doua parte a temei intr-un
                // sir de caractere.
                int n = strlen("Rezultat:");
                int index = n;
                strcpy(s, "Rezultat:");
                char valoare[10];

                for (int i = 0; i < numarValori; i++) {
                    strcpy(s + index, " ");
                    index++;
                    sprintf(valoare, "%d", rezultat[i]);
                    strcpy(s + index, valoare);
                    index += strlen(valoare);
                }

                strcpy(s + index, "\n");

                // Folosesc aceasta comanda ca afisarea vectorului de elemente sa nu 
                // se intersecteze cu mesajele trimise de procese.
                sleep(1);
                // La final, afiseaza sirul de caractere, ce contine vectorul de elemente.
                printf("%s", s);
            }
            else if (rang == 1) {
                f = fopen("cluster1.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_1);

                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v1[i] = rang_subtask;
                }

                fclose(f);

                // Procesul 1 trimite catre procesul 0 rangurile worker-ilor sai.
                MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(1,0)\n");
                MPI_Send(v1, nr_workeri_proces_1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(1,0)\n");

                // Procesul 1 trimite catre procesul 2 rangurile workerilor sai.
                MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(1,2)\n");
                MPI_Send(v1, nr_workeri_proces_1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(1,2)\n");

                // Procesul 1 primeste de la procesul 0 rangurile worker-ilor acestuia.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                MPI_Recv(&vec0, nr_workeri_proces_0, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 1 primeste de la procesul 2 rangurile workerilor acestuia.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                MPI_Recv(&vec2, nr_workeri_proces_2, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul 1 stie toata topologia si o afiseaza.
                printf("1 -> ");
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", v1[i]);
                    }
                    else {
                        printf("%d,", v1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul 1 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri ai procesului 0.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai rangurile worker-ilor procesului 0.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(vec0, nr_workeri_proces_0, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri proprii.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai rangurile worker-ilor proprii.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(v1, nr_workeri_proces_1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri ai procesului 2.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre toti workerii sai rangurile worker-ilor procesului 2.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(vec2, nr_workeri_proces_2, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 primeste de la procesul 0 bucata de vector pe care workerii
                // acestuia trebuie sa o prelucreze, unde x reprezinta dimensiunea
                // bucatii de vector, rest reprezinta restul de elemente ce trebuie
                // distribuit in mod egal, offset reprezinta indexul de la care incepe
                // bucata de vector ce trebuie prelucrata, numarValori reprezinta lumginea
                // totala a vectorului, iar valoriPrimite reprezinta vectorul de 
                // elemente.
                int rest;
                MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite[numarValori];
                MPI_Recv(valoriPrimite, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int index = offset;
                int y = x;

                // Procesul 1 trimite catre workerii sai bucata de vector aferenta 
                // fiecaruia, dinstribuita in mod echilibrat.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    MPI_Send(&y, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(&index, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);

                    index += y;
                    rest--;
                }

                // Procesul 1 trimite catre procesul 0 restul ramas.
                MPI_Send(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(1,0)\n");

                // Procesul 1 primeste de la workerii sai valorile prelucrate, unde offset
                // reprezinta indexul de inceput al bucatii de vector prelucrata,
                // y reprezinta dimensiunea bucatii de vector prelucrata, iar valoriPrimite
                // reprezinta vectorul initial, ce contine modificarile facute. De asemenea,
                // trimite valorile primite catre procesul 0, pentru asamblarea rezultatului final.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(valoriPrimite, numarValori, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Send(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(1,0)\n");
                    MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(1,0)\n");
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(1,0)\n");
                }
            }
            else if (rang == 2) {
                // Procesul 2 citeste din fisierul aferent acestuia rangurile workerilor sai.
                f = fopen("cluster2.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_2);

                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v2[i] = rang_subtask;
                }

                fclose(f); // Inchid fisierul de citire.

                // Procesul 2 trimite catre procesul 0 rangurile worker-ilor sai.
                MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");
                MPI_Send(v2, nr_workeri_proces_2, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");

                // Procesul 2 trimite catre procesul 1 rangurile worker-ilor sai.
                MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                MPI_Send(v2, nr_workeri_proces_2, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");

                // Procesul 2 primeste de la procesul 0 rangurile worker-ilor acestora.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                MPI_Recv(&vec0, nr_workeri_proces_0, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 primeste de la procesul 1 rangurile worker-ilor acestora.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                MPI_Recv(&vec1, nr_workeri_proces_1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul 2 stie toata topologia si o afiseaza.
                printf("2 -> ");
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", v2[i]);
                    }
                    else {
                        printf("%d,", v2[i]);
                    }
                }

                printf("\n");

                // Procesul 2 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri ai procesului 0.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // procesul 2 trimite catre workerii sai rangurile worker-ilor procesului 0.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(vec0, nr_workeri_proces_0, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri ai procesului 1.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai rangurile worker-ilor procesului 1.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(vec1, nr_workeri_proces_1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri proprii.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre toti workerii sai valorile rangurilor worker-ilor proprii.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(v2, nr_workeri_proces_2, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 primeste de la procesul 0 bucata de vector ce trebuie prelucrata
                // de catre workerii acestuia, cu explicatiile de la procesul 1.
                int rest;
                MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite[numarValori];
                MPI_Recv(valoriPrimite, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 distribuie sarcinile proceselor worker in mod echilibrat,
                // tinand cont si de acel rest.
                int index = offset, y = x;
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    MPI_Send(&y, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(&index, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);

                    rest--;
                    index += y;
                }

                // Procesul 2 primese de la workerii sai valorile prelucrate, cu explicatiile
                // de la procesul 1. In continuare, trimite datele catre procesul 0.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(valoriPrimite, numarValori, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Send(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                }
            }
            else {
                // Procesul worker curent primeste valoarea rangului propriu.
                MPI_Recv(&rang_coordonator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul worker curent primeste de la procesul coordonator numarul de 
                // workeri ai procesului 0.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                // Procesul worker curent primeste de la procesul coordonator rangurile workerilor
                // procesului 0.
                MPI_Recv(vec0, nr_workeri_proces_0, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul worker curent primeste de la procesul coordonator numarul de 
                // workeri ai procesului 1.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                // Procesul worker curent primeste de la procesul coordonator rangurile
                // worker-ilor procesului 1.
                MPI_Recv(vec1, nr_workeri_proces_1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul worker curent primeste de la procesul coordonator numarul de
                // workeri ai procesului 2.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                // Procesul worker curent primeste de la procesul coordonator rangurile 
                // workerilor procesului 2.
                MPI_Recv(vec2, nr_workeri_proces_2, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul worker curent stie toata topologia si o afiseaza.
                printf("%d -> ", rang);
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul worker curent primeste de la procesul coordonator urmatoarele:
                // - dimensiunea blocului de elemente, pe care trebuie sa o prelucreze
                // - offset-ul de la care incepe blocul de lemente ce trebuie prelucrat
                // - numarul de valori ale vectorului de elemente
                // - vectorul de elemente.
                MPI_Recv(&x, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite[numarValori];
                MPI_Recv(valoriPrimite, numarValori, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul worker curent face operatiile necesare in bucata de vector asociata lui,
                // restul vectorului ramanand nemodificat.
                for (int i = offset; i < offset + x; i++) {
                    valoriPrimite[i] = valoriPrimite[i] * 2;
                }

                // Procesul worker curent trimite catre procesul coordonator urmatoarele:
                // - offset-ul de la care incepe dimensiunea bucatii prelucrate
                // - dimensiunea bucatii de vector prelucrata
                // - vectorul de elemente, ce contine modificarile facute in bucata
                //   de vector asociata acestui proces worker
                MPI_Send(&offset, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
                MPI_Send(&x, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
                MPI_Send(valoriPrimite, numarValori, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
            }
        }
        else if (eroare_comunicatie == 1) {
            // De data aceasta, am eroare de comunicatie (BONUS).
            if (rang == 0) {
                // Declar o variabila in care retin vectorul afisat la final.
                char s[1000000];

                // Procesul 0 citeste din fisierul asociat lui valorile
                // workerilor acestuia.
                f = fopen("cluster0.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_0);

                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v0[i] = rang_subtask;
                }

                fclose(f); // Inchid fisierul de citire.

                // Procesul 0 trimite catre procesul 2 numarul de workeri ai sai.
                MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                // Procesul 0 trimite catre procesul 2 rangurile worker-ilor sai.
                MPI_Send(v0, nr_workeri_proces_0, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");

                // Procesul 0 primeste de la procesul 2 numarul de workeri ai procesului 1.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                // Procesul 0 primeste de la procesul 2 rangurile worker-ilor procesului 1.
                MPI_Recv(vec1, nr_workeri_proces_1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 0 primeste de la procesul 2 numarul de workeri ai acestuia.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                // Procesul 0 primeste de la procesul 2 rangurile worker-ilor acestuia.
                MPI_Recv(vec2, nr_workeri_proces_2, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul curent stie toata topologia si o afiseaza.
                printf("%d -> ", rang);
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", v0[i]);
                    }
                    else {
                        printf("%d,", v0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul 0 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri proprii.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai valorile workerilor proprii.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(v0, nr_workeri_proces_0, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri ai procesului 1
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai rangurile workerilor procesului 1
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(vec1, nr_workeri_proces_1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai numarul de workeri ai procesului 2
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Procesul 0 trimite catre workerii sai rangurile workerilor procesului 2
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Send(vec2, nr_workeri_proces_2, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                }

                // Dupa ce a terminat de trimis topologia, construieste vectorul de valori
                numarValori = (int)strtol(argv[1], NULL, 10);
                int V[numarValori];

                for (int i = 0; i < numarValori; i++) {
                    V[i] = i;
                }

                // Calculeaza dimensiunea bucatii de vector ce revine, in mod normal,
                // fiecarui proces worker. La acest calcul, se va adauga, dupa caz, si un rest.
                int x = numarValori / (numar_taskuri - 3);
                // Calculeaza restul de elemente.
                int rest = numarValori % (numar_taskuri - 3);
                int y = x;
                offset = 0;

                // Procesul 0 trimite catre workerii sai cate o bucata de vector ce trebuie
                // prelucrata, la dimensiunea careia se adauga 1, in cazul in care
                // avem un rest nenul.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    // Procesul 0 trimite catre workerii sai urmatoarele:
                    // - dimensiunea blocului de vector aferent acestuia
                    // - offset-ul de la care incepe blocul de elemente ce
                    //   trebuie prelucrat
                    // - numarul de elemente ale vectorului
                    // - vectorul de elemente
                    MPI_Send(&y, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(&offset, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);
                    MPI_Send(V, numarValori, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
                    printf("M(0,%d)\n", v0[i]);

                    rest--;

                    offset += y;
                }

                // Procesul 0 trimite catre procesul 2 valorile pentru procesul 1, 
                // conform eplicatiilor de mai sus.
                MPI_Send(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&numarValori, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");

                // Procesul 0 primeste de la procesul 2 restul ramas dupa impartirea
                // elementelor de catre procesul 1.
                MPI_Recv(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // In cazul in care restul de elemente este nenul, o sa adauge 1 
                // la dimensiunea blocului proceselor urmatoare.
                if (rest > 0) {
                    offset += (x + 1) * nr_workeri_proces_1;
                }
                else if (rest <= 0) {
                    offset += x * nr_workeri_proces_1;
                }

                // Procesul 0 trimite catre procesul 2 valorile aferente acestuia,
                // conform explicatiilor de mai sus.
                MPI_Send(&x, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(&numarValori, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");
                MPI_Send(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(0,2)\n");

                int rezultat[11000]; // Declar o variabila in care retin rezultatele finale.
                // Procesul 0 primeste de la workerii sai valorile prelucrate si le adauga in
                // vectorul rezlutat.
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, v0[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Procesul 0 primeste de la procesul 2 valorile prelucrate de workerii 
                // procesului 1 si le adauga in vectorul rezlutat.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Procesul 0 primeste de la procesul 2 valorile prelucrate de workerii acestuia
                // si le adauga in vectorul rezultat.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(V, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = offset; j < offset + y; j++) {
                        rezultat[j] = V[j];
                    }
                }

                // Procesul 0 retine in sirul de caractere, s, afisarile necesare
                // pentru vectorul rezultat.
                int n = strlen("Rezultat:");
                int index = n;
                strcpy(s, "Rezultat:");
                char valoare[10];

                for (int i = 0; i < numarValori; i++) {
                    strcpy(s + index, " ");
                    index++;
                    sprintf(valoare, "%d", rezultat[i]);
                    strcpy(s + index, valoare);
                    index += strlen(valoare);
                }

                strcpy(s + index, "\n");

                // Folosesc aceasta comanda pentru a nu exista suprapuneri
                // intre afisarile aferente vectorului si mesajele trimise.
                sleep(1);
                // Afisez vectorul de elemente.
                printf("%s", s);

            }
            else if (rang == 1) {
                // Procesul 1 citeste din fisierul asociat lui rangurile workerilor sai.
                f = fopen("cluster1.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_1);

                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v1[i] = rang_subtask;
                }

                fclose(f); // Inchid fisierul de citire.

                // Procesul 1 trimite catre procesul 2 numarul de workeri ai sai.
                MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(1,2)\n");
                // Procesul 1 trimite catre procesul 2 rangurile worker-ilor sai.
                MPI_Send(v1, nr_workeri_proces_1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(1,2)\n");

                // Procesul 1 primeste de la procesul 2 numarul de workeri ai procesului 0.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                // Procesul 1 primeste de la procesul 2 rangurile workerilor procesului 0.
                MPI_Recv(vec0, nr_workeri_proces_0, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 1 primeste de la procesul 2 numarul de workeri ai acestuia.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                // Procesul 1 primeste de la procesul 2 rangurile workerilor acestuia.
                MPI_Recv(vec2, nr_workeri_proces_2, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul curent stie toata topologia si o afiseaza.
                printf("%d -> ", rang);
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", v1[i]);
                    }
                    else {
                        printf("%d,", v1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul 1 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri ai procesului 0
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai rangurile workerilor procesului 0
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(vec0, nr_workeri_proces_0, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri proprii
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai rangurile workerilor proprii
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(v1, nr_workeri_proces_1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai numarul de workeri ai procesului 2
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 trimite catre workerii sai rangurile workerilor procesului 2
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Send(vec2, nr_workeri_proces_2, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                }

                // Procesul 1 primeste de la procesul 2 valorile necesare prelucrarii
                // bucatii de vector asociata workerilor sai.
                int rest;
                MPI_Recv(&x, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite[numarValori];
                MPI_Recv(valoriPrimite, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int index = offset;
                int y = x;

                // Procesul 1 distribuie in mod echilibrat numarul de elemente 
                // din vector, tinand cont de un rest.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    MPI_Send(&y, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(&index, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
                    printf("M(1,%d)\n", v1[i]);

                    index += y;
                    rest--;
                }

                // Procesul 1 trimite catre procesul 2 restul ramas in urma distribuirii, ca acesta,
                // la randul sau, sa tirmita acest rest procesului 0.
                MPI_Send(&rest, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(1,2)\n");

                // Procesul 1 primeste de la workerii sai valorile prelucrate si le trimite catre
                // procesul 2, ca acesta, la randul sau, sa le trimita catre procesul 0.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(valoriPrimite, numarValori, MPI_INT, v1[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Send(&offset, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                    printf("M(1,2)\n");
                    MPI_Send(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                    printf("M(1,2)\n");
                    MPI_Send(valoriPrimite, numarValori, MPI_INT, 2, 0, MPI_COMM_WORLD);
                    printf("M(1,2)\n");
                }
                
            }
            else if (rang == 2) {
                // Procesul 2 citeste rangurile workerilor sai din fisierul asociat acestuia.
                f = fopen("cluster2.txt", "r");
                fscanf(f, "%d", &nr_workeri_proces_2);

                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    fscanf(f, "%d", &rang_subtask);
                    v2[i] = rang_subtask;
                }

                fclose(f); // Inchid fisierul de citire.

                // Procesul 2 primeste de la procesul 0 numarul de workeri ai acestuia.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                // Procesul 2 primeste de la procesul 0 rangurile workerilor sai.
                MPI_Recv(vec0, nr_workeri_proces_0, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 primeste de la procesul 1 numarul de workeri ai acestuia.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                // procesul 2 primeste de la procesul 1 rangurile workerilor sai.
                MPI_Recv(vec1, nr_workeri_proces_1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 trimite catre procesul 0 numarul de workeri ai procesului 1.
                MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");
                // Procesul 2 trimite catre procesul 0 rangurile workerilor procesului 1.
                MPI_Send(vec1, nr_workeri_proces_1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");

                // Procesul 2 trimite catre procesul 0 numarul de workeri proprii.
                MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");
                // Procesul 2 trimite catre procesul 0 rangurile workerilor proprii.
                MPI_Send(v2, nr_workeri_proces_2, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");

                // Procesul 2 trimite catre procesul 1 numarul de workeri ai procesului 0.
                MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                // Procesul 2 trimite catre procesul 1 rangurile workerilor procesului 0.
                MPI_Send(vec0, nr_workeri_proces_0, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");

                // Procesul 2 trimite catre procesul 1 numarul de workeri proprii.
                MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                // Procesul 2 trimite catre procesul 1 rangurile workerilor proprii.
                MPI_Send(v2, nr_workeri_proces_2, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");

                // In momentul de fata, procesul curent stie toata topologia si o afiseaza.
                printf("%d -> ", rang);
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", v2[i]);
                    }
                    else {
                        printf("%d,", v2[i]);
                    }
                }

                printf("\n");

                // Procesul 2 trimite catre workerii sai valoarea rangului sau.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&rang, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri ai procesului 0
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_0, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai rangurile worker-ilor procesului 0
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(vec0, nr_workeri_proces_0, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri ai procesului 1
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_1, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai rangurile worker-ilor procesului 1
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(vec1, nr_workeri_proces_1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai numarul de workeri proprii
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(&nr_workeri_proces_2, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 trimite catre workerii sai rangurile worker-ilor proprii
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Send(v2, nr_workeri_proces_2, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                }

                // Procesul 2 primeste de la procesul 0 bucata de vector ce trebuie
                // prelucrata de workerii procesului 1.
                int rest;
                MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite1[numarValori];
                MPI_Recv(valoriPrimite1, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 trimite catre procesul 1 bucata de vector ce trebuie 
                // prelucrata de workerii acestuia, primita de la procesul 0.
                MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                MPI_Send(&rest, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                MPI_Send(&offset, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                MPI_Send(&numarValori, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");
                MPI_Send(valoriPrimite1, numarValori, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(2,1)\n");

                // Procesul 2 primeste un rest ramas in urma distribuirii elementelor
                // de catre procesul 1 catre workerii sai.
                MPI_Recv(&rest, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // Procesul 2 trimite catre procesul 0 restul primit.
                MPI_Send(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("M(2,0)\n");

                // Procesul 2 primeste de la procesul 0 bucata de vector ce trebuie
                // prelucrata de workerii sai.
                MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite2[numarValori];
                MPI_Recv(valoriPrimite2, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul 2 distribuie elementele catre workerii sai, in mod echilibrat,
                // tinand cont de un eventual rest nenul, in cazul in care numarul de elemente 
                // ale vectorului nu se imparte exact la numarul total de workeri.
                int index = offset;
                int y = x;
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (rest > 0) {
                        y = x + 1;
                    }
                    else {
                        y = x;
                    }

                    MPI_Send(&y, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(&index, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(&numarValori, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);
                    MPI_Send(valoriPrimite2, numarValori, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
                    printf("M(2,%d)\n", v2[i]);

                    rest--;
                    index += y;
                }

                // Procesul 2 primeste de la procesul 1 valorile prelucrate de workerii acestuia
                // si le trimite mai departe catre procesul 0.
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(valoriPrimite1, numarValori, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Send(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(valoriPrimite1, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                }

                // Procesul 2 primeste valorile prelucrate de workerii sai si le trimite catre procesul 0.
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    MPI_Recv(&offset, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&y, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(valoriPrimite2, numarValori, MPI_INT, v2[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Send(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                    MPI_Send(valoriPrimite2, numarValori, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    printf("M(2,0)\n");
                }
            }
            else {
                // Procesul curent primeste valoarea rangului procesului coordonator.
                MPI_Recv(&rang_coordonator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul curent primeste numarul de workeri ai procesului 0.
                MPI_Recv(&nr_workeri_proces_0, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec0[nr_workeri_proces_0];
                // Procesul curent primeste rangurile workerilor procesului 0.
                MPI_Recv(vec0, nr_workeri_proces_0, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul curent primeste numarul de workeri ai procesului 1.
                MPI_Recv(&nr_workeri_proces_1, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec1[nr_workeri_proces_1];
                // Procesul curent primeste rangurile worker-ilor procesului 1.
                MPI_Recv(vec1, nr_workeri_proces_1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul curent primeste numarul de workeri ai procesului 2.
                MPI_Recv(&nr_workeri_proces_2, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int vec2[nr_workeri_proces_2];
                // Procesul curent primeste rangurile worker-ilor procesului 2.
                MPI_Recv(vec2, nr_workeri_proces_2, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // In momentul de fata, procesul curent stie toata topologia si o afiseaza.
                printf("%d -> ", rang);
                printf("0:");
                for (int i = 0; i < nr_workeri_proces_0; i++) {
                    if (i == nr_workeri_proces_0 - 1) {
                        printf("%d", vec0[i]);
                    }
                    else {
                        printf("%d,", vec0[i]);
                    }
                }

                printf(" 1:");
                for (int i = 0; i < nr_workeri_proces_1; i++) {
                    if (i == nr_workeri_proces_1 - 1) {
                        printf("%d", vec1[i]);
                    }
                    else {
                        printf("%d,", vec1[i]);
                    }
                }

                printf(" 2:");
                for (int i = 0; i < nr_workeri_proces_2; i++) {
                    if (i == nr_workeri_proces_2 - 1) {
                        printf("%d", vec2[i]);
                    }
                    else {
                        printf("%d,", vec2[i]);
                    }
                }

                printf("\n");

                // Procesul curent primeste de la procesul coordonator urmatoarele:
                // - dimensiunea blocului de elemente ce trebuie prelucrate
                // - offset-ul de la care incepe dimensiunea blocului de elemente
                // - dimensiunea totala a vectorului de elemente
                // - vectorul de elemente
                MPI_Recv(&x, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&offset, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&numarValori, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int valoriPrimite[numarValori];
                MPI_Recv(valoriPrimite, numarValori, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Procesul curent face modificarile necesare in bucata lui de vector
                for (int i = offset; i < offset + x; i++) {
                    valoriPrimite[i] = valoriPrimite[i] * 2;
                }

                // Procesul curent trimite catre procesul coordonator urmatoarele:
                // - offset-ul de la care incepe bucata de vector prelucrata
                // - dimensiunea bucatii de vector prelucrata
                // - vectorul de elemente, ce contine modificari doar in bucata de cod 
                //   aferenta acestuia
                MPI_Send(&offset, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
                MPI_Send(&x, 1, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
                MPI_Send(valoriPrimite, numarValori, MPI_INT, rang_coordonator, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rang, rang_coordonator);
            }
        }
    }

    MPI_Finalize();
}

