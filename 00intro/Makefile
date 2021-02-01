ALL: thread process shared_memory posix_sm

thread: thread.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS} -lpthread

process: process.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

shared_memory: shared_memory.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

posix_sm: posix_sm.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS} -lrt

.PHONY:
clean:
	rm -f thread process shared_memory posix_sm
