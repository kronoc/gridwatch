/*<p class="panel-column1">Wind Project</p>
<p class="panel-column2">Region</p>
<p class="panel-column3">Location</p>
<p class="panel-column4">Turbines</p>
<p class="panel-column5">Project Capacity (MW)</p>
<p class="panel-column6">Turbine Capacity (MW)</p>
<p class="panel-column7">Developer</p>
<p class="panel-column8">Status Change Date</p>
<p class="panel-column9">Status of Project</p>
<p class="panel-column10">Type of Project</p>
..

<p class="panel-column1 panel-content">
Achairn Farm, Stirkoke
</p>
<p class="panel-column2">Scotland</p>
<p class="panel-column3">Wick</p>
<p class="panel-column4 style-turbines">3</p>
<p class="panel-column5 style-mw">2.05</p>
<p class="panel-column6 style-power">6.15</p>
<p class="panel-column7">Iinnes (James & Ronald)</p>
<p class="panel-column8">01-May-2009</p>
<p class="panel-column9">Operational</p>
<p class="panel-column10">onshore</p>
*/
/************************************************************************************
/* program to scrape renewableUK wind farm tables and build a database of the farms *
*************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <mysql/mysql.h>
#include "mydatabase.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};
struct MemoryStruct chunk;

#define DEBUG

#define WINDFARM_URL "http://www.renewableuk.com/en/renewable-energy/wind-energy/uk-wind-energy-database/index.cfm"
#define NUMBER_OF_FIELDS 10
#define OPERATIONAL 0
#define UNDER_CONSTRUCTION 1
#define CONSENTED 2
#define TABLE "windfarms"
#define ONSHORE 1
#define OFFSHORE 0
#define METERED 1

struct windfarm
	{
	char *region;
	char *name;
	char *location;
	int number_of_turbines;
	float turbine_capacity_mw;
	float farm_capacity_mw;
	char *operator;
	char *start_date;
	int status;
	int type;
	int metered;
	struct windfarm *next;
	};

struct windfarm *first_windfarm=NULL; // list head pointer

// add an entry to the head of the linked list order is of no object with SQL databses anyway.
int add_entry(char *name,char *region, char *location, int number_of_turbines, 
				float turbine_capacity_mw, float farm_capacity_mw, char *operator, 
				char *start_date, char *status, char *type)
	{
	struct windfarm *new;
	struct tm timbuf;
	char mbuf[100];
	strptime(start_date,"%d-%b-%Y", &timbuf);
	strftime(mbuf, sizeof(mbuf), "%Y-%m-%d", &timbuf);
	new=malloc(sizeof(struct windfarm));
	new->next=NULL;
	new->name = strdup(name);
	new->region = strdup(region); 
	new->location = strdup(location);
	new->number_of_turbines=number_of_turbines;
	new->turbine_capacity_mw=turbine_capacity_mw;
	new->farm_capacity_mw=farm_capacity_mw;
	new->operator=strdup(operator);
	new->start_date=strdup(mbuf);
	new->status=(!strcmp(status,"Consented")? CONSENTED: !strcmp(status,"Operational") ? OPERATIONAL: UNDER_CONSTRUCTION);
	new->type=(!strcmp(type,"onshore")? ONSHORE:OFFSHORE);
	new->metered = 0; // until we know which ones ARE embedded.
	if(first_windfarm!=NULL)
		new->next=first_windfarm; // set link pointer to old head of list
	else 
		new->next=NULL; // first entry, set pointer to NULL to establish the tail
	first_windfarm=new; // set the head to the new list item
	}
//delete the list
void free_entries()
	{
	struct windfarm *p,*next;
	for(p=first_windfarm;p;p=next)	
		{
		next=p->next; // take a copy before we free up memory its stored in..
		free(p->region);
		free(p->name);
		free(p->location);
		free(p->operator);
		free(p->start_date);
		free(p); // and clear the structure
		}
	}


// library functions to scrape a web page.
static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
	{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
    /* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		exit(EXIT_FAILURE);
	}
	memcpy(&(mem->memory[mem->size]), ptr, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
	}

char *scrape(char *url)
	{
	extern struct MemoryStruct chunk;
	CURL *curl_handle;
	CURLcode error;
  	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  	chunk.size = 0;    /* no data at this point */

  	curl_global_init(CURL_GLOBAL_ALL);

  	/* init the curl session */
  	curl_handle = curl_easy_init();

  	/* specify URL to get */
 	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

  	/* send all data to this function  */
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  	/* we pass our 'chunk' struct to the callback function */
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  	/* some servers don't like requests that are made without a user-agent
     field, so we provide one */
 	 curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

 	 /* get it! */
 	 error=curl_easy_perform(curl_handle);

  	/* cleanup curl stuff */
 	 curl_easy_cleanup(curl_handle);
	 curl_global_cleanup();
	return (error? ((char *)0):(chunk.memory));
	}
//return a ponter to the next line or NULL if its the last line
char *newline(char *p)
	{

	while(*p && *p!=0x0d && *p!= '\n') // bloody website uses CR rather than LF..
		p++;
	// p will equal '\n' or point to a null
	if (!*p)
		return (NULL);
	else
		return(p+1);
	}
// extract data from page into buffer, sanitise CR. 
void extract_data(char *buf, char *p)
	{
	while(*p)
		{
		if(strstr(p,"</p>")==p) // found terminator
			break;
		if(strstr(p,"&amp;")==p) // found ampersand.
			{
			p+=5;
			*buf++='&';
			continue;
			}
		if(*p==0x0d)
			p++; //skip any random shitty carriage returns
		else if (*p=='\'' && *(p-1)!='\\') // look for UNESCAPED inverted commas
			{
			*buf++='\\';  // insert backslashes for apostrophes 
			*buf++='\'';
			p++;
			}
		else 
			*buf++=*p++; // copy string to buffer

		}
	*buf=0; // terminate copy string.
	}
// scrape pages, and extract meaningful data and insert in linked list of windfarm structures.
/*<p class="panel-column1">Wind Project</p>
<p class="panel-column2">Region</p>
<p class="panel-column3">Location</p>
<p class="panel-column4">Turbines</p>
<p class="panel-column5">Project Capacity (MW)</p>
<p class="panel-column6">Turbine Capacity (MW)</p>
<p class="panel-column7">Developer</p>
<p class="panel-column8">Status Change Date</p>
<p class="panel-column9">Status of Project</p>
<p class="panel-column10">Type of Project</p>
* 
* <p class="panel-column1 panel-content">
Achairn Farm, Stirkoke
</p>
<p class="panel-column2">Scotland</p>
<p class="panel-column3">Wick</p>
<p class="panel-column4 style-turbines">3</p>
<p class="panel-column5 style-mw">2.05</p>
<p class="panel-column6 style-power">6.15</p>
<p class="panel-column7">Iinnes (James & Ronald)</p>
<p class="panel-column8">01-May-2009</p>
<p class="panel-column9">Operational</p>
<p class="panel-column10">onshore</p>
* */
void parse_scraped_data()
	{
	char 	region[32];
	char 	name[256];
	char 	location[256];
	int 	number_of_turbines;
	float 	turbine_capacity_mw; 
	float 	farm_capacity_mw; 
	char 	operator[2048]; 
	char 	start_date[256];
	char 	status[32];
	char 	type[16];
	int 	n, i, j;
	char 	*page,*p;
	char 	url[1024];
	char 	match[64];
	char 	data[1024];
	for(i=0;i<20;i++)
		{
		j=0;
		sprintf(url,"%s?page=%d",WINDFARM_URL,i+1);
		if(!(page=scrape(url)))
			break; // no more data or the thing is failed.
		// we seem to have a page.
		p=page; // p is the running pointer
		while(p) // until the end of the page
			{
			// set up the first match string;
			for(n=0;n<NUMBER_OF_FIELDS;n++)
				{
				switch (n+1)
					{
					case 1: sprintf(match,"<p class=\"panel-column1 panel-content\">");
						break;
					case 2:
					case 3:
					case 7:
					case 8:
					case 9:
					case 10:
					 	sprintf(match,"<p class=\"panel-column%d\">",n+1); //1-10 not 0-9
					 	break;
					case 4: sprintf(match,"<p class=\"panel-column4 style-turbines\">");
						break;
					case 5: sprintf(match,"<p class=\"panel-column5 style-mw\">");
						break;
					case 6: sprintf(match,"<p class=\"panel-column6 style-power\">");
						break;
					default: break;
					}
				while(p && (strstr(p,match) !=p)) // if line doesn't begin with it
					p=newline(p); 	// get a new line.
				if(!p) break; 		// if end of file, break
				// now to extract the value.
				p+=(strlen(match)); // p points to actual data now.
				extract_data(data,p); // get a copy of the string we are interested in, in data.
				switch(n+1)
					{
					case 1: strcpy(name,data);
						break;
					case 2: strcpy(region, data);
						break;
					case 3: strcpy (location, data);
						break;
					case 4: number_of_turbines=atoi(data);
						break;
					case 5: turbine_capacity_mw=atof(data);
						break;
					case 6: farm_capacity_mw=atof(data);
						break;
					case 7: strcpy(operator, data);
						break;
					case 8: strcpy(start_date, data);
						break;
					case 9: strcpy(status, data);
						break;
					case 10: strcpy(type, data);
						break;
					default: break;
					}
				// now insert the data in the list.
				if(n==NUMBER_OF_FIELDS-1)
					{
					add_entry(name,region,location,number_of_turbines, turbine_capacity_mw, farm_capacity_mw, operator, start_date, status, type);
					j++; // found one valid entry, at least.
					}
				} 	// end of one windfarm loop
			} 		// end of page loop
		if(page)
			free(page);// clear out page memory.
		if(!j)
				break; // found an empty page
		} 			// end of all pages loop
	}
// update database - the slow way!
/*
 * 
+---------------------+------------------------------------------------------+------+-----+---------+----------------+
| Field               | Type                                                 | Null | Key | Default | Extra          |
+---------------------+------------------------------------------------------+------+-----+---------+----------------+
| id                  | int(11)                                              | NO   | PRI | NULL    | auto_increment |
| name                | varchar(100)                                         | YES  |     | NULL    |                |
| region              | varchar(100)                                         | YES  |     | NULL    |                |
| location            | varchar(100)                                         | YES  |     | NULL    |                |
| number_of_turbines  | int(11)                                              | YES  |     | NULL    |                |
| farm_capacity_mw    | double                                               | YES  |     | NULL    |                |
| turbine_capacity_mw | double                                               | YES  |     | NULL    |                |
| operator            | varchar(256)                                         | YES  |     | NULL    |                |
| start_date          | date                                                 | YES  |     | NULL    |                |
| status              | enum('Operational','Under Construction','Consented') | NO   |     | NULL    |                |
| type                | enum('onshore','offshore')                           | NO   |     | NULL    |                |
| metered             | enum('Yes','No')                                     | NO   |     | NULL    |                |
+---------------------+------------------------------------------------------+------+-----+---------+----------------+

*/
int update_database()
	{
	MYSQL mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[4096];
	int len;
	struct windfarm *p;
	if(!mysql_init(&mysql)) // initalise data structure
		return 1;
	if(!mysql_real_connect(&mysql,"127.0.0.1",USERNAME,PASSWORD,DATABASE,0,"",0)) // connect to database
		{
		printf("Connect failed -%s\n",mysql_error(&mysql));
		return 2;
		}
	len=sprintf(query,"delete from %s",TABLE); //ditch the data
	if(mysql_real_query(&mysql,query,len))
		{
		mysql_close(&mysql);
		return 3; // bad query.
		}
	for(p=first_windfarm;p;p=p->next)
		{
		len=sprintf(query,"insert into %s set name='%s',region='%s',location = '%s', number_of_turbines='%d',farm_capacity_mw='%0.2f',turbine_capacity_mw='%0.2f',operator='%s',start_date='%s',status='%s',type='%s',metered='%s'",
			TABLE,
			p->name,
			p->region,
			p->location,
			p->number_of_turbines,
			p->farm_capacity_mw,
			p->turbine_capacity_mw,
			p->operator,
			p->start_date,
			(p->status==OPERATIONAL? "Operational":(p->status==UNDER_CONSTRUCTION? "Under Construction":"Consented" )),
			(p->type==ONSHORE? "onshore":"offshore"),
			(p->metered==METERED? "Yes":"No")
			);
		if(mysql_real_query(&mysql,query,len))
			{
			mysql_close(&mysql);
			return 3;
			}
		}
	mysql_close(&mysql);
	return 0;
	}
// print out data in CSV format with headers.

void output_data()
	{
	FILE *fp;
	struct windfarm *p;
#ifdef DEBUG
	printf("outputting data\n");
	fp=stdout;
#else
	fp=fopen("/tmp/windfarms.csv","w");
#endif
	fprintf(fp,"\"Wind Project\",\"Region\",\"Location\",\"Turbines\",\"Turbine Capacity (MW)\",\"Project Capacity (MW)\",\"Developer\",\"Status Change Date\",\"Status of Project\",\"Type of Project\",\"Metered\"\n");
	fflush(fp);
	for(p=first_windfarm;p;p=p->next)
		{
		fprintf(fp," \"%s\",\"%s\",\"%s\",\"%d\",\"%0.2f\",\"%0.2f\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
			p->name,
			p->region,
			p->location,
			p->number_of_turbines,
			p->turbine_capacity_mw,
			p->farm_capacity_mw,
			p->operator,
			p->start_date,
			(p->status==OPERATIONAL? "operational":(p->status==UNDER_CONSTRUCTION? "under construction":"consented" )),
			(p->type==ONSHORE? "onshore":"offshore"),
			(p->metered==METERED? "Yes":"No")
			);
		fflush(fp);
		}
#ifndef DEBUG
	fclose(fp);
#endif
	}

main (int argc, char**argv)
	{
	parse_scraped_data();
	update_database();
	output_data();
	free_entries();
	}
