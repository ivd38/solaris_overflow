Solaris Kernel stack overflow
Tested on 11.4

Vulnerability details (using illumos source)
```
#define STACK_BUF_SIZE  64

int
prusrio(proc_t *p, enum uio_rw rw, struct uio *uiop, int old)
{
        /* longlong-aligned short buffer */
        longlong_t buffer[STACK_BUF_SIZE / sizeof (longlong_t)];
        int error = 0;
        void *bp;
        int allocated;
        ssize_t total = uiop->uio_resid;
        uintptr_t addr;
        size_t len;

        /* for short reads/writes, use the on-stack buffer */
[1]        if (uiop->uio_resid <= STACK_BUF_SIZE) {
                bp = buffer;
                allocated = 0;
        } else {
                bp = kmem_alloc(PAGESIZE, KM_SLEEP);
                allocated = 1;
        }
		...
		switch (rw) {
			...
			case UIO_WRITE:
                while (uiop->uio_resid != 0) {
                        addr = uiop->uio_offset;
[2]                        len = MIN(uiop->uio_resid,
                            PAGESIZE - (addr & PAGEOFFSET));

[3]                        if ((error = uiomove(bp, len, UIO_WRITE, uiop)) != 0)
                                break;
                        if ((error = uwrite(p, bp, len, addr)) != 0) {
                                uiop->uio_resid += len;
                                uiop->uio_loffset -= len;
                                break;
                        }
                }
```
uio_resid is signed integer so check on line #1 can be bypassed when uiop->uio_resid < 0.
It will lead to stack overflow on line #3.


The bug is easily exploitable.
