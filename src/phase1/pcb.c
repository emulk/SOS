/*
 *  Copyright (C) 2011 Orgest Shehaj & Fabian Priftaj & Eduart Uzeir
 *  Author: Orgest Shehaj <shehaj@cs.unibo.it>
 *          Fabian Priftaj <priftaj@cs.unibo.it>
 * 			Eduart Uzeir <uzeir@cs.unibo.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <pcb.e>

/* array di pcb con dimensione MAXPROC (MAXPROC = 20) */
static pcb_t pcb_table[MAXPROC];

/* struttura list_head per la sentinella 
 * E la testa della lista pcbFree */ 
static struct pcb_t **pcbfree_h;
 	

/**
 * Inizializza la lista dei processi liberi in modo da contenere
 * tutti gli elementi della pcb_table.
 */
void initPcbs(void)
{
	static int cont = 0;
	
	struct pcb_t *new = &(pcb_table[cont++]);
	freePcb(new);
	
	if (cont == MAXPROC)
		return;
	else
		initPcbs();
}


/**
 * Inserisce il PCB puntato da p  nella lista dei pcb liberi (pcbFree)
 * @return: l'elemento inserito
 */
void freePcb(struct pcb_t *p)
{
	p->p_next = (*pcbfree_h)->p_next;
	(*pcbfree_h)->p_next = p;
}

/**
 * Restituisce NULL se la pcbFree e' vuota. Altrimenti rimuove un elemento
 * dalla pcbFree, inizializza tutti i campi (NULL/0) e restituisce l'elemento
 * rimosso.
 * @return: NULL || l'elemento rimosso
 **/
struct pcb_t *allocPcb(void)
{
	return NULL;
}

/**
 * Inserisce l’elemento puntato da "p" nella coda dei processi puntata da "head".
 * L'inserimento deve avvenire tenendo conto della priorita' di ciascun "pcb"
 * (campo p->priority). La coda dei processi deve essere ordinata in base alla
 * priorita' dei "pcb", in ordine decrescente (i.e. l’elemento di testa e' l'elemento
 * con la priorita’ piu’ alta).
 */
void insertProcQ(struct pcb_t **head, struct pcb_t *p)
{
}

/*[5]
 * Descrizione: Restituisce l'elemento di testa della coda dei processi puntata da head,
 * senza rimuoverlo, ritorna null se la coda e vuota.
 * return: null || l'elemento di testa*/
struct pcb_t* headProcQ(struct pcb_t* head)
{
	return NULL;
}
	
/*[6]
 * Descrizione: rimuove il primo elemento dalla coda dei processi puntata da head.
 * Ritorna null se la coda e vuota, altrimenti ritorna il puntatore all'elemento
 * rimosso dalla lista
 * return: NULL || puntatore all'elemento rimosso*/
struct pcb_t* removeProcQ(struct pcb_t** head)
{ 
	return NULL;
}

/**
 * Rimuove il "pcb" puntato da "p" dalla coda dei processi puntata da "head".
 * Se "p" non e' presente nella coda, restituisce NULL.
 * NOTA: "p" puo' trovarsi in una posizione arbitraria della coda.
 */
struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p)
{
	return NULL;
}

/*[8]
 * Descrizione: Inserisce il pcb puntato da p come figlio del pcb puntato da parent*/
void insertChild(struct pcb_t *parent, struct pcb_t *p)
{
	return NULL;
}

/*[9]
 * Descrizione: Rimuove il primo figlio del pcb puntato da p, se p non ha figli restituisce nulll
 * return: NULL || primo figlio*/
struct pcb_t* removeChild(struct pcb_t *p)
{
	return NULL;
}

/*[10]
 * Descrizione:
 * Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre,
*  restituisce NULL. Altrimenti restituisce l'elemento rimosso (cioe' p). A differenza della removeChild, 
* p puo' trovarsi in una posizione arbitraria (ossia non e' necessariamente il primo figlio del padre)
* return: NULL || figlio*/
struct pcb_t* outChild(struct pcb_t* p)
{
	return NULL; 
}

void forallProcQ(struct pcb_t *head, void *fun(struct pcb_t *pcb, void *), void *arg)
{
}

