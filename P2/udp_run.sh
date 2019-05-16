gcc udp_client.c -o udp_client -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L
START=1
END=$1
echo $END
for ((c=$START; c <= $END; c++))
do
    ./udp_client $c 7 g172358@dac.unicamp.br &
    sleep .5
done
./udp_client $(($END + 1)) 8 g172358@dac.unicamp.br &
