CC =		g++
LDFLAGS = 	-g -O
MPICPP =	mpic++

#master
MASTER_FHE_LIB =	/home/ethan/Thesis/DistributedHELib/HElib-master/src/fhe.a
MASTER_SRC =		/home/ethan/Thesis/DistributedHELib/HElib-master/src
MASTER_LIBS = 		-L/usr/local/lib/ -lntl -lgmp	

#modified
MODIFIED_FHE_LIB = 	/home/ethan/Thesis/DistributedHELib/HElib-modified/src/fhe.a
MODIFIED_SRC = 	/home/ethan/Thesis/DistributedHELib/HElib-modified/src
MODIFIED_LIBS = 	-L/usr/local/lib/ -lntl -lgmp

all:
	make master
	make modified

master:	Test_DistributedHELib.cpp
	$(CC) $(LDFLAGS) Test_DistributedHELib.cpp $(MASTER_FHE_LIB) -I$(MASTER_SRC) -o Test_DistributedHELib_master $(MASTER_LIBS)

modified:	Test_DistributedHELib.cpp
	$(MPICPP) $(LDFLAGS) Test_DistributedHELib.cpp $(MODIFIED_FHE_LIB) -I$(MODIFIED_SRC) -o Test_DistributedHELib_modified $(MODIFIED_LIBS)

clean:
	rm Test_DistributedHELib_master Test_DistributedHELib_modified
