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


HIDDEN void initPcb_t (pcb_t *p)
{
	/* process queue fields */
 	p->p_next.next = NULL ;
 	p->p_next.prev = NULL ;
	
	/* process tree fields */
	p->p_parent = NULL;
	INIT_LIST_HEAD (&(p->p_child));
	p->p_sib.next = NULL;
	p->p_sib.prev = NULL;

	/* processor state, etc 
    * la struttura dati state_t e' una struttura contenente dati di tipo U32 che sono
    * un unsigned int della macchina su cui si trova
    */
	p->p_s.entry_hi = 0;
	p->p_s.cause = 0;
	p->p_s.status = 0;
	p->p_s.pc_epc = 0;  /* pc in the new area, epc in the old area */
	p->p_s.gpr[29] = 0;
	p->p_s.hi = 0;
	p->p_s.lo = 0;				

	/* process priority */
	p->priority = 0;
	
	/* key of the semaphore on which the process is eventually blocked */
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
	return list_add(&(p->p_next), &pcbfree_h);
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
	if(list_empty(&pcbfree_h)){
		return NULL;
	} else {
		temp = container_of (pcbfree_h.next, pcb_t, p_next); /* estrae il primo elemento della lista e lo salva in p */
		list_del (pcbfree_h.next); /* elimina l'elemento estratto dalla lista'*/
		initPcb_t(temp); /* inizializza il pcb */
		return temp; /* ritorna il pcb rimosso */
	}
}


void insertProcQ(struct pcb_t **head, struct pcb_t* p) {}

/*[5]
 * Descrizione: Restituisce l'elemento di testa della coda dei processi puntata da head,
 * senza rimuoverlo, ritorna null se la coda e vuota.
 * return: null || l'elemento di testa*/
struct pcb_t* headProcQ(struct pcb_t* head) {
	pcb_t *tmp;
	/*Se la sentinella punta a se stessa , significa che la coda e vuota e quindi ritorno null*/
	if(head->next == head){
		return NULL;
	} else {
		/* assegna l'elemento puntato da head a temp */
		temp = head->next;
		return temp;
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
	if(head->next == head){
		return NULL;
	} else {
		temp = head->next;
		head=tmp->next;
		return tmp;
	}
}
		
	

struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p) { return NULL; }
void insertChild(struct pcb_t *parent, struct pcb_t *p) {}
struct pcb_t* removeChild(struct pcb_t *p) { return NULL; }
struct pcb_t* outChild(struct pcb_t* p) { return NULL; }

