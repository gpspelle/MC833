gcc client.c -o client -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L
declare -i i=1
./client $i
i=$(( $i + 1 ))
./client $i
i=$(( $i + 1 ))
./client $i
i=$(( $i + 1 ))
./client $i
i=$(( $i + 1 ))
./client $i
i=$(( $i + 1 ))
./client $i
