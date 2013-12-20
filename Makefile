SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

EXEC = test # Programme principal
MAIN_OBJECTS = test.o icmp.o ip.o 

TRACEROUTE_EXEC = traceroute
TRACEROUTE_OBJECTS = traceroute.o icmp.o ip.o utils.o probe.o

PING_EXEC = ping
PING_OBJECTS = ping.o icmp.o ip.o utils.o probe.o



#TEST_EXEC = tests # Exécutable pour les tests unitaires
#TEST_OBJECTS = tests.o base.o couleur.o arbq.o


CC=gcc
LDLIBS = 
CFLAGS = -g -std=gnu99 -pedantic -Wall -Wextra -pedantic -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual -I$(INCLUDE_DIR)


$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

all: dirs $(EXEC) $(TRACEROUTE_EXEC) $(PING_EXEC) information

information:
	@echo Compilation terminée
#	@echo Pour lancer les tests unitaires: bin/$(TEST_EXEC)
	@echo Pour lancer le programme de démonstration: bin/$(EXEC)

dirs:
	mkdir -p bin obj

$(EXEC): $(addprefix $(OBJ_DIR)/, $(MAIN_OBJECTS))
	$(CC) -o $(BIN_DIR)/$@ $^ $(LDLIBS)

$(TRACEROUTE_EXEC): $(addprefix $(OBJ_DIR)/, $(TRACEROUTE_OBJECTS))
	$(CC) -o $(BIN_DIR)/$@ $^ $(LDLIBS)

$(PING_EXEC): $(addprefix $(OBJ_DIR)/, $(PING_OBJECTS))
	$(CC) -o $(BIN_DIR)/$@ $^ $(LDLIBS)



.PHONY: doc clean information

doc:
	doxygen Doxyfile

clean :
	$(RM) -rf $(OBJ_DIR)/ $(BIN_DIR) doc

archive:
	tar cvzf clement_schreiner.tar.gz include src Makefile Doxyfile rapport.pdf
