#!/bin/bash 

echo "*******************start testing*************";

LD_RELAY="mm.o channel.o log.o test.o io.o net.o event.o  "

# $1：test_bin
# $2: src
# $3: LD_RELAY 

test_util()
{
    test_bin="$1"
    src="$2"
    
    # duplicatly compiling
    tmp_LD_RELAY=${LD_RELAY/${test_bin}.o/ }
    echo "*******************start testing ${test_bin} ***************";

    gcc ${src} -D TEST -g  -o ${test_bin} ${tmp_LD_RELAY} 
    ./${test_bin}
    echo "*******************complete testing ${test_bin} **************";
    rm ${test_bin}
}



test_util "master" "master.c"; 
test_util "channel" "channel.c"; 
test_util "net" "net.c" ;
rm test.sock;

test_util "event" "event.c" ;
