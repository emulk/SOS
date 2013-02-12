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
 
#include "../modules/pcb.e"

/* array di pcb con dimensione MAXPROC (MAXPROC = 20) */
HIDDEN pcb_t pcb_table[MAXPROC];

/* struttura list_head per la sentinella 
 * E la testa della lista pcbFree */ 
HIDDEN struct list_head pcbfree_h;

// permette di verificare se la lista dei processi liberi
// e' stato inizializzata
static int initialized = FALSE;



/**
 * Inizializzazione di una struttura "pcb_t"
 */
HIDDEN void initPcb_t(struct pcb_t *p)
{
	int count;

 	p->p_next = NULL ;
 	p->p_sib = NULL;
	p->p_parent = NULL;
	p->p_first_child = NULL;

	/* inizializzazione stati del processo */
	p->p_s.entry_hi = 0;
	p->p_s.cause    = 0;
	p->p_s.status   = 0;
	p->p_s.pc_epc   = 0;
	p->p_s.hi       = 0;
	p->p_s.lo       = 0;	
	
	/* inizializzazione registri ad uso generale */
	for (count=0; count<30; count++)
		p->p_s.gpr[count] = 0;			

	/* priorita' del processo */
	p->priority = 0;
	
	/* chiave del semaforo sul quale il processo puo' bloccarsi */
	p->p_semkey = 0;
}
 	

/**
 * Inizializza la lista dei processi liberi in modo da contenere
 * tutti gli elementi della pcb_table.
 */
void initPcbs(void)
{
	static int cont = 0;

	if (initialized == FALSE)
	{	
		INIT_LIST_HEAD(&pcbfree_h);
		initialized = TRUE;
	}
	
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
void freePcb(struct pcb_t *p) { 
	pcb_t *tmp;
	tmp=pcbfree_h.next;
	pcbfree_h.next=p;
	p->p_sib=tmp;
	return p;
	//return list_add(&(p->p_next), &pcbfree_h);
	
}

/**
 * Restituisce NULL se la pcbFree e' vuota. Altrimenti rimuove un elemento
 * dalla pcbFree, inizializza tutti i campi (NULL/0) e restituisce l'elemento
 * rimosso.
 * @return: NULL || l'elemento rimosso
 **/
struct pcb_t *allocPcb(void)
{
	/* creo una variabile temporanea*/
	pcb_t *temp;
	
	/*Se la pcbFree e vuota restituisco NULL*/
	if(&(pcbfree_h.next) == &(pcbfree_h)){
		return NULL;
	} else {
		temp = container_of (pcbfree_h.next, pcb_t, p_next); /* estrae il primo elemento della lista e lo salva in p */
		list_del (pcbfree_h.next); /* elimina l'elemento estratto dalla lista'*/
		//initPcb_t(temp); /* inizializza il pcb */
		temp=NULL;
		return temp; /* ritorna il pcb rimosso */
	}
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

	if ((*head) != NULL)
	{
		if ((*head)->priority > p->priority)	
			insertProcQ(&(*head)->p_next, p);
		else
		{
    		p->p_next = *head;
    		*head = p;
		}
	}
}

/*[5]
 * Descrizione: Restituisce l'elemento di testa della coda dei processi puntata da head,
 * senza rimuoverlo, ritorna null se la coda e vuota.
 * return: null || l'elemento di testa*/
struct pcb_t* headProcQ(struct pcb_t* head) {
	pcb_t *tmp;
	/*Se la sentinella punta a se stessa , significa che la coda e vuota e quindi ritorno null*/
	if((head->p_next) == head || (head->p_next) == NULL){
		return NULL;
	} else {
		/* assegna l'elemento puntato da head a temp */
		tmp = &(head->p_next);
		return tmp;
	}
}
	
/*[6]
 * Descrizione: rimuove il primo elemento dalla coda dei processi puntata da head.
 * Ritorna null se la coda e vuota, altrimenti ritorna il puntatore all'elemento
 * rimosso dalla lista
 * return: NULL || puntatore all'elemento rimosso*/
struct pcb_t* removeProcQ(struct pcb_t** head) { 
	pcb_t *tmp;
	/*se la coda e vuota ritorna null*/
	if (((*head)->p_next) == head || ((*head)->p_next) == NULL) {
		return NULL;
	} else {
		/*il primo elemento della coda*/
		tmp = (*head)->p_next;
		/*elimino il primo elemento della coda*/
		(*head)=tmp->p_next;
		return tmp;
	}
}

/**
 * Rimuove il "pcb" puntato da "p" dalla coda dei processi puntata da "head".
 * Se "p" non e' presente nella coda, restituisce NULL.
 * NOTA: "p" puo' trovarsi in una posizione arbitraria della coda.
 */
struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p)
{
	if ((*head) != NULL)
	{
		if ((*head) == p)
		{
			struct pcb_t *temp = (*head)->p_next;
			// *head = NULL ??? 
			(*head) = temp;
		}
		else
			outProcQ(&(*head)->p_next, p);
	}
	
	return NULL;
}

/*[8]
 * Descrizione: Inserisce il pcb puntato da p come figlio del pcb puntato da parent*/
void insertChild(struct pcb_t *parent, struct pcb_t *p) {
	/*se parent ha gia dei figli, accoda p ai figli di parent*/
	pcb_t *tmp;
	tmp=&(parent->p_first_child);
	(parent->p_first_child) = p;
	(p->p_next)=tmp;
	
	/*dopo aver aggiunto p come figlio di parent, imposta parent come padre di p*/
	(p->p_parent)=parent;
}

/*[9]
 * Descrizione: Rimuove il primo figlio del pcb puntato da p, se p non ha figli restituisce nulll
 * return: NULL || primo figlio*/
struct pcb_t* removeChild(struct pcb_t *p) {
	pcb_t *figlio;
	pcb_t *fratello;
	/*se non ci sono figli restituisco null*/
	if(&(p->p_first_child) == NULL ){
		return NULL;
	} else {
		/*asegno il filgio di p a figlio*/
		figlio = p->p_first_child;
		/*asegno il fratello del figlio di p, a fratello*/
		fratello=figlio->p_sib;
		/*asegno come figlio di p, il fratello del figlio di p*/
		p->p_first_child=fratello;
		
		figlio->p_parent=NULL; /* p non ha piu il puntatore al padre */
		return figlio;
	}
}

/*[10]
 * Descrizione:
 * Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre,
*  restituisce NULL. Altrimenti restituisce l'elemento rimosso (cioe' p). A differenza della removeChild, 
* p puo' trovarsi in una posizione arbitraria (ossia non e' necessariamente il primo figlio del padre)
* return: NULL || figlio*/
struct pcb_t* outChild(struct pcb_t* p) { 
	/*controllo se p ha un padre.Se non ha un padre restituisco NULL*/
	if ((&(p->p_parent))== NULL) { 
		return NULL;
		}else {
            list_del (&(p->p_sib)); /*elimino elemento p dall'albero*/
            
            p->p_parent = NULL;
            return p;
       }
}

