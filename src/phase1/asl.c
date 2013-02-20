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

struct semd_t semd_table[MAXPROC];   // vettore dei semafori
struct semd_t *semdFree_h;           // testa della lista dei semfarori liberi
struct semd_t *semd_h;               // puntatore alla lista dei semafori attivi


/**
 * Inserisce un elemento in coda alla lista
 */
void insert_tail(struct semd_t **head, struct semd_t *elem)
{
	if (*head == NULL)
	{
		*head = elem;
		elem->s_next = NULL;
	} 
	else
		return insert_tail(&((*head)->s_next), elem);    
}

/**
 * Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della
 * semdTable. Questo metodo viene invocato una volta sola durante l’inizializzazione
 * della strutuura dati.
 */
void initASL()
{
	static int cont = 0;
	
	insert_tail(&(semdFree_h), &(semd_table[cont++]));
	
	if (cont == MAXPROC)
		return;
	
	initASL();
}


struct semd_t* getSemd(int key) { return NULL; }
int insertBlocked(int key, struct pcb_t* p) { return 0; }
struct pcb_t* removeBlocked(int *key) { return NULL; }
struct pcb_t* outBlocked(struct pcb_t *p) { return NULL; }
struct pcb_t* headBlocked(int key) { return NULL; }
void outChildBlocked(struct pcb_t *p) { }
void forallBlocked(int *key, void *fun(struct pcb_t *pcb, void *), void *arg) {}
