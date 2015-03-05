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

#define RUNFILE "/tmp/nationalgrid"


// #define RUNFILE "/tmp/test"
// #define DEBUG 1

float get_last_value(char *buffer, char *type, float existing) // get value associate with LAST XML entry.
	{
	char *p, *last;
	float value;
	p=buffer;
	last=(char *)0;
	while(p=strstr(p,type))
		{	 
		last=p;
		p++; // +1 otherwise infinite loop.
		}
	if(last)
		{
		p=strstr(last,"VAL=\"")+5;
		sscanf(p,"%f",&value);
		return(value);
		}
	else return(existing);
	}

float get_value(char *buffer, char* type, float existing)
	{
	char *p;
	float value;
	p=strstr(buffer,type);
	if(p)
		{
		p=strstr(p,"VAL=\"")+5;
		sscanf(p,"%f",&value);
		return(value);
		}
	else return(existing);
	}
float get_total(char *buffer, float existing)
	{
	char *p;
	float value;
	p=strstr(buffer,"TOTAL=\"")+7;
	if(p)
		{
		sscanf(p,"%f",&value);
		return(value);
		}
	else return (existing);	
	}
int runfile_bad()
	{
	struct stat buf;
	int ret;
	time_t t1;
	(void) time(&t1);
 	if(stat(RUNFILE,&buf)) // file not there
		return (1);
	if(t1> buf.st_mtime +120)
		return (2);
	else return (0);
	}
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

int insert_data(float ccgt,float ocgt,float oil,float coal,float nuclear,float wind,float pumped,float hydro,float other,float france, float ireland,float holland, float ew, float total,float frequency)
	{
	MYSQL mysql;
	char query[4096];
	time_t t1;
	char timestr[256];
	int len;
	t1=time(NULL); // get current time.
	strftime(timestr,254,"%Y-%m-%d %H:%M:%S",gmtime(&t1)); // into SQL format..
	// Prepare the query
	len=sprintf(query,"insert into day set timestamp='%s',demand='%f',frequency='%f',coal='%f',nuclear='%f',ccgt='%f',ocgt='%f',french_ict='%f', irish_ict='%f', dutch_ict='%f', ew_ict= '%f', pumped='%f',hydro='%f',wind='%f',oil='%f',other='%f'",
		timestr,total,frequency,coal, nuclear,ccgt,ocgt,france,ireland,holland,ew,pumped,hydro,wind,oil,other);
	// printf("%s\n",query);	
	if(!mysql_init(&mysql)) // initalise data structure
		return 1;
	if(!mysql_real_connect(&mysql,"127.0.0.1",USERNAME,PASSWORD,DATABASE,0,"",0)) // connect to database
		{
		printf("Connect failed -%s\n",mysql_error(&mysql));
		return 2;
		}
	if(mysql_real_query(&mysql,query,len))
		{
		mysql_close(&mysql);
		return 3; // bad query.
		}
	mysql_close(&mysql);
	return(0);
	}

int main(void)
	{
    float ccgt;
    float ocgt;
    float oil;
    float coal;
    float nuclear;
    float wind;
    float pumped;
    float hydro;
	float other;
    float france;
    float ireland;
    float holland;
	float ew;
    float total;
    float frequency;
	float tmp;
    extern struct MemoryStruct chunk;
    char *f;
    FILE *fp;
    char linebuf[128];
	MYSQL mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[4096];
	int len;
	// we assume we fail in getting the data so load existing, which will get overwrtten by good data if available.
	len=sprintf(query,"select ccgt,ocgt,oil,coal,nuclear,wind,pumped,hydro,other,demand,frequency,french_ict,dutch_ict,irish_ict, ew_ict from day order by id desc limit 1");
	if(!mysql_init(&mysql)) // initalise data structure
		return 1;
	if(!mysql_real_connect(&mysql,"127.0.0.1",USERNAME,PASSWORD,DATABASE,0,"",0)) // connect to database
		{
		printf("Connect failed -%s\n",mysql_error(&mysql));
		return 2;
		}
	if(mysql_real_query(&mysql,query,len))
		{
		mysql_close(&mysql);
		return 3; // bad query.
		}
	// assume query done, now load data
	result=mysql_store_result(&mysql);
	row=mysql_fetch_row(result);
	// load up the data
	ccgt=atof(row[0]);
	ocgt=atof(row[1]);
	oil=atof(row[2]);
	coal=atof(row[3]);
	nuclear=atof(row[4]);
	wind=atof(row[5]);
	pumped=atof(row[6]);
	hydro=atof(row[7]);
	other=atof(row[8]);
	total=atof(row[9]);
	frequency=atof(row[10]);
	france=atof(row[11]);
	holland=atof(row[12]);
	ireland=atof(row[13]);
	ew=atof(row[14]);
	
	mysql_free_result(result);
	mysql_close(&mysql);
 #ifdef DEBUG
	printf (" ccgt  | ocgt  | oil   | coal  |nuclear| wind  | pumped| hydro | other | demand| frequency|french_ict| dutch_ict| irish_ict|ew_ict|\n");
	printf ("%*f|%*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f|%*f|\n",7,ccgt,7,ocgt,7,oil,7,coal,7,nuclear,7,wind,7,pumped,7,hydro,7, other,7,total,10,frequency,10,france,10,holland,10,ireland,10,ew);
 #endif	
  	if(scrape("http://www.bmreports.com/bsp/additional/soapfunctions.php?element=generationbyfueltypetable"))
		{
		ccgt=get_value(chunk.memory,"CCGT",ccgt);
  		ocgt=get_value(chunk.memory,"OCGT",ocgt);
  		oil=get_value(chunk.memory,"OIL",oil);
  		coal=get_value(chunk.memory,"COAL",coal);
  		nuclear=get_value(chunk.memory,"NUCLEAR",nuclear);
  		wind=get_value(chunk.memory,"WIND",wind);
  		pumped=get_value(chunk.memory,"PS",pumped);
		hydro=get_value(chunk.memory,"NPSHYD",hydro);
  		other=get_value(chunk.memory,"OTHER",other);
		total=get_total(chunk.memory,total);
		}
    if(chunk.memory)
    	free(chunk.memory);	
	if(scrape("http://www.bmreports.com/bsp/additional/soapfunctions.php?element=rollingfrequency&output"))
		{
		frequency=get_last_value(chunk.memory,"ST=",frequency);
		}
	if(chunk.memory)
    	free(chunk.memory);	
	if(scrape("http://www.bmreports.com/bsp/additional/soapfunctions.php?element=interconnectorsgraph&output="))
		{	
		france=get_last_value(chunk.memory,"INTERCONNECTOR TYPE=\"INTFR\"",france);
		ireland=get_last_value(chunk.memory,"INTERCONNECTOR TYPE=\"INTIRL\"",ireland);
  		holland=get_last_value(chunk.memory,"INTERCONNECTOR TYPE=\"INTNED\"",holland);
		ew=get_last_value(chunk.memory,"INTERCONNECTOR TYPE=\"INTEW\"",ew);
		}
	if(chunk.memory)
    	free(chunk.memory);	
    if(!(fp=fopen(RUNFILE,"w")))
		{
		printf(" Unable to open %s\n", RUNFILE);
		exit(-1);
		}	// open and smack the contents   
    fprintf( fp,"%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.0f\n%0.03f\n%0.03f\n", 
			ccgt,ocgt,oil,coal,nuclear,wind,pumped,hydro,other,france,ireland,holland,total,frequency,ew);
	fclose(fp);
#ifdef DEBUG
	printf ("%*f|%*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f| %*f|%*f|\n",7,ccgt,7,ocgt,7,oil,7,coal,7,nuclear,7,wind,7,pumped,7,hydro,7, other,7,total,10,frequency,10,france,10,holland,10,ireland,10,ew);
#else
	insert_data(ccgt,ocgt,oil,coal,nuclear,wind,pumped,hydro,other,france,ireland,holland,ew,total,frequency);
#endif
	return 0;
	}

