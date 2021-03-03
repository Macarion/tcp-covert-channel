obj-m :=
obj-m += send.o
# obj-m += recv.o

ccflags-y += "-fno-stack-protector"
ccflags-y += "--warn-no-unused-variable"

KVERSION := 5.4.72-v8+

send-objs := sendhook.o data.o handle.o time.o file.o functions.o device.o

all :
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
