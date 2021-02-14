all: test01 test02 test03 test04 test05 test06  cancel-issue 

LIBS=-lpthread

test01: test01.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test02: test02.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test03: test03.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test04: test04.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test05: test05.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

test06: test06.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

cancel-issue: cancel-issue.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

.PHONY:
clean:
	rm test01 test02 test03 test04 test05 cancel-issue
