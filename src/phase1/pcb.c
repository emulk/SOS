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

// funzione di stampa per il debug
extern void addokbuf(char *str);

static pcb_t pcb_table[MAXPROC+1];
static struct pcb_t *pcbfree_h;

void debug(int a0, int a1) {
	return;
}

static int initialized = FALSE;
 
void initPcb(struct pcb_t *p)
{
	int count = 0;

 	p->p_next = NULL ;
	p->p_first_child = NULL;
	p->p_parent = NULL;
	p->p_sib = NULL;
	
	// inizializzazione campi processo
	p->p_next = NULL;
	p->p_parent = NULL;
	p->p_first_child = NULL;
	p->p_sib = NULL;
		
	// inizializzazione dei campi del processo
	p->p_s.entry_hi = 0;
	p->p_s.cause    = 0;
	p->p_s.status   = 0;
	p->p_s.pc_epc   = 0;
	p->p_s.hi       = 0;
	p->p_s.lo       = 0;

	// inizializzazione registri ad uso generale
	for (count=0; count<29; count++)
		p->p_s.gpr[count] = 0;

	// inizializzazione priorita' processo
	p->priority = 0;

	// inizializzazione chiave del semaforo 
	// new->p_semkey = NULL;
} 


/**
 * Inizializzazione lista processi liberi e della memoria contenente
 * i processi
 */
void initPcbs(void)
{
	static int cont = 0;
	
	if (initialized == FALSE)
	{
		pcbfree_h = &(pcb_table[MAXPROC]);
		pcbfree_h->p_next = pcbfree_h;
		pcbfree_h->p_parent = NULL;
		pcbfree_h->p_sib = NULL;
		pcbfree_h->p_first_child = NULL;
		
		initialized = TRUE;
	}
	
	struct pcb_t *new = &(pcb_table[cont]);
	freePcb(new);

	if (++cont == MAXPROC)
		return;
	else
		initPcbs();
}


/**
 * I processi non piu' utilizzati vengono inseriti all'interno della
 * lista dei processi liberi.
 */
void freePcb(struct pcb_t *p)
{
	p->p_next = pcbfree_h->p_next;
	pcbfree_h->p_next = p;
}

/**
 * Rimozione di un processo dalla lista dei processi liberi, e successivamente
 * vengono inizializzati tutti i suoi campi cosiche' possa essere riutilizzato.
 * @return NULL se la lista dei processi liberi e' vuota, altrimenti viene
 * restituito un puntatore al processo rimosso.
 **/
struct pcb_t *allocPcb(void)
{
	struct pcb_t *new = NULL;

	if ((pcbfree_h)->p_next != (pcbfree_h))
	{
		// punto al primo processo in testa alla lista
		new = (pcbfree_h)->p_next;
		
		// rimozione del processo dalla lista
		(pcbfree_h)->p_next = new->p_next;
		initPcb(new);
	}

	return new;
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
	while ((*head) != NULL)
	{
		if ((*head)->priority < p->priority)
			break;
		head = &(*head)->p_next;
	}
	
	p->p_next = *head;
	*head = p;
}

/**
 * Restituisce l'elemento di testa della coda dei processi puntata da head,
 * senza rimuoverlo, ritorna null se la coda e vuota.
 * @return: NULL se la lista e' vuota, oppure l'elemento estratto.
 */
struct pcb_t* headProcQ(struct pcb_t* head)
{
	struct pcb_t *tmp = NULL;
	if (head != NULL)
		tmp = &(head->p_next);
		
	return tmp;
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
	struct pcb_t *tmp = NULL;
	while ((*head) != NULL)
	{
		if ((*head)->p_next == p) {
			tmp = &(*head)->p_next->p_next;
			(*head)->p_next = tmp;
		}
		
		head = &(*head)->p_next;
	}
	
	return tmp;
}

/* [8]
 * Descrizione: Inserisce il pcb puntato da p come figlio del pcb puntato da parent*/
void insertChild(struct pcb_t *parent, struct pcb_t *p)
{
	return NULL;
}

/* [9]
 * Descrizione: Rimuove il primo figlio del pcb puntato da p, se p non ha figli restituisce nulll
 * return: NULL || primo figlio*/
struct pcb_t* removeChild(struct pcb_t *p)
{
	return NULL;
}

/* [10]
 * Descrizione:
 * Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre,
 * restituisce NULL. Altrimenti restituisce l'elemento rimosso (cioe' p). A differenza della removeChild, 
 * p puo' trovarsi in una posizione arbitraria (ossia non e' necessariamente il primo figlio del padre)
 * return: NULL || figlio*/
struct pcb_t* outChild(struct pcb_t* p)
{
	return NULL; 
}

void forallProcQ(struct pcb_t *head, void *fun(struct pcb_t *pcb, void *), void *arg)
{
}

