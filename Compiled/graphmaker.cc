#include <pngwriter.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mysql/mysql.h>
#include <sys/stat.h>
#include "mydatabase.h"

#define STARTX 40
#define STARTY 14
#define MAPX 360
#define MAPY 100
#define FONTPATH "/var/www.gridwatch.templar.co.uk/fonts/lat725m.ttf"
#define GRAPHPATH "/var/www.gridwatch.templar.co.uk/"
#define RED "#FF0000"
#define GREEN "#00FF00"
#define BLUE "#0000FF"
#define CYAN "#00FFFF"


struct line_def
{
float carray[3];
char title[256];
int solid;
int lastx;
int lasty;
float current;
};

// global mysql stuff	
MYSQL mysql;
MYSQL_RES *result;
MYSQL_ROW row;
int rows; // number of db records read.

int open_database()
	{
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
	mysql_query(&mysql, "set time_zone= '+00:00'"); // otherwise unix_timestamp uses local server timezone..
	return 0;
	}
int query_database(char *values, int limit)
	{
	char buffer[4096];
	sprintf(buffer,"select id,unix_timestamp(timestamp)as t,%s from day order by id desc limit %d", values, limit);
	if(mysql_query(&mysql,buffer))
		{
		mysql_close(&mysql);
		return 3;
		}
	result=mysql_store_result(&mysql);
	rows=mysql_num_rows(result);
	return 0;
	}

void close_database()
	{
	mysql_free_result(result);
	mysql_close(&mysql);
	}

char *fullpath(char*filename)
	{
	static char buffer[4096]; //enough?
	sprintf(buffer,"%s%s",GRAPHPATH,filename);
	return buffer;
	}
int update_needed(const char *filename, long timeval)
	{
	struct stat buf;
	int ret;
	time_t t1;
	(void) time(&t1);
 	if(stat(filename,&buf)) // file not there
		return (1);
	if(t1> buf.st_mtime +timeval)
		return (2);
	else return (0);
	}
// parse e.g. #FFA003 into color values
int get_colors(float carray[3], char * color)
	{
	long cvalue;
	char *end;
	if(color[0] !='#')
		return -1;
	else color++; // skip the hash
	cvalue= strtol(color, &end, 16);
	carray[2]= ((float)(cvalue & 255))/255;
	cvalue>>=8;
	carray[1]= ((float)(cvalue & 255))/255;
	cvalue>>=8;
	carray[0]= ((float)(cvalue & 255))/255;
	return 0;
	}
// parse two comma separated character strings into color values and solid/line values
int get_line_types(struct line_def ld[256], char *colors, char *styles, char *titles)
	{
	char *ptr,*newptr;
	char newbuf[1024];
	int i,j;
	// zero our array;
	for (i=0; i<256;i++)
		{
		ld[i].solid=0; // assume line;
		ld[i].carray[0]=ld[i].carray[1]=ld[i].carray[2]=0.0;
		ld[i].title[0]=0;
		ld[i].current=0.0;
		}
	// now for the line types. "solid", "SOLID" or simply anything else is line.
	// convert input to uppercase
	for (ptr=styles,newptr=newbuf;*ptr; newptr++,ptr++) // scan the string
		{
		*newptr=toupper(*ptr);	
		}
	*newptr=0;
	for (ptr=newbuf, i=0;*ptr; ptr++) // scan the string
		{
		if(!strncmp(ptr,"SOLID",5))
			{
			ld[i].solid=1;
		 	}
		if(*ptr==',') i++; // next value (if any)
		}
	// parse the titles..
	for (ptr=titles,i=0, j=0; *ptr;ptr++,j++)
		{
		if(*ptr==',')// skip to next
			{
			ld[i].title[j]=0; // terminate existing
			i++; // next graph;
			j=-1; //reset pointerto -1 - gets incremented next loop
			}
		else ld[i].title[j]= (*ptr);
		}
	ld[i].title[j]=0; // terminate last one
	i=0;
	for (ptr=colors;*ptr; ptr++) // scan the string
		{
		if (*ptr=='#') // color start
			{
			get_colors(ld[i].carray, ptr); // load up color values..
			i++;
			}
		}
	return (i); //number of colors found..
	}
// horiz or vertical dotted line..	
void dotted_line(pngwriter *image, int startx, int starty, int endx, int endy)
	{
	int i;
	if(starty==endy)
		{
		for (i=startx;i<=endx;i++)
			{
			if ((i%3))
				(*image).plot(i,starty,0,0,0);
			}
		}
	else
		{
		for (i=starty;i<=endy;i++)
			{
			if (!(i%3))
				(*image).plot(startx,i,0,0,0);
			}
		}
	}
//vertical red line..mod this for variable height
void red_line(pngwriter *image,int x)
	{
	(*image).line(x+STARTX,STARTY,x+STARTX,MAPY-22,1.0,0.0,0.0);
	}

// add the title bar along the top
int write_legend( pngwriter *image, struct line_def lines[], int graphs )	
	{
	int x,y,j;
	x=STARTX +7;
	y=MAPY-17;
	for(j=0;j<graphs;j++)
		{
		 (*image).filledsquare(x,y-2,x+10,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
		x+=12;
		(*image).plot_text((char *)FONTPATH,7,x,y-3,0.0,lines[j].title, 0,0,0);
		x+=((*image).get_text_width((char *)FONTPATH,7,lines[j].title) + 7);
		}
	return 0;
	}	
//horizontal lines and LHS numbers
void y_grid(pngwriter *image, int min, int max, int interval, float scale_factor)
	{
	int i,y;
	char buffer[32];
	for (i=min;i<=max;i+=interval)
		{
		y= STARTY + (int)(((float)((i-min))/scale_factor) +0.5);
		sprintf(buffer,(max>=100 && max<1000)? "%5.0f":"%5.1f",max>=1000? (float)i/1000.0:(float)i);
		(*image).plot_text((char *)FONTPATH,7,STARTX-23,y-3,0.0,buffer, 0,0,0);
		dotted_line(image, STARTX,y,STARTX+288,y);
		}
	}
// draw a day x axis and label it.
void x_grid_day(pngwriter *image)
	{
	time_t t1;
	struct tm *tlocal;
	long offset;
	int x,scale;
	char buffer[256];
	// find midnight on the graph
	t1=time(NULL); // get current time.
	tlocal=localtime(&t1);
	offset=t1/(5 * 60) ; // 5 minute periods since midnight
	if(tlocal->tm_isdst) // daylight savings
		offset+=12;
	offset = 288-(offset%288);
	red_line(image,offset);
	for(scale=48,x=STARTX+offset;x>=STARTX;x-=6)
		scale-=1;
	x+=6;
	for(;x<=288+STARTX;x+=6, scale++)
		{
		if(scale%2)
	 		dotted_line(image, x,STARTY,x,MAPY-22);
		if (scale ==48)
			scale=0;
		if (!(scale %4) && (x-11)>STARTX) // we mark here
			{
			sprintf(buffer,"%02d",scale/2 );
			(*image).plot_text((char *)FONTPATH,7,x -11,STARTY-8,0.0,buffer, 0,0,0);
			}
		}
	}
void x_grid_week(pngwriter *image)
	{
	time_t t1;
	struct tm *tlocal;
	int i,j,x,x1;
	long offset;
	const char *dow [7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	// find midnight Sunday on the graph
	t1=time(NULL); // get current time.
	tlocal=localtime(&t1);
	j=tlocal->tm_wday; // number of today! or is it?
	// calculate at what x our redline should go
	// 41 pixels per diem.
	// get minutes past midnight, sunday
	offset=(long)tlocal->tm_wday * 24 * 3600 + (long)tlocal->tm_hour*3600+(long)tlocal->tm_min*60+(long)tlocal->tm_sec;
	offset/=2107;
	offset=288-offset;
	red_line(image,offset);
	j=tlocal->tm_wday; //today.
	x=tlocal->tm_hour*3600+tlocal->tm_min*60+tlocal->tm_sec;
	x=(x*41)/(24*3600); 
	x=STARTX+288-x;
	for (i=0;i<7;i++,x-=41)
		{
		dotted_line(image, x,STARTY,x,MAPY-22);
		x1=x+10;
		if(x1>STARTX && x1<(STARTX+270))
			(*image).plot_text((char *)FONTPATH,7,x1,STARTY-8,0.0,(char *)dow[j], 0,0,0);
		j--;
		if(j<0)
			j=6;
		}
	}
	
void x_grid_month(pngwriter *image)
	{
	time_t t1;
	struct tm *tlocal;
	long offset,offset1;
	int x,i,j, dom;
	char buffer[256];
	int dim[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	t1=time(NULL); // get current time.
	tlocal=localtime(&t1);
	j=tlocal->tm_mday; // number of today!
	// calculate at what x our redline should go
	// 36 samples per pixel pixels per diem.
	// get seconds past midnight, first of month
	offset=(long)(tlocal->tm_mday-1) * 24 * 3600 + (long)tlocal->tm_hour*3600+(long)tlocal->tm_min*60+(long)tlocal->tm_sec;
	offset=288-(offset/10800); // 10800 seconds per pixel
	red_line(image,offset);
	// check for last month as well
	j=tlocal->tm_mon -1;
	if (j<0)
		j=11; // last month was december?
	offset1 = offset -(dim[j] * 8); // 8 pixels per day. 3 hours per pixel.
	if(offset1>0)
		red_line(image,offset1);
	offset=(long)tlocal->tm_hour*3600+(long)tlocal->tm_min*60+(long)tlocal->tm_sec; //offset form graph end in seconds
	offset=288-(offset/10800);
	dom=tlocal->tm_mday; //todays date
	j=tlocal->tm_mon;
	for(x=offset;x>0;x-=8) // step through days
		{
		if(dom<1) // gone over a month start
			{
			j--; //last month
			if(j<0) j=11; //if that was december, make it so
			dom=dim[j]; //and set day of month to number of days in the month.
			}
		dotted_line(image, x+STARTX,STARTY,x+STARTX,MAPY-22); //add a vertical
		// if day is unprintable range, skip it
		i=x-2; //text  position
		if(i>0 && i< 288 && !(dom % 2)) 
			{
			sprintf (buffer,"%02d",dom); // the correct day
			(*image).plot_text((char *)FONTPATH,7,i+STARTX,STARTY-8,0.0,buffer, 0,0,0);
			}
		dom--;
		}
	}
void x_grid_year(pngwriter *image)
	{
	time_t t1;
	struct tm *tlocal;
	long offset;
	int i,j,x,scale;
	char buffer[256];
	const char *moy [12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	int dim[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	t1=time(NULL); // get current time.
	tlocal=localtime(&t1);
	j=tlocal->tm_mon; // this month
	// calculate at what x our redline should go
	// 109800 seconds per pixel 
	offset=(long)(tlocal->tm_yday) * 24 * 3600 + (long)tlocal->tm_hour*3600+(long)tlocal->tm_min*60+(long)tlocal->tm_sec;
	offset/=109800l; // 109800 seconds per pixel
	offset=288-offset;
	red_line(image,offset);
	// now put in the month end lines
	for(i=0,j=0; i<12; i++)
		{
		j+=dim[i]; //add in days
		if(i==1 && !(tlocal->tm_year%4)) //leap year and february	
			j++;
		// calculate x position from offset its offset + (j/365 *288)
		x=offset+ STARTX +(j*288 +183)/365;
		if (x>STARTX+288)
			x-=288;
		if(i!=11) // dont print end dec or that overwrites red line.
			dotted_line(image,x,STARTY,x,MAPY-22);
		sprintf (buffer,"%s",moy[i]);
		if(x-21>STARTX)
			(*image).plot_text((char *)FONTPATH,7,x-21,STARTY-8,0.0,buffer, 0,0,0);
		}
	}
// add a border
void add_borders(pngwriter *image)
	{
	(*image).square(1,1,MAPX,MAPY,0.5,0.5,0.5);
	(*image).square(STARTX,STARTY,STARTX+288,MAPY-22,0,0,0);
	}
// make a year graph
int year_graph(char *titles, char *values, char *colors, char *style, char *filename, int interval,int min, int max)
	{
	long i,j,k;
	int graphs;
	int scale;
	long offset;
	int x,y;
	long timestamp;
	float value;
	float scale_factor;
	char buffer [1024];
	struct line_def lines[256]; // plenty!!
	filename=fullpath(filename);
	// check if our file needs updating..
	// at 180  inutes per pixel thats after 10800
	if(!update_needed(filename,(long) 100000))
		return 0;
	// parse inputs to line_def structure array.
	graphs=get_line_types(lines,colors,style,titles); 
	// calculate scale factor and where to put the grid lines.
	scale_factor= ((float)(max-min))/((float)(MAPY-STARTY-22)); // units per pixel.
	if(open_database())
		return 1; //abort if database is not there
	if(query_database(values, 105408))
		return 1;
	pngwriter image(MAPX, MAPY, 1.0, filename); // create raw image.
	// build the graph.
	for(i=0;i<rows;i++)
		{
		if (!(i%366))
			{
			x=288+STARTX-i/366;
			}
		row=mysql_fetch_row(result);
		for(j=0;j<graphs;j++)
			{
			lines[j].current+=atof(row[2+j]); // update running average
			if (!(i%366))
				{
				if (i==0) //preload array
					{
					lines[j].lasty=STARTY + (int)((lines[j].current-(float)min)/scale_factor +0.5);
					lines[j].lastx=x;
					lines[j].current*=366;
					}
				y=STARTY + (int)((lines[j].current/366 -(float)min)/scale_factor +0.5);
				lines[j].current=0.0;
				if(lines[j].solid)
					image.line(x,STARTY,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				else 
					image.line(lines[j].lastx,lines[j].lasty,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				lines[j].lastx=x;
				lines[j].lasty=y;	
				}
			}
		}
	close_database();
	add_borders(&image);
	y_grid(&image,min,max,interval,scale_factor);
	x_grid_year(&image);
	write_legend( &image, lines, graphs );
	image.close();
	return 0;
	} 	
	

int month_graph(char *titles, char *values, char *colors, char *style, char *filename, int interval,int min, int max)
	{
	int i,j,k;
	int graphs;
	int scale;
	long offset,offset1;
	int x,y;
	long timestamp;
	float value;
	float scale_factor;
	struct line_def lines[256]; // plenty!!
	// check if our file needs updating..
	// at 180  minutes per pixel thats after 10800
	filename=fullpath(filename);
	if(!update_needed(filename,(long) 10000))
		return 0;
	// parse inputs to line_def structure array.
	graphs=get_line_types(lines,colors,style,titles); 
	// calculate scale factor and where to put the grid lines.
	scale_factor= ((float)(max-min))/((float)(MAPY-STARTY-22)); // units per pixel.
	if(open_database())
		return 1;
	if(query_database(values, 10368))
		return 1;
	pngwriter image(MAPX, MAPY, 1.0, filename); // create raw image.
	// splat some data
	for(i=0;i<rows;i++)
		{
		if (!(i%36))
			{
			x=288+STARTX-i/36;
			}
		row=mysql_fetch_row(result);
		for(j=0;j<graphs;j++)
			{
			lines[j].current+=atof(row[2+j]); // update running average
			if (!(i%36))
				{
				if (i==0) //preload array
					{
					lines[j].lasty=STARTY + (int)((lines[j].current-(float)min)/scale_factor +0.5);
					lines[j].lastx=x;
					lines[j].current*=36;
					}
				y=STARTY + (int)((lines[j].current/36 -(float)min)/scale_factor +0.5);
				lines[j].current=0.0;
				if(lines[j].solid)
					image.line(x,STARTY,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				else 
					image.line(lines[j].lastx,lines[j].lasty,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				lines[j].lastx=x;
				lines[j].lasty=y;	
				}
			}
		}
	close_database();
	add_borders(&image);
	y_grid(&image,min,max,interval,scale_factor);
	x_grid_month(&image);
	write_legend(&image, lines, graphs);
	image.close();
	return 0;
	} 

int week_graph(char *titles, char *values, char *colors, char *style, char *filename, int interval,int min, int max)
	{
	int i,j;
	int graphs;
	int scale;
	long offset;
	int x,y;
	float value;
	float scale_factor;
	char buffer [1024];
	struct line_def lines[256]; // plenty!!
	filename=fullpath(filename);
	// check if our file needs updating..
	// at 41 pixels per diem thats after 24*3600/41 seconds say if over 2107 seconds stale..
	if(!update_needed(filename,(long) 2107))
		return 0;
	// parse inputs to line_def structure array.
	graphs=get_line_types(lines,colors,style,titles); 
	// calculate scale factor and where to put the grid lines.
	scale_factor= ((float)(max-min))/((float)(MAPY-STARTY-22)); // units per pixel.
	if(open_database())
		return 1;
	if(query_database(values, 2016))
		return 1;
	pngwriter image(MAPX, MAPY, 1.0, filename); // create raw image.
	// splat some data
	for(i=0;i<rows;i++)
		{
		if (!(i%7))
			{
			x=288+STARTX-i/7;
			}
		row=mysql_fetch_row(result);
		for(j=0;j<graphs;j++)
			{
			lines[j].current+=atof(row[2+j]); // update running average
			if (!(i%7))
				{
				if (i==0) //preload array
					{
					lines[j].lasty=STARTY + (int)((lines[j].current-(float)min)/scale_factor +0.5);
					lines[j].lastx=x;
					lines[j].current*=7;
					}
				y=STARTY + (int)((lines[j].current/7 -(float)min)/scale_factor +0.5);
				lines[j].current=0.0;
				if(lines[j].solid)
					image.line(x,STARTY,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				else 
					image.line(lines[j].lastx,lines[j].lasty,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
				lines[j].lastx=x;
				lines[j].lasty=y;	
				}
			}
		}
	close_database();
	add_borders(&image);
	y_grid(&image,min,max,interval,scale_factor);
	x_grid_week(&image);
	write_legend( &image, lines, graphs );
	image.close();
	return 0;
	} 	
	
int day_graph(char *titles, char *values, char *colors, char *style, char *filename, int interval,int min, int max)
	{
	int i,j;
	int graphs;
	int scale,offset;
	int x,y;
	long timestamp;
	float value;
	float scale_factor;
	time_t t1;
	struct tm *tlocal;
	char buffer [1024];
	struct line_def lines[256]; // plenty!!
	// parse inputs to line_def structure array.
	filename=fullpath(filename);
	graphs=get_line_types(lines,colors,style,titles); 
	// calculate scale factor and where to put the grid lines.
	scale_factor= ((float)(max-min))/((float)(MAPY-STARTY-22)); // units per pixel.
	if (open_database())
		return 1;
	if (query_database(values,288))
		return 1;
	pngwriter image(MAPX, MAPY, 1.0, filename); // create raw image.
	// splat some data
	for(i=0;i<rows;i++)
		{
		x=288+STARTX-i;
		row=mysql_fetch_row(result);
		for(j=0;j<graphs;j++)
			{
			value=atof(row[2+j]);
			y=STARTY + (int)((value -(float)min)/scale_factor +0.5);
			if (i==0) 
				{
				lines[j].lasty=y;
				lines[j].lastx=x;
				}
			if(lines[j].solid)
				image.line(x,STARTY,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
			else 
				image.line(lines[j].lastx,lines[j].lasty,x,y,lines[j].carray[0],lines[j].carray[1],lines[j].carray[2]);
			lines[j].lastx=x;
			lines[j].lasty=y;	
			}
		}
	close_database();
	add_borders(&image);
	y_grid(&image,min,max,interval,scale_factor);
	x_grid_day(&image);
	write_legend( &image, lines, graphs );
	image.close();
	return 0;
	} 	
	
int main()
{
day_graph((char *)"UK Demand(GW)", (char *)"demand", (char *)"#505050", (char *)"SOLID",(char *)"demand-day.png",10000,0,60000);
day_graph((char *)"Nuclear (GW),Coal (GW),CCGT(GW)", (char *)" nuclear, coal, ccgt ", (char *)"#80FF00,#0000FF,#FF0000", (char *)"SOLID,LINE,LINE",(char *)"coal-nuke-ccgt-day.png",5000,0,25000);
day_graph((char *)"Hydro(GW),Pumped(GW),Wind(GW),Bio.(GW)", (char *)"hydro, pumped, wind, other", (char *)"#00FF60,#0000FF,#FF0000,#000000", (char *)"SOLID,LINE,LINE,LINE",(char *)"wind-hydro-pumped-day.png",1000,0,6000);
day_graph((char *)"French (GW),Dutch (GW),Irish (GW), EW (GW)", (char *)"french_ict, dutch_ict, irish_ict, ew_ict", (char *)"#0000FF,#FF0000,#007F00. #808880", (char *)"LINE,LINE,LINE.LINE",(char *)"ict-day.png",500,-2000,2000);
week_graph((char *)"UK Demand(GW)", (char *)"demand", (char *)"#505050", (char *)"SOLID",(char *)"demand-week.png",10000,0,60000);
week_graph((char *)"Nuclear (GW),Coal (GW),CCGT(GW)", (char *)" nuclear, coal, ccgt ", (char *)"#80FF00,#0000FF,#FF0000", (char *)"SOLID,LINE,LINE",(char *)"coal-nuke-ccgt-week.png",5000,0,25000);
week_graph((char *)"Hydro(GW),Pumped(GW),Wind(GW),Bio.(GW)", (char *)"hydro, pumped, wind, other", (char *)"#00FF60,#0000FF,#FF0000,#000000", (char *)"SOLID,LINE,LINE,LINE",(char *)"wind-hydro-pumped-week.png",1000,0,6000);
week_graph((char *)"French (GW),Dutch (GW),Irish (GW), EW (GW)", (char *)"french_ict, dutch_ict, irish_ict, ew_ict", (char *)"#0000FF,#FF0000,#007F00. #808880", (char *)"LINE,LINE,LINE.LINE",(char *)"ict-week.png",500,-2000,2000);
month_graph((char *)"UK Demand(GW)", (char *)"demand", (char *)"#505050", (char *)"SOLID",(char *)"demand-month.png",10000,0,60000);
month_graph((char *)"Nuclear (GW),Coal (GW),CCGT(GW)", (char *)" nuclear, coal, ccgt ", (char *)"#80FF00,#0000FF,#FF0000", (char *)"SOLID,LINE,LINE",(char *)"coal-nuke-ccgt-month.png",5000,0,25000);
month_graph((char *)"Hydro(GW),Pumped(GW),Wind(GW),Bio.(GW)", (char *)"hydro, pumped, wind, other", (char *)"#00FF60,#0000FF,#FF0000,#000000", (char *)"SOLID,LINE,LINE,LINE",(char *)"wind-hydro-pumped-month.png",1000,0,6000);
month_graph((char *)"French (GW),Dutch (GW),Irish (GW), EW (GW)", (char *)"french_ict, dutch_ict, irish_ict, ew_ict", (char *)"#0000FF,#FF0000,#007F00. #808880", (char *)"LINE,LINE,LINE.LINE",(char *)"ict-month.png",500,-2000,2000);
year_graph((char *)"UK Demand(GW)", (char *)"demand", (char *)"#505050", (char *)"SOLID",(char *)"demand-year.png",10000,0,60000);
year_graph((char *)"Nuclear (GW),Coal (GW),CCGT(GW)", (char *)" nuclear, coal, ccgt ", (char *)"#80FF00,#0000FF,#FF0000", (char *)"SOLID,LINE,LINE",(char *)"coal-nuke-ccgt-year.png",5000,0,25000);
year_graph((char *)"Hydro(GW),Pumped(GW),Wind(GW),Bio.(GW)", (char *)"hydro, pumped, wind, other", (char *)"#00FF60,#0000FF,#FF0000,#000000", (char *)"SOLID,LINE,LINE,LINE",(char *)"wind-hydro-pumped-year.png",1000,0,6000);
year_graph((char *)"French (GW),Dutch (GW),Irish (GW), EW (GW)", (char *)"french_ict, dutch_ict, irish_ict, ew_ict", (char *)"#0000FF,#FF0000,#007F00. #808880", (char *)"LINE,LINE,LINE.LINE",(char *)"ict-year.png",500,-2000,2000);

}

