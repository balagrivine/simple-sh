READLINE_PREFIX = $(shell brew --prefix readline)
SDK             = $(shell xcrun --sdk macosx --show-sdk-path)

CC      = clang
CFLAGS  = -std=c99 -Wall -Wextra -isysroot $(SDK) -I$(READLINE_PREFIX)/include -Isrc
LDFLAGS = -L$(READLINE_PREFIX)/lib -L$(SDK)/usr/lib -lreadline

TARGET  = shell
SRC     = src/main.c src/builtin.c src/completion.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $@

clean:
	rm -f $(TARGET)

.PHONY: clean
