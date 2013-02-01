#ifndef _TYPES11_H
#define _TYPES11_H

#include <uMPStypes.h>

// Process Control Block (PCB) data structure
typedef struct pcb_t {
	/*process queue fields */
	struct pcb_t* p_next;

	/*process tree fields */
	struct pcb_t* p_parent;
	struct pcb_t* p_first_child;
	struct pcb_t* p_sib;

	/* processor state, etc */
	state_t	p_s;     

	/* process priority */
	int	priority;
	
	/* key of the semaphore on which the process is eventually blocked */
	int	*p_semkey;

} pcb_t;

// Semaphore Descriptor (SEMD) data structure
typedef struct semd_t {
	struct semd_t s_next;
	
	// Semaphore value
	int *s_key;
	
	// Queue of PCBs blocked on the semaphore
	pcb_t *s_procQ;
} semd_t;

#endif
