CFLAGS := -g -Wall

co_test: co_test.o co.a co_ctx.a
co_ctx_test: co_ctx.a co_ctx_test.o co_ctx.h

co.a: co.o co_ctx.a
	ar -crs co.a $^

co_ctx.a: co_make_ctx.o co_jump_ctx.o
	ar -crs co_ctx.a $^

clean:
	rm -f *.o
	rm -f co_ctx.a
	rm -f core.*
	rm -f co_ctx_test

.PHONY: clean
