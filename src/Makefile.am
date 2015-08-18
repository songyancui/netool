#AUTOMAKE_OPTIONS=foreign 

bin_PROGRAMS=netool testAccept 

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
        client.c\
		channel.c\
		net.c\
		io.c\
        event.c\
        modes/mode.c\
        modules/modules.c\
		third/cJSON/cJSON.c
		

testAccept_SOURCES= test_accept.c \
                    log.c\
                    mm.c\
                    net.c\
                    io.c\
                    event.c
