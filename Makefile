GCC_COMPILER = gcc
GCC_COMPILER_FLAGS = -ggdb -Wall

GTK_INCS = `pkg-config gtk+-2.0 --cflags`
GTK_LIBS = `pkg-config gtk+-2.0 --libs`
CURL_LIBS = `curl-config --libs`

MISC_LIBS = -lm -lrt -lpthread

INCS = $(GTK_INCS)
LIBS = $(GTK_LIBS) $(CURL_LIBS) $(MISC_LIBS)

TARGET = gtkPview

ifeq ($(porn), 1)
	GCC_COMPILER_FLAGS += -DPORN
endif

SRC_DIRS := .
SRC_FILES := $(foreach DIR, $(SRC_DIRS), $(wildcard $(DIR)/*.c))
OBJS := $(patsubst %.c, %.o, $(SRC_FILES))

all : $(TARGET)
	@echo All done

$(TARGET) : $(OBJS)
	$(GCC_COMPILER) $(GCC_COMPILER_FLAGS) -o $@ $^ $(LIBS)

%.o : %.c
	$(GCC_COMPILER) $(GCC_COMPILER_FLAGS) -o $@ -c $(INCS) $<

clean :
	rm -f $(OBJS) $(TARGET)
	@echo Clean done

.PHONY: clean


# $^ test.o / test.cu
# $@ test
# $< test.o / test.cu
