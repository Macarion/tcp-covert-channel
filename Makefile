obj-m := send.o

all :
	make -C /lib/modules/5.4.51-v8+/build M=$(PWD) modules

clean:
	make -C /lib/modules/5.4.51-v8+/build M=$(PWD) clean
