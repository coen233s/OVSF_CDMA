TESTS += \
	test/TestTransmit.cpp \
	test/TestBitQueue.cpp \
	test/TestOVSF.cpp \

SRCS += \
	ovsf.cpp \
	Configuration.cpp \
	dat/BitInQueue.cpp \
	dat/BitOutQueue.cpp \
	phy/Receiver.cpp \
	phy/RxTxBase.cpp \
	phy/SimplePhyChannel.cpp \
	phy/Transmitter.cpp \
	dev/DeviceBase.cpp \
	dev/BaseStation.cpp \
	dev/MobileStation.cpp \
	dev/protocol/ControlProtocol.cpp \

BINDIR := bin
INCLUDES += -I.

ifneq ($(DEBUG),)
CPPFLAGS += -g -O0
else
CPPFLAGS += -O3
endif

TARGETS := $(addprefix $(BINDIR)/,$(patsubst test/%.cpp,%,$(TESTS)))
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

$(BINDIR)/%:test/%.cpp $(OBJS)
	mkdir -p $(dir $@)
	g++ -o $@ $(INCLUDES) $(CPPFLAGS) $^

%.o: %.cpp
	g++ -o $@ $(INCLUDES) $(CPPFLAGS) -c $^

all: $(TARGETS)

clean:
	rm -rf $(TARGETS) $(OBJS) *.h~ *.cpp~
	rm -rf $(BINDIR)
