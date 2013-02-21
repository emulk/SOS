#####################################################
#       Makefile for Strange Operating System       #
#####################################################

SRC	= ./src
MOD	= $(SRC)/modules
INC	= $(SRC)/include
PH1	= $(SRC)/phase1
DEP	= $(MOD) $(INC) $(PH1)

## percorso locale
LIBDIR	= /usr/local/lib/umps2
INCDIR	= /usr/local/include/umps2/umps
ELF32DIR	= /usr/local/share/umps2

## percorso laboratorio
#LIBDIR	= /usr/lib/umps2
#INCDIR	= /usr/include/umps2/umps
#ELF32DIR	= /usr/share/umps2

# opzioni di compilazione/linking
CC	= mipsel-linux-gcc
LD	= mipsel-linux-ld
OPTIONS	= -Wall -W
CFLAGS	= -I$(MOD) -I$(INC) -I$(PH1) -I$(INCDIR) $(OPTIONS) -c
LDFLAGS	= -T
MKDEV		= umps2-mkdev
ELF2MPS	= umps2-elf2umps

all:	kernel.core.umps tape0.umps disk0.umps

tape0.umps:	kernel.core.umps
		$(MKDEV) -t tape0.umps kernel.core.umps

disk0.umps:	kernel.core.umps
		$(MKDEV) -d disk0.umps

# conversione del kernel in formato riconoscibile da uMPS2
kernel.core.umps: kernel
		$(ELF2MPS) -k kernel

# creazione kernel
kernel: 	pcb.o asl.o p1test.o
			$(LD) $(LDFLAGS) \
			$(ELF32DIR)/elf32ltsmip.h.umpscore.x \
			$(LIBDIR)/crtso.o \
			pcb.o \
			asl.o \
			p1test.o \
			$(LIBDIR)/libumps.o \
			-o kernel

# creazione moduli oggetto
pcb.o:	$(PH1)/pcb.c $(DEP)
			$(CC) $(CFLAGS) $(PH1)/pcb.c

asl.o:	$(PH1)/asl.c $(DEP)
			$(CC) $(CFLAGS) $(PH1)/asl.c

p1test.o:	$(PH1)/p1test.c $(DEP)
			$(CC) $(CFLAGS) $(PH1)/p1test.c

# rimozione file generati dopo la compilazione
clean:
		rm -r -f *~ asl.o pcb.o p1test.o kernel tape*.umps disk*.umps \
		printer*.umps term*.umps kernel.core.umps kernel.stab.umps
