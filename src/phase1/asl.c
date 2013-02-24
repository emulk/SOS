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

#include <asl.e>

/*Array di semd con dimenzione massima maxproc*/
semd_t semd_table[MAXPROC];
/*Testa della lista dei semd liberi o inutilizzati*/
semd_t *semdFree_h;
/*Puntatore alla testa dei semafori attivi*/
semd_t *semd_h;


HIDDEN void insert(struct semd_t **testa, struct semd_t *elemento){
        if(*testa == NULL){
                *testa = elemento;
                elemento->s_next=NULL;
                return ;
        } else {
			/*scorro*/
               return insert(&((*testa)->s_next),elemento);
        }
}


HIDDEN struct semd_t *RemRec(struct semd_t **semd_h, struct semd_t *sem)
{
	if(*semd_h == sem)
	{
		semd_t *removed = sem;
		*semd_h = sem->s_next;
		insert(&semdFree_h, sem);
		return removed;
	}
	else
		return RemRec(&((*semd_h)->s_next), sem);
}


struct semd_t* Semd_ric(struct semd_t **tmp, int* key)
{
	if (*tmp == NULL){
		return NULL;
	}
	if ((*tmp)->s_key == key){
		return *tmp;
	} else {
		return Semd_ric(&((*tmp)->s_next), key);
	}
}

/**
 * Cerco il semaforo
 */

struct semd_t* getSemd(int *key)
{
	semd_t *tmp;
	tmp = semd_h;
	/*se non c'e ne sono di semafori attivi, restituisco null*/
	if (tmp == NULL){
		return NULL;
	}		
	/*se il semaforo cercato e in prima posizione*/
	if (tmp->s_key == key){
		return tmp;
	} else{
		tmp=tmp->s_next;
		return Semd_ric(&tmp,key);    
	}
}



/**
 * Inizializza la lista dei semdFree in modo da contenere tutti gli elementi 
 * della semdTable. Questo metodo viene invocato una volta sola durante 
 * l'inizializzazione della struttura dati.
 */
void initASL()
{
	static int i = 0;
	/*da 0 a 19*/
	if (i > MAXPROC-1)
		return;
	else
	{
		insert(&(semdFree_h), &(semd_table[i]));
		i++;
		initASL();
	}
}


struct semd_t *allocSem()
{
	/*se la lista dei semd liberi e vuota*/
	if(semdFree_h == NULL)
		return NULL;
	
	semd_t *tmp;
	tmp = semdFree_h;
	semdFree_h = semdFree_h->s_next;
	tmp->s_next = NULL;
	tmp->s_key = NULL;
	tmp->s_procQ = NULL;

	return tmp;   

}



 
/*Viene inserito il PCB puntato da p nella coda
dei processi bloccati associata al SEMD con chiave key. Se
il semaforo corrispondente non e’ presente nella ASL, alloca
un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo
inserisce nella ASL, settando I campi in maniera opportuna. Se
non e’ possibile allocare un nuovo SEMD perche’ la lista di
quelli liberi e’ vuota, restituisce TRUE. In tutti gli altri casi,
restituisce FALSE.
*/
int insertBlocked(int *key, pcb_t* p)
{
	semd_t *tmp;
	p->p_next=NULL;
	tmp = getSemd(key);
	/*se il semaforo non esiste*/
	if (tmp == NULL)
	{
		/**/
		tmp = allocSem(); 
		/*se la lista dei semd liberi e vuota*/
		if (tmp == NULL){ 
			return TRUE;
		}
		tmp->s_key = key;

		insert(&semd_h, tmp);
		
		p->p_semkey = key;
		insertProcQ(&(tmp->s_procQ), p);
		
	}
	else 
		insertProcQ(&(tmp->s_procQ), p); 
		
	return FALSE;   
}





/**
 * Ritorna il primo PCB dalla coda dei processi bloccati (s_ProcQ) associata al SEMD
 * della ASL con chiave key. Se tale descrittore non esiste nella ASL, restituisce NULL.
 * Altrimenti, restituisce l’elemento rimosso. Se la coda dei processi bloccati per il
 * semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo
 * inserisce nella coda dei descrittori liberi (semdFree).
 */
struct pcb_t* removeBlocked(int *key)
{
	semd_t *tmp;
	pcb_t* rem;
	tmp = getSemd(key);

	if(tmp == NULL){
		return NULL;
	}else{
		rem = removeProcQ(&(tmp->s_procQ));
		if (tmp->s_procQ == NULL)
		RemRec(&semd_h, tmp);
		return rem;
	}
	
}

/*
 * rimuove il pcb puntato da p dalla coda dei processi*/
pcb_t* outBlocked(pcb_t *p){
	semd_t *tmp;
	pcb_t *temp;
	tmp = getSemd(p->p_semkey);
	temp = outProcQ(&(tmp->s_procQ), p);
	/*se non esiste*/
	if(temp==NULL){
		return NULL;
	}else{
		return temp;
	}
}



/*Rimuove il PCB puntato da p
	dalla coda del semaforo su cui e’ bloccato, termina anche tutti i processi discendenti
*/
void outChildBlocked(pcb_t *p){
	semd_t *tmp;
	tmp = getSemd(p->p_semkey);
	outProcQ(&(tmp->s_procQ), p);
	if(p->p_first_child == NULL){
		p->p_first_child = NULL;
		return;
	}else{
		outChildBlocked(p->p_first_child);
	}
}



/*[7]
Descrizione:richiama la funzione fun per ogni provesso
 bloccato sul semaforo identificato da key
*/
void forallBlocked(int *key, void fun(struct pcb_t *pcb, void *), void *arg){
        semd_t *tmp;
        tmp = getSemd(key);
        if ((tmp->s_procQ)==NULL){
			return;
		} else {
			forallProcQ(tmp->s_procQ, fun, arg);
		}
}
     
