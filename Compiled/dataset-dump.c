#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mysql/mysql.h>
#include <sys/stat.h>
#include "mydatabase.h"

#define DATAFILE "/var/www.gridwatch.templar.co.uk/downloads/grid.csv"


// get the whole dataset and parse into CSV style and dump in the working directory
/*
+------------+------------------+------+-----+---------+----------------+
| Field      | Type             | Null | Key | Default | Extra          |
+------------+------------------+------+-----+---------+----------------+
| id         | int(10) unsigned | NO   | PRI | NULL    | auto_increment |
| timestamp  | datetime         | YES  |     | NULL    |                |
| demand     | double           | YES  |     | NULL    |                |
| frequency  | double           | YES  |     | NULL    |                |
| coal       | double           | YES  |     | NULL    |                |
| nuclear    | double           | YES  |     | NULL    |                |
| ccgt       | double           | YES  |     | NULL    |                |
| ocgt       | double           | YES  |     | NULL    |                |
| french_ict | double           | YES  |     | NULL    |                |
| irish_ict  | double           | YES  |     | NULL    |                |
| dutch_ict  | double           | YES  |     | NULL    |                |
| pumped     | double           | YES  |     | NULL    |                |
| hydro      | double           | YES  |     | NULL    |                |
| wind       | double           | YES  |     | NULL    |                |
| oil        | double           | YES  |     | NULL    |                |
| other      | double           | YES  |     | NULL    |                |
| ew_ict     | double           | YES  |     | NULL    |                |
+------------+------------------+------+-----+---------+----------------+
*/
int main()
	{
	MYSQL mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int i,j,rows;
	char buffer[4096];
	FILE *fp;
	if(!mysql_init(&mysql)) // initalise data structure
		{
		return (1);
		}
	if(!mysql_real_connect(&mysql,"127.0.0.1",USERNAME,PASSWORD,DATABASE,0,"",0)) // connect to database
		{
		printf("Connect failed -%s\n",mysql_error(&mysql));
		mysql_close(&mysql);
		return 2;
		}	
	sprintf(buffer,"select id,timestamp, demand, frequency, coal, nuclear, ccgt, ocgt, french_ict,irish_ict,dutch_ict, ew_ict, pumped, hydro,wind,oil,other from day order by id");
	if(mysql_query(&mysql,buffer))
		{
		mysql_close(&mysql);
		return 3;
		}
	result=mysql_store_result(&mysql);
	rows=mysql_num_rows(result);
    // have the data
	if(!(fp=fopen(DATAFILE, "w")))
		return 4;
	fprintf(fp,"id,timestamp,demand,frequency,coal,nuclear,ccgt,ocgt,french_ict,irish_ict,dutch_ict,ew_ict, pumped,hydro,wind,oil, other\r\n");
	for(i=0;i<rows;i++)
		{
		row=mysql_fetch_row(result);
		// got the row so time to print it.
		sprintf(buffer,",");
		for (j=0;j<17;j++)
			{
			if(j==16) 
				sprintf(buffer,"\r\n"); // terminate the line 
			fprintf(fp,"%s%s",row[j],buffer);
			}
		}
	fclose(fp);
	}
