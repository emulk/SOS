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
/*Lista dei pcb che sono liberi o inutilizzati */
pcb_t pcbfree;
/*testa della lista pcbfree*/
pcb_t* pcbfree_h=&pcbfree;
/*array di pcb con dimensione massima di MAXPROC (20)*/
pcb_t pcbFree_table[MAXPROC];

/* implementazione ricorsiva*/

HIDDEN void InitPcbs(pcb_t **head){
	static int i = 0;
	i++;
	(*head)=pcbFree_table+i;
	(*head)->p_next=NULL;
	/*se i e uguale o maggiore di 20, ho finito */
	if(i>MAXPROC-1) return;
	/*altrimenti chiamo la ricorsione*/
	else InitPcbs(&(*head)->p_next);
}

HIDDEN reset(pcb_t *p){
	/*inizializzo tutti i 6 campi a NULL/0*/
	p->p_next=NULL;
	p->p_parent=NULL;
	p->p_first_child=NULL;
	p->p_sib=NULL;
	p->priority=0;
	p->p_semkey=NULL;
}

/*Inserisco p nella coda dei processi puntata da head*/
HIDDEN void insert(struct semd_t **testa, struct semd_t *elemento){
        if(*testa == NULL){
                *testa = elemento;
                elemento->s_next=NULL;
                return ;
        } else {
                return insert(&((*testa)->s_next),elemento);
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
	/*chiamo la insertProcQ , che prende in input la sentinella e il processo
	 * ed inserisce il processo puntato da p nella lista dei pcb liberi*/
	insertProcQ(&pcbfree_h, p);
}

/*Restituisce NULL se la
pcbFree e’ vuota. Altrimenti rimuove un
elemento dalla pcbFree, inizializza tutti i
campi (NULL/0) e restituisce l’elemento
rimosso.
*/
pcb_t *allocPcb(void){
	pcb_t *tmp;
	tmp=removeProcQ(&pcbfree_h);
	/*se la pcbFree e vuota restituisco NULL*/
	if (tmp==NULL)
		return NULL;
	/*dopo aver rimosso un elemento dalla pcbFree reseto tutti i campi a NULL/0*/
	reset(tmp);
	return tmp;
}

/*inserisce l’elemento puntato da p nella coda dei
processi puntata da head. L’inserimento deve avvenire tenendo
conto della priorita’ di ciascun pcb (campo p--‐>priority).
La coda dei processi deve essere ordinata in base alla priorita’
dei PCB, in ordine decrescente
*/
void insertProcQ(struct pcb_t **head, pcb_t *p){
	if((*head)->priority>p->priority){
		/*scorro la coda*/
		insertProcQ(&(*head)->p_next,p);
	} else {
		/*arrivato alla posizione giusta faccio l'inserimento*/
		p->p_next=(*head);
		(*head)=p;
	}
}

/*Restituisce l’elemento di testa della coda dei
processi da head, SENZA RIMUOVERLO. Ritorna NULL se la
coda non ha elementi.
*/
pcb_t *headProcQ(pcb_t *head){
	/*se la coda non ha elementi*/
	if(head == NULL){
		return NULL;
	} else {
		return head;
	}
}

/*rimuove il primo elemento dalla coda dei processi
puntata da head. Ritorna NULL se la coda e’ vuota. Altrimenti ritorna
il puntatore all’elemento rimosso dalla lista.
*/
pcb_t *removeProcQ(pcb_t **head){
	/*se la coda e vuota*/
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
		} else  {
			/*scorro la coda , se non trovo p nella prima posizione*/
			return outProcQ(&(*head)->p_next, p);
		}
	}
	/*se la coda e vuota, restituisco NULL*/
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

/*-------------------------------------------ALBERI------------------------------------------------------*/

/*Inserisce il PCB puntato da p come figlio del PCB puntato da parent.
*/
void insertChild(pcb_t *parent, pcb_t *p){
	if(p!=NULL && parent!=NULL){
		p->p_parent=parent;
		insert(&parent->p_first_child, p);
	} else {
		parent->p_first_child = p;
		p->p_parent = parent;
		p->p_sib = NULL;
	}
}

/*Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL
*/
pcb_t *removeChild(pcb_t *p){
	pcb_t *tmp;
	if (p!=NULL){
		tmp=removeProcQ(&p->p_first_child);
		if(tmp!=NULL)
			tmp->p_parent=NULL;
		return tmp;
	}
	/*se p non ha figli*/
	return NULL;
}

/*Rimuove il PCB puntato da p dalla lista dei
figli del padre. Se il PCB puntato da p non ha un
padre, res*tuisce NULL. Altrimenti restituisce l’elemento
rimosso (cioe’ p). A differenza della removeChild, p puo’
trovarsi in una posizione arbitraria
*/
pcb_t *outChild(pcb_t *p){
	/*se p non ha padre*/
	if(p->p_parent==NULL)
		return NULL;
	pcb_t *parent=p->p_parent;
	return outProcQ(&parent->p_first_child, p);

}
