obj-m := covert.o

ccflags-y += "-fno-stack-protector"
ccflags-y += "--warn-no-declaration-after-statement"
# ccflags-y += "--warn-no-unused-variable"

KVERSION := 5.4.72-v8+

covert-objs := send.o data.o handle.o device.o time.o file.o functions.o command.o list.o queue.o

all :
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
