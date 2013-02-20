#ifndef PCB_E
#define PCB_E

#include <const13.h>
#include <types13.h>
#include <listx.h>

void initPcbs(void);
void freePcb(struct pcb_t *p);
struct pcb_t *allocPcb(void);

void insertProcQ(struct pcb_t **head, struct pcb_t* p);
struct pcb_t* headProcQ(struct pcb_t* head);
struct pcb_t* removeProcQ(struct pcb_t** head);
struct pcb_t* outProcQ(struct pcb_t** head, struct pcb_t *p);

void forallProcQ(struct pcb_t *head, void *fun(struct pcb_t *pcb, void *), void *arg);

void insertChild(struct pcb_t *parent, pcb_t *p);
struct pcb_t* removeChild(struct pcb_t *p);
struct pcb_t * outChild(pcb_t* p);

#endif
