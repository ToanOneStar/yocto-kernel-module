obj-m := gpio_blink.o

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

# Thêm mục tiêu modules_install
install: modules_install

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install