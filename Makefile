# Makefile for RemoteShell
# by Wenhai Pan in CRSC
# 2013-05-22

CC = g++
TARGET = RemoteShell 
LIB_ARGS =

OBJS = main.o \
	   CTcpSocket.o \
	   CRemoteCmdTransmitter.o \
	   CCmdShell.o \
	   CConfigFileParser.o

OBJS_C = 

all: $(TARGET)


$(TARGET): $(OBJS) $(OBJS_C)
	$(CC) -o $@ $(OBJS) $(OBJS_C) $(LIB_ARGS)


$(OBJS):%.o:%.cpp *.h
	$(CC) -c $< -o $@

$(OBJS_C):%.o:%.c *.h
	gcc -c $< -o $@

.PHONY: clean

clean:
	-rm -f $(TARGET) *.o


