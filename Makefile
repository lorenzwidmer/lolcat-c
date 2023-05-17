BUILDDIR=./build

CC=g++
CFLAGS=-I$(IDIR)

ODIR=./
LDIR =./lib
IDIR =./include

LIBS=-lm

_OBJ = include/*.cc include/ansiescape/*.cc
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

lolcat: $(OBJ)
	$(CC) -o $(BUILDDIR)/lolcat -g $@.c $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ 

