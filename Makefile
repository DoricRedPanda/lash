program_name := lash

CC = gcc
CFLAGS = -Wall -pedantic -std=c99
linkerflags =

source_dir := src
include_dir := include
bin_dir := bin

source_files := $(wildcard $(addsuffix /*.c, $(source_dir) ) )
object_files := $(notdir $(source_files) )
object_files := $(object_files:.c=.o)
object_files := $(addprefix $(bin_dir)/, $(object_files))

VPATH := $(source_dir) $(include_dir) $(bin_dir)

all: $(program_name)

$(program_name): $(object_files)
	$(CC) $^ ${linkerflags} -o $@

$(bin_dir)/%.o: %.c
	$(CC) -c $< $(CFLAGS) -I$(include_dir) -I$(source_dir) -o $@

clean:
	$(RM) $(bin_dir)/*.o $(program_name)

.PHONY: clean
