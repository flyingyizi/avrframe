# -std=c99, -std=gnu99, -std=c11 or -std=gnu11 to compile your code


#最终bin文件的名字，可以更改为自己需要的
BIN=myapp
MCU     ?= atmega328p
F_CPU   = 16000000
# Output format. (can be srec, ihex, binary)
FORMAT = ihex

#设置编译器选项
#### 1. 设置使用的编译器选项
OPTIMIZE      = -O2
CFLAGS        = -g -Wall $(OPTIMIZE) $(DEFS)

# ALL_CFLAGS 		= -mmcu=$(MCU)  -D F_CPU=$(F_CPU)  -I. $(CFLAGS) 
# ALL_CXXFLAGS 	= -mmcu=$(MCU)  -D F_CPU=$(F_CPU)  -I. $(CFLAGS) -std=c++98 


ALL_CFLAGS 		= -mmcu=$(MCU) -D _DEBUG  -D F_CPU=$(F_CPU) -I. $(CFLAGS)
ALL_CXXFLAGS 	= -mmcu=$(MCU) -D _DEBUG  -D F_CPU=$(F_CPU) -I. $(CFLAGS) -std=c++98
ALL_ASFLAGS 	= -mmcu=$(MCU) -D _DEBUG  -D F_CPU=$(F_CPU) -I. -x assembler-with-cpp $(ASFLAGS)


#设置工具
#### 1.设置编译器路径 
DIRAVR = C:/prog/Arduino/hardware/tools/avr
DIRAVRBIN = $(DIRAVR)/bin
CC     = $(DIRAVRBIN)/avr-gcc.exe
OBJCOPY        = $(DIRAVRBIN)/avr-objcopy.exe
OBJDUMP        = $(DIRAVRBIN)/avr-objdump.exe
AVRDUDE        = $(DIRAVRBIN)/avrdude.exe -C $(DIRAVR)/etc/avrdude.conf
# Convert ELF to COFF for use in debugging / simulating in
# AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
	--change-section-address .data-0x800000 \
	--change-section-address .bss-0x800000 \
	--change-section-address .noinit-0x800000 \
	--change-section-address .eeprom-0x810000 


#debug文件夹里的makefile文件需要最后执行，所以这里需要执行的子目录要排除debug文件夹，这里使用awk排除了Debug文件夹，读取剩下的文件夹
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "Debug" ) print $$9}')
#无需下一行的注释代码，因为我们已经知道debug里的makefile是最后执行的，所以最后直接去debug目录下执行指定的makefile文件就行，具体下面有注释
#DEBUG=$(shell ls -l | grep ^d | awk '{if($$9 == "Debug") print $$9}')
#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)
#目标文件所在的目录
OBJS_DIR=Debug/obj
#bin文件所在的目录
BIN_DIR=Debug/bin
#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_CSOURCE=${wildcard *.c}
CUR_CPPSOURCE=${wildcard *.cpp}

CUR_COBJS := $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(CUR_CSOURCE)))))
CUR_CPPOBJS := $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(CUR_CPPSOURCE)))))
DEPENDS := $(addsuffix .d,$(CUR_COBJS))

ifneq (v$(V),v1)
hide?=@
else
hide?=
endif

#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC ALL_CFLAGS ALL_CXXFLAGS  ALL_ASFLAGS LDFLAGS  BIN OBJS_DIR BIN_DIR ROOT_DIR  \
       hide  COFFCONVERT \  OBJCOPY \ FORMAT

#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:CHECKDIR  $(SUBDIRS) $(CUR_COBJS) $(CUR_CPPOBJS) DEBUG

CHECKDIR: $(OBJS_DIR)  $(BIN_DIR)
$(OBJS_DIR)  $(BIN_DIR): ; mkdir -p $@

#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
#直接去debug目录下执行makefile文件
	make -C debug
ECHO:
	@echo $(SUBDIRS)


define make-cmd-cc
$2 : $1
	$$(info CC $$<)
	$$(hide) $$(CC) $$(ALL_CFLAGS)  -Wa,-adhlns=$$(<:.c=.lst) -MMD -MT $$@ -MF $$@.d -c -o $$@ $$<   
endef
 
$(foreach afile,$(CUR_CSOURCE),\
    $(eval $(call make-cmd-cc,$(afile),\
        $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(afile))))))))

define make-cmd-cc-cpp
$2 : $1
	$$(info CC $$<)
	$$(hide) $$(CC) $$(ALL_CXXFLAGS)  -Wa,-adhlns=$$(<:.cpp=.lst) -MMD -MT $$@ -MF $$@.d -c -o $$@ $$<   
endef
 
$(foreach afile,$(CUR_CPPSOURCE),\
    $(eval $(call make-cmd-cc-cpp,$(afile),\
        $(addsuffix .o,$(addprefix $(ROOT_DIR)/$(OBJS_DIR)/,$(basename $(notdir $(afile))))))))

clean:
	rm -rf $(OBJS_DIR)/*.o
	rm -rf $(OBJS_DIR)/*.o.d
	rm -rf $(OBJS_DIR)/*.lst
	rm -rf $(BIN_DIR)/*
	rm -rf *.lst   

-include $(DEPENDS)

# #############################################################

# #---------------- 编程下载选项 (avrdude) ----------------
# # Programming hardware: alf avr910 avrisp bascom bsd
# # dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
# #
# # Type: avrdude -c ?
# # to get a full listing.
# #
# AVRDUDE_PROGRAMMER = usbtiny
# # com1 = serial port. Use lpt1 to connect to parallel port.
# #AVRDUDE_PORT = com1 # programmer connected to serial device
# AVRDUDE_PORT = usb # programmer connected to serial device
# AVRDUDE_WRITE_FLASH = -U flash:w:$(PRG).hex
# #AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
# # Uncomment the following if you want avrdude's erase cycle counter.
# # Note that this counter needs to be initialized first using -Yn,
# # see avrdude manual.
# # AVRDUDE_ERASE_COUNTER = -y
# # Uncomment the following if you do /not/ wish a verification to be
# # performed after programming the device.
# # AVRDUDE_NO_VERIFY = -V
# # Increase verbosity level. Please use this when submitting bug
# # reports about avrdude. See <[url]http://savannah.nongnu.org/projects/avrdude>[/url]
# # to submit bug reports.
# # AVRDUDE_VERBOSE = -v -v
# AVRDUDE_FLAGS = -p $(MCU_TARGET) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
# AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
# AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
# AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

# #下载程序到目标
# program: $(PRG).hex 
# 	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


