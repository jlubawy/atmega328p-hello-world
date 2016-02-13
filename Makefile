################################################################################
# Makefile
# Josh Lubawy <jlubawy@gmail.com>
################################################################################

RM = rm -rf
MKDIR = mkdir -p $@

MMCU ?= atmega328p
COM_PORT ?= COM10

DEFINES  = -DF_CPU=16000000L
DEFINES += -DARDUINO=10603
DEFINES += -DARDUINO_AVR_UNO
DEFINES += -DARDUINO_ARCH_AVR

ARDUINO_DIR = C:/PROGRA~2/Arduino
AVR_BIN_DIR = $(ARDUINO_DIR)/hardware/tools/avr/bin
AVR_INCLUDE_DIR = $(ARDUINO_DIR)/hardware/tools/avr/avr/include
AVR_ETC_DIR = $(ARDUINO_DIR)/hardware/tools/avr/etc

BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
DIRS = $(BIN_DIR) $(OBJ_DIR)

AVR_DUDE = $(AVR_BIN_DIR)/avrdude
AVR_OBJCOPY = $(AVR_BIN_DIR)/avr-objcopy
AVR_OBJDUMP = $(AVR_BIN_DIR)/avr-objdump
AVR_SIZE = $(AVR_BIN_DIR)/avr-size

AR = $(AVR_BIN_DIR)/avr-ar
CC = $(AVR_BIN_DIR)/avr-gcc
CXX = $(AVR_BIN_DIR)/avr-g++

INCLUDES = $(addprefix -I,$(AVR_INCLUDE_DIR))

COMMON_FLAGS = -g -Os -Wl,-Map,$@.map -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -lm -fno-exceptions -ffunction-sections -fdata-sections -MMD -mmcu=$(MMCU) $(DEFINES) $(INCLUDES)
CFLAGS = $(COMMON_FLAGS)
CXXFLAGS = $(COMMON_FLAGS)

C_RECIPE = $(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
CPP_RECIPE = $(CXX) $(CPPFLAGS) $(CXXFLAGS) -fno-threadsafe-statics -o $@ -c $<

################################################################################
# Application Recipes
################################################################################
APP_TARGET = HelloWorld
APP_TARGET_ELF = $(addprefix $(BIN_DIR)/,$(addsuffix .elf,$(APP_TARGET)))
APP_TARGET_EEP = $(addprefix $(BIN_DIR)/,$(addsuffix .eep,$(APP_TARGET)))
APP_TARGET_HEX = $(addprefix $(BIN_DIR)/,$(addsuffix .hex,$(APP_TARGET)))
APP_TARGETS = $(APP_TARGET_EEP) $(APP_TARGET_ELF) $(APP_TARGET_HEX)
APP_C_PREREQS = $(wildcard $(SRC_DIR)/*.c)
APP_CPP_PREREQS = $(wildcard $(SRC_DIR)/*.cpp)
APP_C_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(APP_C_PREREQS:.c=.c.o)))
APP_CPP_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(APP_CPP_PREREQS:.cpp=.cpp.o)))

################################################################################
# Targets
################################################################################
all: $(DIRS) $(APP_TARGETS)

clean:
	$(RM) $(DIRS)

install: all $(APP_TARGET_HEX)
	$(AVR_DUDE) -C$(AVR_ETC_DIR)/avrdude.conf -v -p$(MMCU) -carduino -P$(COM_PORT) -b115200 -D -Uflash:w:$(APP_TARGET_HEX):i

.PHONY: all clean install

$(DIRS):
	MKDIR $(DIRS)

# Object Files
$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	$(info $@)
	$(C_RECIPE)

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(info $@)
	$(CPP_RECIPE)

# Output Files
$(APP_TARGET_ELF): $(APP_C_OBJS) $(APP_CPP_OBJS)
	$(info $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^
	@$(AVR_SIZE) --mcu=$(MMCU) -C $@
	@$(AVR_SIZE) --mcu=$(MMCU) -C $@ > $(BIN_DIR)/memory_usage.txt
	@$(AVR_OBJDUMP) --disassemble --syms $@ > $@.lst
$(APP_TARGET_EEP) $(APP_TARGET_HEX): $(APP_TARGET_ELF)
	$(info $@)
	@$(AVR_OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $@
	@$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@
