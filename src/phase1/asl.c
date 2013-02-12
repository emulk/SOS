#include <asl.e>

semd_t* getSemd(int key) { return NULL; }
void initASL() { }
int insertBlocked(int key,pcb_t* p) { return 0; }
pcb_t* removeBlocked(int *key) { return NULL; }
pcb_t* outBlocked(pcb_t *p) { return NULL; }
pcb_t* headBlocked(int key) { return NULL; }
void outChildBlocked(pcb_t *p) { }
void forallBlocked(int *key, void *fun(struct pcb_t *pcb, void *), void *arg) {}
