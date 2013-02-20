/**
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

static pcb_t pcb_table[MAXPROC+1];
static struct pcb_t *pcbfree_h;
static int initialized = FALSE;

/**
 * Inizializzazione della struttura dati di un processo
 */ 
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
 *
 */
void insertSibling(struct pcb_t *firstchild, struct pcb_t *p)
{
	if (firstchild->p_sib == NULL)
	{
		firstchild->p_sib = p;
		p->p_parent = firstchild->p_parent;
	}
	else
		insertSibling(firstchild->p_sib, p);
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
	if (head != NULL)
		return head;
		
	return NULL;
}

/**
 * Rimuove il primo elemento dalla coda dei processi puntata da "head".
 * @return NULL se la coda e' vuota, altrimenti ritorna il puntatore
 * all'elemento rimosso dalla lista
 */
struct pcb_t* removeProcQ(struct pcb_t** head)
{
	struct pcb_t *tmp = NULL;

	if (*head == NULL)
   	return NULL;

	tmp = *head;
	*head = (*head)->p_next;
	
	return tmp;
}

/**
 * Rimuove il "pcb" puntato da "p" dalla coda dei processi puntata da "head".
 * Se "p" non e' presente nella coda, restituisce NULL.
 * NOTA: "p" puo' trovarsi in una posizione arbitraria della coda.
 */
struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p)
{
	struct pcb_t *tmp = NULL;

	if (head == NULL)
   	 return NULL;
	else if (*head == p)
		return removeProcQ(head);
	else
		return outProcQ(&((*head)->p_next), p);
}

/**
 * Inserisce il pcb puntato da p come figlio del pcb puntato da parent
 */
void insertChild(struct pcb_t *parent, struct pcb_t *p)
{
	if (parent->p_first_child == NULL)
	{
		parent->p_first_child = p;
		p->p_parent = parent;
	}
	else
		insertSibling(parent->p_first_child,p);
}

/**
 * Rimuove il primo figlio del pcb puntato da p
 * @return: NULL se il processo non ha figli, oppure primo figlio
 */
struct pcb_t* removeChild(struct pcb_t *p)
{
	struct pcb_t *tmp = NULL;
	
	if (p->p_first_child == NULL)
		return NULL;
   
	tmp = p->p_first_child;
	p->p_first_child = tmp->p_sib;
	tmp->p_sib = NULL;
	tmp->p_parent = NULL;
	
	return tmp;
}


struct pcb_t *outChild_rec(struct pcb_t *list_child, struct pcb_t *p)
{
	if (list_child->p_sib == p)
	{
		struct pcb_t *removed = list_child->p_sib;
		list_child->p_sib = removed->p_sib;
		removed->p_sib = NULL;
		removed->p_parent = NULL;
		return removed;
	} else
		return outChild_rec(list_child->p_sib,p);
}

/**
 * Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da "p"
 * non ha un padre, restituisce NULL, altrimenti restituisce l'elemento rimosso (cioe' p).
 * A differenza della removeChild, "p" puo' trovarsi in una posizione arbitraria
 * (ossia non e' necessariamente il primo figlio del padre)
 */
struct pcb_t* outChild(struct pcb_t* p)
{
	struct pcb_t *list_child = (p->p_parent)->p_first_child;
	
	if (p->p_parent == NULL)
		return NULL;
	else if (list_child == p)
   	 return removeChild(p->p_parent);
	else {
		pcb_t *removed = outChild_rec(list_child,p);
		return removed;
	}

	return NULL;
}

/**
 * Richiama la funzione fun() per ogni elemento contenuto all'interno 
 * della lista "head"
 */
void forallProcQ(struct pcb_t *head, void *fun(struct pcb_t *pcb, void *), void *arg)
{
	if (head != NULL)
	{
		fun(head, arg);
		forallProcQ(head->p_next,fun,arg);
	}
}
