CC=g++

INSTALL_DIR = ../bin
INCLUDE_DIR = -I. -I.. -Iinclude  -I../../open-source-projects/cxxopts/include/


EXE      = logster
SOURCE   = src/main.cpp 
LINKLIBS = -lpthread 
OBJ_DIR  = ./obj

OBJS     = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCE:.cpp=.o)))
DEPS     = $(OBJS:.o-.d)

-include $(DEPS)

CFLAGSALL     = -std=c++23 -Wall -Wextra -Werror -Wshadow -march=native -fno-default-inline -fno-stack-protector -pthread -Wall -Werror -pedantic -Wextra -Waddress -Warray-bounds -Wno-builtin-macro-redefined -Wundef
CFLAGSRELEASE = -O2 -DNDEBUG $(CFLAGSALL)
CFLAGSDEBUG   = -ggdb3 -O0 -DDEBUG $(CFLAGSALL)
CSANITIZER    = $(CFLAGSDEBUG) -fsanitize=address -fno-omit-frame-pointer

.PHONY: release
release: CFLAGS   = $(CFLAGSRELEASE)
release: CXXFLAGS = $(CFLAGSRELEASE)
release: all

.PHONY: debug
debug: CFLAGS   = $(CFLAGSDEBUG)
debug: CXXFLAGS = $(CFLAGSRELEASE)
debug: all

.PHONY: sanitize
sanitize: CFLAGS      =  $(CSANITIZER)
sanitize: CXXFLAGS    = $(CFLAGSRELEASE)
sanitize: LINKEXE    := $(LINKEXE) -fsanitize=address
sanitize: all

$(shell mkdir -p $(OBJ_DIR))

all : $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(STATICLIBS) $(LINKLIBS)

$(OBJ_DIR)/%.o: src/%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -MMD -MP -MF $(@:.o=.d) -c $< -o $@

install : all
	install -d $(INSTALL_DIR)
	install -m 750 $(EXE) $(INSTALL_DIR)

uninstall :
	/bin/rm -rf $(INSTALL_DIR)

clean :
	rm -f src/*.o src/*.d $(EXE) 

cleanobj :
	rm -vf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d 
 
