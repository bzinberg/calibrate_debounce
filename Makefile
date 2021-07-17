ARDMK_DIR    = Arduino-Makefile
BOARD_TAG    = uno
ARDUINO_LIBS = Wire

# Digital pin that is connected to the switch
PIN_SWITCH = 2

include $(ARDMK_DIR)/Arduino.mk

CXXFLAGS += -DPIN_SWITCH=${PIN_SWITCH}
