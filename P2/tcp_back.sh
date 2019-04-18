gcc tcp_server.c sql/db.c -o tcp_server -std=c11 -pedantic -Wall -I/usr/include/mysql -lmysqlclient -D_POSIX_C_SOURCE=200809L
./tcp_server $1
