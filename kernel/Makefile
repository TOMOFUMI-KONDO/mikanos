OBJS = main.o pixel_writer.o

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17
LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static

.PHONY: all
all: kernel.elf

kernel.elf: $(OBJS)
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS)

%.o: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $<

.PHONY: clean
clean:
	rm -f kernel.elf $(OBJS)
