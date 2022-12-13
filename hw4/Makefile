OUTPUT=mymalloc
CFLAGS=-g -Wall -Wvla -Werror -Wno-unused-function -fsanitize=address -c
LFLAGS=

%: %.c %.h
	gcc $(CFLAGS) -o $@ $< $(LFLAGS)

%: %.c
	gcc $(CFLAGS) -o $@ $< $(LFLAGS)

all: $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT)