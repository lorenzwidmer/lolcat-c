IDIR =./include
CC=g++
CFLAGS=-I$(IDIR)

ODIR=./
LDIR =./lib

LIBS=-lm

_DEPS = anisescape/**.cc anisescape.cc
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = include/*.cc include/ansiescape/*.cc
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

lolcat: $(OBJ)
	$(CC) -g $@.c $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

