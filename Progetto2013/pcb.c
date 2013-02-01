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
 
 #include "pcb.e"


/*array di pcb con dimensione MAXPROC (MAXPROC = 20) */
HIDDEN pcb_t pcb_table[MAXPROC];

/*struttura list_head per la sentinella 
 * E la testa della lista pcbFree*/ 
HIDDEN struct list_head pcbfree_h; 


/* PCB free list handling functions */
void initPcbs(void){
	
	
	
}
void freePcb(pcb_t *p){
	
	
	
	
}

/* [3]
 * DESCRIZIONE: Restituisce NULL se la pcbFree e' vuota. Altrimenti rimuove un elemento dalla pcbFree,
 * inizializza tutti i campi (NULL/0) e restituisce l'elemento rimosso.
 * @param: void
 * @return: NULL || l'elemento rimosso
 * */
pcb_t *allocPcb(void){
	/* creo una variabile temporanea*/
	pcb_t *temp;
	/*Se la pcbFree e vuota restituisco NULL*/
	if(list_empty(&pcbfree_h)){
		return NULL;
	} else {
		
	
	
	
	
	
	
}


/* PCB queue handling functions */
void insertProcQ( pcb_t **head, pcb_t* p){
	
	
	
	
	
}
pcb_t headProcQ(pcb_t* head){
	
	
	
	
	
}
pcb_t* removeProcQ( pcb_t** head){
	
	
	
	
}
pcb_t* outProcQ( pcb_t** head, pcb_t *p){
	
	
	
	
}
void insertChild(pcb_t *parent, pcb_t *p){
	
	
	
	
	
}
pcb_t* removeChild(pcb_t *p){
	
	
	
	
	
}

pcb_t * outChild(pcb_t* p){
	
	
	
	
	
}

/*
this can be inlined or implemented as preprocessor macro
void forallProcQ(struct pcb_t *head, void *fun(struct pcb_t *pcb, void *), void *arg);
*/

/* Tree view functions */
void insertChild(pcb_t *parent, pcb_t *p){
	
	
	
	
}
pcb_t* removeChild(pcb_t *p){
	
	
	
	
}


pcb_t * outChild(pcb_t* p){
	
	
	
	
}
