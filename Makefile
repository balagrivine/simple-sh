READLINE_PREFIX = $(shell brew --prefix readline)
SDK             = $(shell xcrun --sdk macosx --show-sdk-path)

CC      = clang
CFLAGS  = -std=c99 -Wall -Wextra -isysroot $(SDK) -I$(READLINE_PREFIX)/include
LDFLAGS = -L$(READLINE_PREFIX)/lib -L$(SDK)/usr/lib -lreadline

TARGET  = shell
SRC     = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: clean
