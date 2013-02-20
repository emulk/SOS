#ifndef ASL_E
#define ASL_E

#include <const13.h>
#include <types13.h>

void initASL();
struct semd_t* getSemd(int *key);

int insertBlocked(int *key, pcb_t* p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int key);
void outChildBlocked(pcb_t *p);

void forallBlocked(int *key, void *fun(struct pcb_t *pcb, void *), void *arg);

void debug(int a0, int a1);

#endif
