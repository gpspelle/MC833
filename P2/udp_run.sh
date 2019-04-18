gcc udp_client.c -o udp_client -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L
./udp_client $1
