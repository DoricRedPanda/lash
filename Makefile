program_name := lash

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99
linkerflags =

source_dir := src
include_dir := include
bin_dir := bin
install_dir := /usr/local/bin

source_files := $(wildcard $(addsuffix /*.c, $(source_dir) ) )
object_files := $(notdir $(source_files) )
object_files := $(object_files:.c=.o)
object_files := $(addprefix $(bin_dir)/, $(object_files))

VPATH := $(source_dir) $(include_dir) $(bin_dir)

all: $(program_name)

install: $(program_name)
	cp $(program_name) $(install_dir)/

$(program_name): $(object_files)
	$(CC) $^ ${linkerflags} -o $@

$(bin_dir)/%.o: %.c
	$(CC) -c $< $(CFLAGS) -I$(include_dir) -I$(source_dir) -o $@

clean:
	$(RM) $(bin_dir)/*.o $(program_name)

uninstall:
	$(RM) -f $(install_dir)/$(program_name)

.PHONY: clean install uninstall
