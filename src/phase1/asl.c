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


struct semd_t* Semd_ric(struct semd_t **tmp, int* key)
{
	if (*tmp == NULL)
		return NULL;
	
	if ((*tmp)->s_key == key)
		return *tmp;
	else
		return Semd_ric(&((*tmp)->s_next), key);
}

/**
 * Gestione della lista dei semd attivi
 */
struct semd_t* getSemd(int *key)
{
	struct semd_t *tmp = semd_h;
	
	if (tmp == NULL)
		return NULL;
		
	if (tmp->s_key == key)
		return tmp;
	else
	{
		tmp=tmp->s_next;
		return Semd_ric(&tmp,key);    
	}
}

void insert(struct semd_t **testa, struct semd_t *elemento)
{
        if(*testa == NULL){
                *testa = elemento;
                elemento->s_next=NULL;
                return ;
        } else {
                return insert(&((*testa)->s_next),elemento);
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
	if(semdFree_h == NULL)
		return NULL;
	
	struct semd_t *ptemp = semdFree_h;
	semdFree_h = semdFree_h->s_next;
	ptemp->s_next = NULL;
	ptemp->s_key = NULL;
	ptemp->s_procQ = NULL;

	return ptemp;   

}

/**
 * Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al
 * SEMD con chiave key. Se il semaforo corrispondente non e' presente nella ASL,
 * alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella
 * ASL, settando I campi inmaniera opportuna. Se non e’ possibile allocare un nuovo
 * SEMD perche’ la lista di quelli liberi e’ vuota, restituisce TRUE. In tutti gli altri
 * casi, restituisce FALSE.
 */
int insertBlocked(int *key, pcb_t* p)
{
	struct semd_t *semd_target = getSemd(key);
	
	if (semd_target == NULL)
	{
		semd_target = allocSem(); 
		
		if (semd_target == NULL) 
			return TRUE;
	
		semd_target->s_key = key;

		insert(&semd_h, semd_target);
		p->p_semkey = key;
		insertProcQ(&(semd_target->s_procQ), p);
	}
	else
		insertProcQ(&(semd_target->s_procQ), p); 
		
	return FALSE;   
}

struct semd_t *deAllocSem(struct semd_t **semd_h, struct semd_t *sem)
{
	if(*semd_h == sem)
	{
		semd_t *removed = sem;
		*semd_h = sem->s_next;
		insert(&semdFree_h, sem);
		return removed;
	}
	else
		return deAllocSem(&((*semd_h)->s_next), sem);
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
	struct semd_t *semd_target = getSemd(key);

	if(semd_target)
	{
		pcb_t *removed = removeProcQ(&(semd_target->s_procQ));
		if (semd_target->s_procQ == NULL)
		deAllocSem(&semd_h, semd_target);
		return removed;
	}
	else{
		return NULL;
	}
}

/*[5]
Descrizione:*/
pcb_t* outBlocked(pcb_t *p){
	semd_t *tmp;
	pcb_t *temp;
	tmp = getSemd(p->p_semkey);
	temp = outProcQ(&(tmp->s_procQ), p);
	if(temp==NULL){
		return NULL;
	}else{
		return temp;
	}
}



/*[6]
Descrizione:Rimuove il PCB puntato da p
	dalla coda del semaforo su cui e’ bloccato
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
        semd_t *tmp = getSemd(key);
        if ((tmp->s_procQ)==NULL){
			return;
		} else {
			forallProcQ(tmp->s_procQ, fun, arg);
		}
}
     
