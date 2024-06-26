CFLAGS = -std=c17 -g -O0

all: iconv_test

test: iconv_test
	./iconv_test
	./iconv_test ISO-8859-15 ANSI_X3.4-1968 || true
	LC_CTYPE='POSIX' ./iconv_test 
