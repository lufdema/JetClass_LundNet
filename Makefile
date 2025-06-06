# If you are using this Makefile standalone and fastjet-config is not
# in your path, edit this line to specify the full path
FASTJETCONFIG=/home/felipe/fastjet-install/bin/fastjet-config
PREFIX=`$(FASTJETCONFIG) --prefix`
CXX=g++
CXXFLAGS= -O3 -Wall `root-config --cflags --glibs --ldflags` -g -fopenmp 
install_script = $(SHELL) ../utils/install-sh
check_script = ../utils/check.sh

# global contrib-wide Makefile include may override some of the above
# variables (leading "-" means don't give an error if you can't find
# the file)
-include ../.Makefile.inc

#------------------------------------------------------------------------
# things that are specific to this contrib
NAME=LundPlane
SRCS=LundGenerator.cc LundWithSecondary.cc SecondaryLund.cc
EXAMPLES=example example_secondary example3 example2 example4 part_trans part_trans_multiple part_trans_to_root part_trans_to_root2 part_trans_to_root2_multiple
INSTALLED_HEADERS=LundGenerator.hh LundWithSecondary.hh SecondaryLund.hh LundJSON.hh
#------------------------------------------------------------------------

CXXFLAGS+= $(shell $(FASTJETCONFIG) --cxxflags) $(root-config --cxx --cflags)
LDFLAGS += -lm $(shell $(FASTJETCONFIG) --libs) `root-config --glibs` 

OBJS  = $(SRCS:.cc=.o)
EXAMPLES_SRCS  = $(EXAMPLES:=.cc)

install_HEADER  = $(install_script) -c -m 644
install_LIB     = $(install_script) -c -m 644
install_DIR     = $(install_script) -d
install_DATA    = $(install_script) -c -m 644
install_PROGRAM = $(install_script) -c -s
install_SCRIPT  = $(install_script) -c

.PHONY: clean distclean examples check install

# compilation of the code (default target)
all: lib$(NAME).a

lib$(NAME).a: $(OBJS) 
	ar cru lib$(NAME).a $(OBJS)
	ranlib lib$(NAME).a

# building the examples
examples: $(EXAMPLES)

# the following construct makes it possible to automatically build
# each of the examples listed in $EXAMPLES
$(EXAMPLES): % : %.o all
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -l$(NAME) $(LDFLAGS)

# check that everything went fine
check: examples
	@for prog in $(EXAMPLES); do\
	  $(check_script) $${prog} ../data/single-event.dat || exit 1; \
	done
	@echo "All tests successful"

# cleaning the directory
clean:
	rm -f *~ *.o

distclean: clean
	rm -f lib$(NAME).a $(EXAMPLES)

# install things in PREFIX/...
install: all
	$(install_DIR) $(PREFIX)/include/fastjet/contrib
	for header in $(INSTALLED_HEADERS); do\
	  $(install_HEADER) $$header $(PREFIX)/include/fastjet/contrib/;\
	done
	$(install_DIR) $(PREFIX)/lib
	$(install_LIB) lib$(NAME).a $(PREFIX)/lib

depend:
	makedepend -Y --   -- $(SRCS) $(EXAMPLES_SRCS)
# DO NOT DELETE

LundGenerator.o: LundGenerator.hh
LundWithSecondary.o: LundWithSecondary.hh LundGenerator.hh SecondaryLund.hh
SecondaryLund.o: SecondaryLund.hh LundGenerator.hh
example.o: LundGenerator.hh SecondaryLund.hh LundJSON.hh
example_secondary.o: LundWithSecondary.hh LundGenerator.hh SecondaryLund.hh
example2.o: LundWithSecondary.hh LundGenerator.hh SecondaryLund.hh
example4.o: LundWithSecondary.hh LundGenerator.hh SecondaryLund.hh
