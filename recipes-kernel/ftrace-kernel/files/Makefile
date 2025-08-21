obj-m := gpio-ftrace-demo.o

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

install: modules_install

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install