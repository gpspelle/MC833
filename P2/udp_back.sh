gcc udp_server.c sql/db.c -o udp_server -std=c11 -pedantic -Wall -I/usr/include/mysql -lmysqlclient -D_POSIX_C_SOURCE=200809L
./udp_server $1
