program_name := lash

CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g

source_dir := src
include_dir := include

source_files := $(wildcard $(addsuffix /*.c, $(source_dir) ) )
object_files := $(notdir $(source_files) )
object_files := $(object_files:.c=.o)

VPATH := $(source_dir) $(include_dir)

$(program_name): $(object_files)
	$(CC) $^ -o $@


%.o: %.c
	$(CC) $< -c $(CFLAGS) -I$(include_dir) -I$(source_dir)

clean:
	$(RM) *.o $(program_name)
