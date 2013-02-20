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

#include <asl.e>
#include <pcb.e>

struct semd_t semd_table[MAXPROC];   // vettore dei semafori
struct semd_t *semdfree_h;           // testa della lista dei semfarori liberi
struct semd_t *semd_h;               // puntatore alla lista dei semafori attivi


/**
 * Inserisce un elemento in coda alla lista
 */
void insert(struct semd_t **head, struct semd_t *elem)
{
	if (*head == NULL)
	{
		*head = elem;
		elem->s_next = NULL;
	} 
	else
		return insert(&((*head)->s_next), elem);    
}

/**
 * Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della
 * semdTable. Questo metodo viene invocato una volta sola durante l’inizializzazione
 * della strutuura dati.
 */
void initASL()
{
	static int cont = 0;
	
	insert(&(semdfree_h), &(semd_table[cont++]));
	
	if (cont == MAXPROC)
		return;
	
	initASL();
}

/**
 * Funzione di supporto per getSemd()
 * La funzione controlla se il semaforo con chiave "key" si trova
 * nella lista dei semafori attivi.
 * @return NULL se il semaforo non e' attivo, altrimenti un puntatore
 * al semaforo con chiave pari a "key"
 */
struct semd_t* semd_rec(struct semd_t **semd_h, int *key)
{
	if ((*semd_h)->s_key == key)
		return *semd_h;
	
	return semd_rec(&((*semd_h)->s_next), key);
}

/**
 * Estrazione di un descrittore di semaforo dalla lista dei semafori attivi,
 * la cui chiave e' pari a "key".
 * @return NULL la lista e' vuota; altrimenti un puntatore al SEMD estratto
 * dalla lista dei semafori attivi.
 */
struct semd_t* getSemd(int *key)
{	
	if (semd_h == NULL)
		return NULL;
	
	return semd_rec(&semd_h, key);
}

/**
 * Viene inserito il PCB puntato da "p" nella coda dei processi bloccati associata
 * al SEMD con chiave key. Se il semaforo corrispondente non e' presente nella ASL,
 * alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella ASL,
 * settando I campi in maniera opportuna. Se non e' possibile allocare un nuovo SEMD
 * perche’ la lista di quelli liberi e’ vuota, restituisce TRUE. In tutti gli altri casi,
 * restituisce FALSE.
 */
int insertBlocked(int *key, struct pcb_t* p)
{
	struct semd_t *sem = getSemd(key);
	if (sem == NULL)
	{
		if (semdfree_h == NULL)
			return TRUE;
		else
		{
			struct semd_t *newsem = semdfree_h;
			semdfree_h = semdfree_h->s_next;

			// inizializzazione semaforo
			newsem->s_next = NULL;
			newsem->s_procQ = NULL;
			newsem->s_key = key;
			
			insertProcQ(&newsem->s_procQ, p);
			p->p_semkey = key;
		}
	}
	else
	{
		// inserimento processo nella coda dei processi bloccati al semaforo
		insertProcQ(&sem->s_procQ, p);
		p->p_semkey = key;
	}
	

	return FALSE;
}

struct pcb_t* removeBlocked(int *key) { return NULL; }
struct pcb_t* outBlocked(struct pcb_t *p) { return NULL; }
struct pcb_t* headBlocked(int key) { return NULL; }
void outChildBlocked(struct pcb_t *p) { }

/**
 * Funzione di supporto per creare la ricorsione;
 */
void forallBlocked_r(struct pcb_t *head, void *fun(struct pcb_t *pcb, void*), void *arg)
{
	if (head != NULL)
	{
		fun(head, arg);
		forallBlocked_r(head->p_next, fun, arg);
	}
}

/**
 * Richiama la funzione fun() per ogni processo bloccato sulla chiave "key"
 */
void forallBlocked(int *key, void *fun(struct pcb_t *pcb, void *), void *arg) 
{
	struct semd_t *sem = getSemd(key);
	if (sem != NULL)
		forallBlocked_r(sem->s_procQ, fun, arg);
}
