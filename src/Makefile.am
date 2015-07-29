#AUTOMAKE_OPTIONS=foreign 

bin_PROGRAMS=netool test

netool_LDADD= -lm
netool_SOURCES=main.c \
		log.c\
		test.c\
        mm.c\
		adlist.c\
		dict.c\
		ntconfig.c\
		master.c\
        worker.c\
		channel.c\
		net.c\
		io.c\
        event.c\
        modes/mode.c\
		third/cJSON/cJSON.c
		

test_SOURCES= test_main.c 
