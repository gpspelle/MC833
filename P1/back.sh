gcc server.c sql/db.c -o server -std=c99 -pedantic -Wall -I/usr/include/mysql -lmysqlclient 
./server
