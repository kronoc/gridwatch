all: graphmaker nationalgrid-mysql nationalgrid dataset-dump get-windfarms-data-sql

install:
	cp graphmaker /usr/local/bin
	cp nationalgrid-mysql /usr/local/bin
	cp nationalgrid /usr/local/bin
	cp dataset-dump /usr/local/bin
	cp get-windfarms-data-sql /usr/local/bin
	cp nationalgrid.sh /usr/local/scripts

graphmaker: graphmaker.cc mydatabase.h
	g++ graphmaker.cc -o graphmaker `freetype-config --cflags` -I/usr/local/include  -L/usr/local/lib -lpng -lpngwriter -lz -lfreetype -lmysqlclient

nationalgrid-mysql:  nationalgrid-mysql.c mydatabase.h
	cc  nationalgrid-mysql.c -o nationalgrid-mysql -lcurl -lmysqlclient

get-windfarms-data-sql:  get-windfarms-data-sql.c mydatabase.h
	cc  get-windfarms-data-sql.c -o get-windfarms-data-sql -lcurl -lmysqlclient

nationalgrid: nationalgrid.c mydatabase.h
	cc  nationalgrid.c -o nationalgrid -lcurl

dataset-dump: dataset-dump.c mydatabase.h
	cc  dataset-dump.c -o dataset-dump -lmysqlclient

