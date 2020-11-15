obj-m := recv.o

ccflags-y += "-fno-stack-protector"

all :
	make -C /lib/modules/5.4.72-v8+/build M=$(PWD) modules

clean:
	make -C /lib/modules/5.4.72-v8+/build M=$(PWD) clean
