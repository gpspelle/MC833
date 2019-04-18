gcc server.c sql/db.c -o server -std=c11 -pedantic -Wall -I/usr/include/mysql -lmysqlclient -D_POSIX_C_SOURCE=200809L
gcc client.c -o client -std=c11 -pedantic -Wall -D_POSIX_C_SOURCE=200809L
./client $1
#declare -i i=1
#./server $i
#./client $i
#i=$(( $i + 1 ))
#./server $i
#./client $i
#i=$(( $i + 1 ))
#./server $i
#./client $i
#i=$(( $i + 1 ))
#./server $i
#./client $i
#i=$(( $i + 1 ))
#./server $i
#./client $i
#i=$(( $i + 1 ))
#./server $i
#./client $i
