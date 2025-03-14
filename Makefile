CFLAGS := -std=c99 -g -Wall

all: co_ctx_test co_test echosvr

co_ctx_test: co_ctx.a co_ctx_test.o co_ctx.h
co_test: co.a co_ctx.a co_test.o co.h
echosvr: co_io.a co.a co_ctx.a echosvr.o

co_io.a: co_io.o co.a
	ar -crs co_io.a $^

co.a: co.o co_ctx.a
	ar -crs co.a $^

co_ctx.a: co_make_ctx.o co_jump_ctx.o
	ar -crs co_ctx.a $^

test: co_ctx_test co_test
	./co_ctx_test
	./co_test

clean:
	rm -f *.o
	rm -f *.a
	rm -f core.*
	rm -f *_test
	rm -f echosvr

.PHONY: clean
