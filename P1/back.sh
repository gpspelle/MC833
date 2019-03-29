gcc server.c sql/db.c -o server -std=gnu99 -pedantic -Wall -I/usr/include/mysql -lmysqlclient -D_POSIX_C_SOURCE=200809L
./server
