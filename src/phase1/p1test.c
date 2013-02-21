/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2012 by Marco Di Felice                                          *
 * Modified 2013 by Paolo Parisen Toldin                                     *
 * Modified 2013 by Renzo Davoli                                             *
 *                                                                           *
 * This file is part of the projct "strange operating systems" SOS 2013.     *
 *                                                                           *
 * SOS   is free software; you can redistribute it and/or modify it under the*
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P1TEST.C*******************************
 *
 *	Test program for the modules ASL and PCB (phase 1).
 *
 *	Produces progress messages on terminal 0 in addition
 *		to the array ``okbuf[]''
 *		Error messages will also appear on terminal 0 in
 *		addition to the array ``errbuf[]''.
 *
 *		Aborts as soon as an error is detected.
 *
 *
 */

#include "const13.h"
#include "uMPStypes.h"
#include "types13.h"
#include "libumps.h"

#include "pcb.e"
#include "asl.e"

#define	MAXSEM	MAXPROC

#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

#define TRANSMITTED	5
#define TRANSTATUS    2
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000250
#define DEVREGSIZE 16
#define READY     1
#define DEVREGLEN   4
#define TRANCOMMAND   3
#define BUSY      3

char okbuf[2048];			/* sequence of progress messages */
char errbuf[128];			/* contains reason for failing */
char msgbuf[128];			/* nonrecoverable error message before shut down */


int my_counter_process;
int onesem;
pcb_t	*procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
int  sem[MAXSEM];
int onesem;
pcb_t* head;

char *mp = okbuf;

typedef unsigned int devreg;


/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr *stataddr) {
	return((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if
 * print was successful, FALSE if not   */
unsigned int termprint(char * str, unsigned int term) {

	memaddr *statusp;
	memaddr *commandp;

	devreg stat;
	devreg cmd;

	unsigned int error = FALSE;

	if (term < DEV_PER_INT) {
		/* terminal is correct */
		/* compute device register field addresses */
		statusp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
		commandp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));

		/* test device status */
		stat = termstat(statusp);
		if ((stat == READY) || (stat == TRANSMITTED)) {
			/* device is available */

			/* print cycle */
			while ((*str != '\0') && (!error)) {
				cmd = (*str << CHAROFFSET) | PRINTCHR;
				*commandp = cmd;

				/* busy waiting */
				while ((stat = termstat(statusp)) == BUSY);

				/* end of wait */
				if (stat != TRANSMITTED) {
					error = TRUE;
				} else {
					/* move to next char */
					str++;
				}
			}
		}	else {
			/* device is not available */
			error = TRUE;
		}
	}	else {
		/* wrong terminal device number */
		error = TRUE;
	}

	return (!error);
}


/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {

	termprint(strp, 0);
}

/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp) {

	termprint(strp, 0);

	PANIC();
}

void increment_counter(struct pcb_t *pcb, void* pt)
{
	int *counter=pt;
	(*counter)++;
}

/******************************************************************************
 * Main Test File
 ******************************************************************************/

int main() {

	int i;

	head = NULL;

	addokbuf("Strange Operating System test\n");

	initPcbs();
	addokbuf("Control Blocks initialized  \n");
	if(MAXPROC == 20)
		addokbuf("No more than 20 proc\n");

	/* controlla la allocPcb */
	for (i = 0; i < MAXPROC; i++) {
		if ((procp[i] = allocPcb()) == NULL)
			adderrbuf("allocPcb(): unexpected NULL   ");
	}

	/* it should not allocate more than MAXPROC elements */
	if (allocPcb() != NULL) {
		adderrbuf(" ERROR: allocPcb(): allocated more than MAXPROC entries   ");
	}
	addokbuf(" allocPcb test OK   \n");
	addokbuf(" list initialized correctly  \n");

	/* Return the last 10 entries back to free list */
	for (i = 10; i < MAXPROC; i++)
		freePcb(procp[i]);

	addokbuf(" 10 entries were added to the free PCB list   \n");

	addokbuf("We are going to test the insertProcQ function...   \n");

	/* insert 10 elements using different priorities. one max, one min prio
	all the others default prio */
	for (i = 0; i < 10; i++) {
		q = allocPcb();
		if ( q == NULL)
			adderrbuf("ERROR: allocPcb(): unexpected NULL while insert   ");
		switch (i) {
			case 5:
				q->priority=DEFAULT_PCB_PRIORITY;
				proc=q;
				break;
			case 1:
				q->priority=MAX_PCB_PRIORITY;
				maxproc = q;
				break;
			case 2:
				q->priority=MIN_PCB_PRIORITY;
				minproc=q;
				break;
			default:
				q->priority=DEFAULT_PCB_PRIORITY;
				break;
		}
		/* insert the process in the queue */
		insertProcQ(&head, q);
	}

	addokbuf("Test insertProcQ: OK. Inserted 10 elements \n");

	addokbuf(" Checking if the first element has max priority\n");
	/* Check outProcQ and headProcQ */
	if (headProcQ(head) != maxproc)
		adderrbuf("ERROR: headProcQ(qa) failed   ");
	addokbuf("Done\n");

	/* remove an element */
	addokbuf("Removing an element. Testing outProcQ\n");
	q=outProcQ(&head, proc);
	if ((q == NULL) || (q != proc))
		adderrbuf("ERROR: outProcQ(&qa, proc) failed to remove the entry   ");
	freePcb(q); /* return the element to the free list */
	addokbuf("Done\n");

	/* Removing the first element from ProcQ */
	addokbuf("Removing the first element. Testing removeProcQ\n");
	q = removeProcQ(&head);
	if (q == NULL || q != maxproc)
		adderrbuf("ERROR: removeProcQ(&qa, midproc) failed to remove the elements in the right order   ");
	freePcb(q);
	addokbuf("Done\n");

	/* we inserted 10 elements and removed 2. Now let us remove the
	remaining elements and check if the last one has the min prio */
	addokbuf(" Testing removeProcQ ...   \n");
	for (i = 0; i < 7; i++) {
		if ((q = removeProcQ(&head)) == NULL)
			adderrbuf("removeProcQ(&qa): unexpected NULL   ");
		freePcb(q);
	}

	/* keeping track of the last element */
	q=headProcQ(head);
	addokbuf("Applying a function to all the elements of the list. Testing forallProcQ\n");
	/* there should be just one element inside the list. i'm going to remove by using the forallProcQ instruction */
	my_counter_process = 0;
	forallProcQ(head, increment_counter , &my_counter_process);
	if(my_counter_process != 1)
		adderrbuf("ERROR: forallProcQ(): failed on last entry   ");
	my_counter_process = 0;

	removeProcQ(&head);
	if (q != minproc)
		adderrbuf("ERROR: headProcQ(): failed on last entry   ");
	freePcb(q);
	addokbuf("Done\n");

	addokbuf("Let's see if now the queue is empty\n");
	addokbuf(" Test insertProcQ(), removeProcQ(), headProcQ(), forallProcQ(), outProcQ(): OK   \n");
	addokbuf(" Test process queues module: OK      \n");

	addokbuf(" Testing process trees...\n");

	/*check that it has no child*/
	q = removeChild(procp[2]);
	if ( q!=NULL )
		adderrbuf("ERROR: emptyChild: unexpected FALSE   ");
	/*if the test is passed, then the node has no child at all*/

	/* make procp[1],procp[2],procp[3], procp[7] children of procp[0] */
	addokbuf("Inserting...   \n");
	insertChild(procp[0], procp[1]);
	insertChild(procp[0], procp[2]);
	insertChild(procp[0], procp[3]);
	insertChild(procp[0], procp[7]);
	addokbuf("Inserted 2 children of pcb0  \n");

	/* make procp[8],procp[9] children of procp[7] */
	insertChild(procp[7], procp[8]);
	insertChild(procp[7], procp[9]);
	addokbuf("Inserted 2 children of pcb7  \n");


	/*checking if node 7 has a child. it it doesn't have, ERRRO*/
	q=removeChild(procp[7]);
	addokbuf("    removed the first child of 7\n");
	if(q==NULL)
		adderrbuf("ERROR: emptyChild: node 7 has no child! ");
	/*otherwise, we will re-insert*/
	insertChild(procp[7], q);

	addokbuf("   re-inserted son in 7, now removing in 0\n");

	/*check the same with node 0*/
	q=removeChild(procp[0]);
	addokbuf("    removed the first child of 0\n");
	if(q==NULL)
		adderrbuf("ERROR: emptyChild: node 7 has no child! ");
	/*otherwise, we will re-insert*/
	insertChild(procp[0], q);

	addokbuf(" Check outchild function\n");

	/* Check outChild */
	q = outChild(procp[1]);
	addokbuf("  outchild exectuted on 1\n");
	if (q == NULL || q != procp[1])
		adderrbuf("ERROR: outChild(procp[1]) failed ");

	q = outChild(procp[8]);
	addokbuf("  outchild exectuted on 8\n");
	if (q == NULL || q != procp[8])
		adderrbuf("ERROR: outChild(procp[8]) failed ");

	/* Check removeChild */
	q = removeChild(procp[0]);
	addokbuf("  removechild exectuted on 0\n");
	if (q == NULL || q != procp[2])
		adderrbuf("ERROR: removeChild(procp[0])  1 failed ");

	q = removeChild(procp[7]);
	addokbuf("  removechild exectuted on 7\n");
	if (q == NULL || q != procp[9])
		adderrbuf("ERROR: removeChild(procp[7])  2 failed ");

	q = removeChild(procp[0]);
	addokbuf("  removechild exectuted on 0\n");
	if (q == NULL || q != procp[3])
		adderrbuf("ERROR: removeChild(procp[0]) 2 failed ");

	q = removeChild(procp[0]);
	addokbuf("  removechild exectuted on 0\n");
	if (q == NULL || q != procp[7])
		adderrbuf("ERROR: removeChild(procp[0]) 3 failed ");

	q=removeChild(procp[0]);
	addokbuf("  removechild exectuted on 0\n");
	if ( q!=NULL )
		adderrbuf("ERROR: removeChild(procp[0]): it had already a child! it shouldn't! ");

	addokbuf("Test: insertChild(), removeChild() OK   \n");
	addokbuf("Testing process tree module OK      \n");


	freePcb(procp[0]);
	freePcb(procp[1]);
	freePcb(procp[2]);
	freePcb(procp[3]);
	freePcb(procp[4]);
	freePcb(procp[5]);
	freePcb(procp[6]);
	freePcb(procp[7]);
	freePcb(procp[8]);
	freePcb(procp[9]);


	addokbuf("Now starting with test of semaphores\n");

	/* check ASL */
	initASL();
	addokbuf("Initializing active semaphore list   \n");

	/* check removeBlo11cked and insertBlocked */
	addokbuf(" Test insertBlocked(): test #1 started  \n");
	for (i = 10; i < MAXPROC; i++) {
		procp[i] = allocPcb();
		if (insertBlocked(&sem[i], procp[i]))
			adderrbuf("ERROR: insertBlocked() test#1: unexpected TRUE   ");
	}

	addokbuf("Test insertBlocked(): test #2 started  \n");
	for (i = 0; i < 8; i++) {
		procp[i] = allocPcb();
		if (insertBlocked(&sem[i], procp[i]))
			adderrbuf("ERROR:insertBlocked() test #2: unexpected TRUE   ");
	}

	for (i = 8; i < 10; i++) {
		procp[i] = allocPcb();
		if (insertBlocked(&sem[1], procp[i]))
			adderrbuf("ERROR:insertBlocked() test #2: unexpected TRUE   ");
	}
	/*due semafori left*/


	addokbuf("Counting the number of elements inside semaphores\n");
	/*****************************************************************************************/
	/*counting the processes inside a semaphore*/
	/*first i reset to 0*/
	my_counter_process = 0;
	/*then i count*/
	forallBlocked(&sem[1], increment_counter, &my_counter_process);
	if(my_counter_process != 3)
		adderrbuf("ERROR forallBlocked, increment_counter. expected 3");
	else
		addokbuf("   i have found 3 elements in sem 1; correct\n'");

	/*counting the processes inside a semaphore*/
	/*first i reset to 0*/
	my_counter_process = 0;
	/*then i count*/
	forallBlocked(&sem[2], increment_counter, &my_counter_process);
	if(my_counter_process != 1)
		adderrbuf("ERROR forallBlocked, increment_counter. expected 1");
	else
		addokbuf("   i have found 1 elements in sem 2; correct\n");

	/*adjust*/
	p = removeBlocked(&sem[1]);
	if (insertBlocked(&sem[8],p))
		adderrbuf("ERROR: removeBlocked(): fails to return to free list   ");
	p = removeBlocked(&sem[1]);
	if (insertBlocked(&sem[9],p))
		adderrbuf("ERROR: removeBlocked(): fails to return to free list   ");
	/*now all the semaphores are used*/

	/*****************************************************************************************/

	addokbuf("Check if the semaphores are returned to the free list\n");
	/* check if semaphore descriptors are returned to the free list */
	p = removeBlocked(&sem[11]);
	if (insertBlocked(&sem[11],p))
		adderrbuf("ERROR: removeBlocked(): fails to return to free list   ");

	if (insertBlocked(&onesem, procp[9]) == FALSE)
		adderrbuf("ERROR: insertBlocked(): inserted more than MAXPROC   ");

	addokbuf("Test removeBlocked(): test started   \n");
	for (i = 10; i< MAXPROC; i++) {
		q = removeBlocked(&sem[i]);
		if (q == NULL) 
			adderrbuf("ERROR: removeBlocked(): wouldn't remove   ");
		if (q != procp[i]) /* different from what expected!*/
			adderrbuf("ERROR: removeBlocked(): removed wrong element   ");
	}

	addokbuf("test blocked done\n");

	if (removeBlocked(&sem[11]) != NULL)
		adderrbuf("ERROR: removeBlocked(): removed nonexistent blocked proc   ");

	addokbuf("Test insertBlocked() and removeBlocked() ok   \n");


	/* Creating a 2-layer tree */
	insertChild(procp[0], procp[1]);
	insertChild(procp[0], procp[2]);
	insertChild(procp[2], procp[3]);
	insertChild(procp[3], procp[4]);

	/* Testing outChildBlocked */
	outChildBlocked(procp[2]);
	addokbuf("I called the function outChildBlocked on procp[2]\n");

	/*i'm expecting that the child 2 has no children*/
	if(removeBlocked(&sem[2])!= NULL)
		adderrbuf("ERROR: removeBlocked(): nonNULL for a nonexistent queue (2)  ");
	if(removeBlocked(&sem[3])!= NULL)
		adderrbuf("ERROR: removeBlocked(): nonNULL for a nonexistent queue (3)  ");
	if(removeBlocked(&sem[4])!= NULL)
		adderrbuf("ERROR: removeBlocked(): nonNULL for a nonexistent queue (4)  ");

	if(removeBlocked(&sem[0])== NULL)
		adderrbuf("ERROR: removeBlocked(): nonNULL for a nonexistent queue (0)  ");
	if(removeBlocked(&sem[1])== NULL)
		adderrbuf("ERROR: removeBlocked(): nonNULL for a nonexistent queue (1)  ");


	addokbuf("ASL module OK   \n");
	addokbuf("Farewell to ye all! In the land of the stranger I rise or I fall - David Crockett\n");
	addokbuf("All the tests for SOS were correctly performed\n");
	addokbuf("\n");

	return 0;
}
