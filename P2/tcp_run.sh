gcc tcp_client.c -o tcp_client -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L
START=0
END=$1
for ((c=$START; c <= $END; c++))
do
    ./tcp_client $c 7 g172358@dac.unicamp.br &
    sleep .5
done
#./tcp_client $(($END + 1)) 8 g172358@dac.unicamp.br &
