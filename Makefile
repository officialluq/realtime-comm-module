obj-m += cproc.o

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install