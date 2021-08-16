TARGET=alarm-clock-lamp

CC=arm-none-eabi-g++
OBJCPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
STFLASH=../stlink/bin/st-flash

CC_FLAGS=-mthumb -mcpu=cortex-m3 -Iinc -Ilib -Ilib/etl/include -DSTM32F103xB -DETL_NO_STL -fstack-usage -Os -g3
LD_FLAGS=-mcpu=cortex-m3 -nostdlib -TSTM32F103XB_FLASH.ld -DSTM32F103xB -Wl,--print-memory-usage\
 		 -Wl,-Map=obj/$(TARGET).map,--gc-sections

CC_FLAGS+=-Wall -Wextra -Wshadow -Wstack-usage=255 -Wconversion
CC_FLAGS+=-fno-exceptions -fno-common -fno-non-call-exceptions -fno-rtti -ffreestanding -ffunction-sections\
		  -fdata-sections -finline-small-functions -findirect-inlining -std=c++17


STARTUP=lib/startup_stm32f103xb.s

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(TARGET).bin

download: $(TARGET).bin
	$(STFLASH) --format binary --flash=0x10000 write $(TARGET).bin 0x8000000

$(TARGET).bin: $(TARGET).elf
	$(OBJCPY) -O binary $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJS)
	$(CC) $(LD_FLAGS) $(STARTUP) $(OBJS) -o $@

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	del /F /Q obj\* *.elf *.bin

disassemble: $(OBJS)
	$(OBJDUMP) -S -d $< > $<.dump