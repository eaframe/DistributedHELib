CC =		g++
LDFLAGS = 	-g -O
MPICPP =	mpic++

#master
MASTER_FHE_LIB =	../HElib-master/src/fhe.a
MASTER_SRC =		../../ntl-8.1.2/local/include -I../HElib-master/src
MASTER_LIBS = 		-L/usr/local/lib/ -L../../ntl-8.1.2/local/lib/ -L../../gmp-6.0.0/local/lib/ -lntl -lgmp

#modified
MODIFIED_FHE_LIB = 	../HElib-modified/src/fhe.a
MODIFIED_SRC = 	../../ntl-8.1.2/local/include -I../HElib-modified/src
MODIFIED_LIBS = 	-L/usr/local/lib/ -L../../ntl-8.1.2/local/lib/ -L../../gmp-6.0.0/local/lib/ -lntl -lgmp

all:
	make master
	make modified

master:	Test_DistributedHE.cpp
	$(MPICPP) $(LDFLAGS) Test_DistributedHE.cpp $(MASTER_FHE_LIB) -I$(MASTER_SRC) -o Test_DistributedHE_HELib_master $(MASTER_LIBS)

modified:	Test_DistributedHE.cpp
	$(MPICPP) $(LDFLAGS) Test_DistributedHE.cpp $(MODIFIED_FHE_LIB) -I$(MODIFIED_SRC) -o Test_DistributedHE_HELib_modified $(MODIFIED_LIBS)

clean:
	rm Test_DistributedHE_HELib_master Test_DistributedHE_HELib_modified
