EXTRALIBS   += `root-config --glibs`
all: decode show_wave pickup objclean

decode: decode_without_daq.o EventDict.o Event.o DataDef.o
	@echo "building the $@..."
	@g++ -o $@ $^ $(EXTRALIBS)

show_wave: EventDict.o Event.o show_wave.o
	@echo "building the $@..."
	@g++ -o $@ $^ $(EXTRALIBS)

pickup: PickUp.o HighPassFilter.o LowPassFilter.o DataProcessing.o EventDict.o Event.o Smooth.o 
	@echo "building the $@..."
	@g++ -o $@ $^ $(EXTRALIBS)

EventDict.cpp : Event.h EventLinkDef.h
	@rootcint -f EventDict.cpp -c Event.h EventLinkDef.h

%.o : %.cpp
	@g++ -o $@ -c $^ `root-config --cflags`

objclean:
	@rm -rf *.o
clean :
	@echo "Cleaning..."
	@rm -f decode show_wave pickup EventDict.*
dataclean:
	@echo "Cleaning ROOT files..."
	@rm -f *.root
