all: test01 test02 atomic

LIBS=-lpthread

test01: test01.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test02: test02.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

atomic:atomic.c
	${CC} ${CFLAGS} -std=c11 -o $@ $< 

.PHONY:
clean:
	rm test01 test02 atomic
