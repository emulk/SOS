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
HIDDEN pcb_t pcb_table[MAXPROC];

/* struttura list_head per la sentinella 
 * E la testa della lista pcbFree */ 
HIDDEN struct list_head pcbfree_h;

/**
 * TODO
 * Inizializza la lista dei processi liberi in modo da contenere
 * tutti gli elementi della pcb_table. 
 */
void initPcbs(void)
{
	int cont;

	// inizializzazione della lista dei processi liberi
	INIT_LIST_HEAD(&pcbfree_h);

	for (cont=0; cont<MAXPROC; cont++)
	{
		struct pcb_t *new = &(pcb_table[cont]);
		freePcb(new);
	}
}

/* TODO */
void freePcb(struct pcb_t *p) { }

/**
 * TODO
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
		;
	}
}


void insertProcQ(struct pcb_t **head, struct pcb_t* p) {}
struct pcb_t* headProcQ(struct pcb_t* head) { return NULL; }
struct pcb_t* removeProcQ(struct pcb_t** head) { return NULL; }
struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p) { return NULL; }
void insertChild(struct pcb_t *parent, struct pcb_t *p) {}
struct pcb_t* removeChild(struct pcb_t *p) { return NULL; }
struct pcb_t* outChild(struct pcb_t* p) { return NULL; }

