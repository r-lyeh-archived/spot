all: pvrtctest

pvrtctest: main.cpp PvrTcPacket.cpp MortonTable.cpp PvrTcEncoder.cpp PvrTcDecoder.cpp BitScale.cpp PvrTcPacket.h MortonTable.h PvrTcEncoder.h PvrTcDecoder.h BitScale.h
	g++ -std=c++11 -o pvrtctest main.cpp PvrTcPacket.cpp MortonTable.cpp PvrTcEncoder.cpp PvrTcDecoder.cpp BitScale.cpp
