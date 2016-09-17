# canvas - a 2d/3d graphics engine 
include config.mk
#include tests.mk

NAME = canvas
SRC = canvas.c lite.c main.c 
OBJ = ${SRC:.c=.o}
#UPDATE = lineslow.c
#GAMENAME = sm 
DBGCHK = gdb
DBGFLAGS =  
LEAKCHK = valgrind
LKFLAGS = --leak-check=full 
IGNORE = $(NAME) archive/* bin/* tools/* rsrch/* wk/* *.out
ARCHIVEIGNORE = $(IGNORE)
ARCHIVEFMT = gz
ARCHIVEFILE = $(NAME).`date +%F`.`date +%H.%M.%S`.tar.${ARCHIVEFMT}
PKGIGNORE = $(IGNORE)
PKGFMT = gz
PKGFILE = $(NAME).`date +%F`.`date +%H.%M.%S`.tar.${ARCHIVEFMT}

test-$(NAME): ${OBJ}
	@echo CC -o $@ ${OBJ} ${CFLAGS}
	@${CC} -o $@ ${OBJ} ${CFLAGS}

depd:
	@test ! -z ${DIR} || printf "Target 'depd' depends on DIR value from command-line or config.mk.\n" > /dev/stderr
	@test ! -z ${DIR}
	@test ! -h ${DIR}/$(NAME).c || printf "$(NAME).c is already in the source directory ${DIR}!\n" > /dev/stderr
	@test ! -h ${DIR}/$(NAME).c
	@echo ln -s `realpath ./$(NAME).c` ${DIR}/$(NAME).c
	@ln -s `realpath ./$(NAME).c` ${DIR}/$(NAME).c
	@echo ln -s `realpath ./$(NAME).h` ${DIR}/$(NAME).h
	@ln -s `realpath ./$(NAME).h` ${DIR}/$(NAME).h

libr:
	-test -f canvas.o && rm -f canvas.o
	@echo CC -o canvas.o ${DSOFLAGS} ${CFLAGS}
	@${CC} $^ -o lib/lib$(NAME).$(DSOEXT) ${DSOFLAGS} ${CFLAGS}
	@echo CC $^ -o lib/lib$(NAME).$(DSOEXT) ${DSOFLAGS} ${CFLAGS}
	@${CC} $^ -o lib/lib$(NAME).$(DSOEXT) ${DSOFLAGS} ${CFLAGS}
	
.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

clean:
	@echo Cleaning
	@rm -f *.o *.so *.dll $(NAME) test-$(NAME)

veryclean: clean
veryclean:
	@echo Cleaning up version control files...
	-find -type f -name "*.swo" | xargs rm
	-find -type f -name "*.swp" | xargs rm
	-find -type f -name "*.swn" | xargs rm
	-find -type f -name "*.swl" | xargs rm

leak: test-$(NAME) 
leak:
	@echo $(LEAKCHK) $(LKFLAGS) ./$(GAMENAME)
	$(LEAKCHK) $(LKFLAGS) ./$(GAMENAME)

dist: clean

# Target for testing little bits that I think may or may not work.
learn:
	@test ! -z ${NAME} || echo "Please define the environment variable 'NAME' to use this target."
	@test ! -z ${NAME}
	@echo CC $
	$(CC) ${CFLAGS} ${NAME}.c

# Add a check in the future to tell whether or not we're on Windows...
# Also figure out how to glob excluded files and directories together to get rid of multiple invocations of grep...
permissions:
	@find | grep -v './tools' | grep -v './examples' | grep -v './.git' | sed '1d' | xargs stat -c 'chmod %a %n' > PERMISSIONS

restore-permissions:
	chmod 744 PERMISSIONS
	./PERMISSIONS
	chmod 644 PERMISSIONS

backup:
	@echo tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(ARCHIVEIGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*
	@tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(ARCHIVEIGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*

archive: ARCHIVEDIR = archive
archive: clean 
archive: backup

.PHONY: all options clean dist install uninstall permissions archive
