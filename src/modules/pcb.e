#ifndef PCB_E
#define PCB_E
#include <const13.h>
#include <types13.h>

/* PCB handling functions */

/* PCB free list handling functions */
void initPcbs(void);
void freePcb(pcb_t *p);
pcb_t *allocPcb(void);


/* PCB queue handling functions */
void insertProcQ( pcb_t **head, pcb_t* p);
pcb_t* headProcQ(pcb_t* head);
pcb_t* removeProcQ( pcb_t** head);
pcb_t* outProcQ( pcb_t** head, pcb_t *p);


/*
this can be inlined or implemented as preprocessor macro
*/
void forallProcQ(struct pcb_t *head, void fun(struct pcb_t *pcb, void *), void *arg);


/* Tree view functions */
void insertChild(pcb_t *parent, pcb_t *p);
pcb_t* removeChild(pcb_t *p);
pcb_t * outChild(pcb_t* p);



#endif
