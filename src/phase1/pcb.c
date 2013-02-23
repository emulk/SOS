/*
*  Copyright (C) 2011 Orgest Shehaj & Fabian Priftaj & Eduart Uzeir
*  Author: Orgest Shehaj <shehaj@cs.unibo.it>
*          Fabian Priftaj <priftaj@cs.unibo.it>
*             Eduart Uzeir <uzeir@cs.unibo.it>
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
pcb_t pcbfree;
pcb_t* pcbfree_h=&pcbfree;
pcb_t pcbFree_table[MAXPROC];

/* implementazione ricorsiva*/

HIDDEN void InitPcbs(pcb_t **head){
	static int i = 0;
	i++;
	(*head)=pcbFree_table+i;
	(*head)->p_next=NULL;
	if(i>MAXPROC-1) return;
	else InitPcbs(&(*head)->p_next);
}

HIDDEN reset(pcb_t *p){
	
	p->p_next=NULL;
	p->p_parent=NULL;
	p->p_first_child=NULL;
	p->p_sib=NULL;
	p->priority=0;
	p->p_semkey=NULL;
}

HIDDEN void insert(struct pcb_t **head,pcb_t *p){
	if((*head)!=NULL){
		insert(&(*head)->p_next,p);
	} else {
		(*head)=p;
		p->p_next=NULL;
	}
}

/*Inizializza la pcbFree in
modo da contenere tutti gli elementi della
pcbFree_table. Questo metodo deve
essere chiamato una volta sola in fase
di inizializzazione della struttura dati.
*/
void initPcbs(void){
	InitPcbs(&pcbfree_h);
}

/*Inserisce il PCB puntato
da p nella lista dei PCB liberi (pcbFree)
*/
void freePcb(pcb_t *p){
	reset(p);
	insertProcQ(&pcbfree_h, p);
}

/*Restituisce NULL se la
pcbFree e’ vuota. Altrimenti rimuove un
elemento dalla pcbFree, inizializza tutti i
campi (NULL/0) e restituisce l’elemento
rimosso.
*/
pcb_t *allocPcb(void){
	pcb_t *out;
	out=removeProcQ(&pcbfree_h);
	if (out==NULL)
		return NULL;
	reset(out);
	return out;
}

/*inserisce l’elemento puntato da p nella coda dei
processi puntata da head. L’inserimento deve avvenire tenendo
conto della priorita’ di ciascun pcb (campo p--‐>priority).
La coda dei processi deve essere ordinata in base alla priorita’
dei PCB, in ordine decrescente
*/
void insertProcQ(struct pcb_t **head, pcb_t *p){
	if((*head)->priority>p->priority){
		insertProcQ(&(*head)->p_next,p);
	} else {
		p->p_next=*head;
		*head=p;
	}
}

/*Restituisce l’elemento di testa della coda dei
processi da head, SENZA RIMUOVERLO. Ritorna NULL se la
coda non ha elementi.
*/
pcb_t *headProcQ(pcb_t *head){
	return head;
}

/*rimuove il primo elemento dalla coda dei processi
puntata da head. Ritorna NULL se la coda e’ vuota. Altrimenti ritorna
il puntatore all’elemento rimosso dalla lista.
*/
pcb_t *removeProcQ(pcb_t **head){
	if (*head==NULL)
		return NULL;
	return outProcQ(head, headProcQ(*head));
}

/*Rimuove il PCB puntato da p dalla coda dei processi
puntata da head. Se p non e’ presente nella coda, restituisce NULL.
*/
pcb_t *outProcQ(pcb_t **head, pcb_t *p){
	if((*head)!=NULL){
		if(*head==p){
			(*head)=(*head)->p_next;
			return p;
		}
		else if((*head)->p_next==p){
			(*head)->p_next=p->p_next;
			return p;
		}
		return outProcQ(&(*head)->p_next, p);
	}
	return NULL;
}


/*richiama la funzione fun per ogni elemento della lista puntata da head.
*/
void forallProcQ(struct pcb_t *head, void fun(struct pcb_t *pcb, void *), void *arg){
	if(head!=NULL){
		fun(head, arg);
		forallProcQ(head->p_next, fun, arg);
	}
	return;
}


/*Inserisce il PCB puntato da parent.
*/
void insertChild(pcb_t *parent, pcb_t *p){
	if(p!=NULL && parent!=NULL){
		p->p_parent=parent;
		insert(&parent->p_first_child, p);
	}
}

/*Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL
*/
pcb_t *removeChild(pcb_t *p){
	if (p!=NULL){
		pcb_t *out=removeProcQ(&p->p_first_child);
		if(out!=NULL)
			out->p_parent=NULL;
		return out;
	}
	return NULL;
}

/*Rimuove il PCB puntato da p dalla lista dei
figli del padre. Se il PCB puntato da p non ha un
padre, res*tuisce NULL. Altrimenti restituisce l’elemento
rimosso (cioe’ p). A differenza della removeChild, p puo’
trovarsi in una posizione arbitraria
*/
pcb_t *outChild(pcb_t *p){
	if(p->p_parent==NULL)
		return NULL;
	pcb_t *parent=p->p_parent;
	return outProcQ(&parent->p_first_child, p);

}
