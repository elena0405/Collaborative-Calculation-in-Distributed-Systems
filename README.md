# Collaborative-Calculation-in-Distributed-Systems
Tema 3 APD 

Pentru prima parte a temei, fiecare procesor coordonator citeste rangurile
workerilor proprii din fisierul ascociat, dupa care tirmite catre ceilalti doi
coordonatori valorile citite. La randul sau, primeste valorile rangurilor 
workerilor celorlalti doi coordonatori, si afiseaza topologia. In continuare,
fiecare procesor coordonator comunica cu workerii sai si le trimite rangul propriu,
precum si valorile rangurilor workerilor fiecarui proces coodonator, inclusiv 
pe ale sale. Astfel, procesele worker afla topologia si o afiseaza.
	Pentru a doua parte a temei, procesul 0 extrage numarul de elemente ale
vectorului din linia de comanda si construieste vectorul. Calculeaza cam cate elemente
 ar veni, in mod normal, fiecarui worker, precum si un rest, ce reprezinta numarul de
elemente care raman, in cazul in care se distribuie un numar de elemente divizibil cu
numarul de procese worker. In continuare, distribuie elementele catre cei doi coordonatori, 
care, la randul lor, le distribuie inca odata catre workerii lor, insa tinand cond de 
acel rest; astfel, la dimensiunea obisnuita blocului de elemente asociat fiecarui worker 
se aduna 1 pana cand restul devine 0. Astfel, unele procese worker vor avea o dimensiune mai
mare cu 1 decat a celorlalte procese worker. Dupa ce procesele worker primesc blocul de
elemente, le inmultesc cu 2 si le trimit catre procesele coordonator. In continuare,
procesele coordonator primesc aceste elemente de la procesele worker si le trimit catre 
procesul 0, care le retine pe toate intr-un vector. La final, procesul 0 afiseaza
vectorul.
	Pentru situatia in care cade conexiunea intre clusterele 0 si 2, se repeta pasii de mai 
sus, doar ca, comunicarea intre procesul 0 si procesul 1 se face prin intermediul 
procesului 2. Astfel, de fiecare data cand procesul 0 vrea sa comunice cu procesul 1, 
va tirmite datele respective catre procesul 2, iar acesta le va tirmite catre procesul 1. 
Cand procesul 1 vrea sa comunice catre procesul 0, acesta trimite datele catre procesul 2, 
iar procesul 2 le trimite catre procesul 0.
