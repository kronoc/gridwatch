<?php
if (isset($_SERVER['HTTP_ACCEPT_ENCODING']) && substr_count($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')) ob_start("ob_gzhandler");
else ob_start();
// global arrays and variables
$switches=array(
	'None'=>'none',
	'Demand'=>'demand',
	'Frequency'=>'frequency',
	'Coal'=>'coal',
	'Nuclear'=>'nuclear',
	'CCGT'=>'ccgt',
	'Wind'=>'wind',
	'Pumped'=>'pumped',
	'Hydro'=>'hydro',
	'Biomass'=>'other',
	'Oil'=>'oil',
	'OCGT'=>'ocgt',
	'French ICT'=>'french_ict',
	'Dutch ICT'=>'dutch_ict',
	'Irish ICT'=>'irish_ict',
	'E-W ICT'=>'ew_ict',
	'All'=>'all');

$daysinmonth = array (
	'Jan'=>31,
	'Feb'=>28,
	'Mar'=>31,
	'Apr'=>30,
	'May'=>31,
	'Jun'=>30,
	'Jul'=>31,
	'Aug'=>31,
	'Sep'=>30,
	'Oct'=>31,
	'Nov'=>30,
	'Dec'=>31);

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
        "http://www.w3.org/TR/1998/REC-html40-19980424/loose.dtd">
<html >
<head>
<TITLE>U.K. National Grid status download data</TITLE>
      <meta http-equiv="Content-Type" content="text/html;charset=utf-8" >
	   <link rel=stylesheet href="style.css" type="text/css">
</head>

<body onload="set_dates()">
<form name="myform" action="do_download.php" method="post">
<script  type="text/javascript">
var startdate = new Date(2009,04,13,22,30); 
var startlimit = new Date(2009,04,13,22,30);
var enddate = new Date(); 
var endlimit = new Date(); 
var monthnames = new Array("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec");



function set_dates()
	{
	var tmp,tmp1;
	var colon=':';
	if (enddate>endlimit)
		enddate.setTime(endlimit.getTime());
	if(startdate<startlimit)
		startdate.setTime(startlimit.getTime());
	if(startdate>enddate)
		startdate.setTime(enddate.getTime());
	enddate.setMinutes(Math.round(enddate.getMinutes()/5) *5);
	
	document.getElementsByName('startminute')[0].value=startdate.getMinutes();
	document.getElementsByName('starthour')[0].value=startdate.getHours();
	document.getElementsByName('startday')[0].value=startdate.getDate();
	document.getElementsByName('startmonth')[0].value=startdate.getMonth();
	document.getElementsByName('startyear')[0].value=startdate.getFullYear();
	document.getElementsByName('endminute')[0].value=enddate.getMinutes();
	document.getElementsByName('endhour')[0].value=enddate.getHours();
	document.getElementsByName('endday')[0].value=enddate.getDate();
	document.getElementsByName('endmonth')[0].value=enddate.getMonth();
	document.getElementsByName('endyear')[0].value=enddate.getFullYear();
	document.getElementById('startyearvalue').innerHTML=startdate.getFullYear();
	document.getElementById('endyearvalue').innerHTML=enddate.getFullYear();
	document.getElementById('startmonthvalue').innerHTML=monthnames[startdate.getMonth()];
	document.getElementById('endmonthvalue').innerHTML=monthnames[enddate.getMonth()];
	if((tmp=startdate.getDate())<10)
		tmp='0'+tmp;
	document.getElementById('startdayvalue').innerHTML=tmp;
	if((tmp=enddate.getDate())<10)
		tmp='0'+enddate.getDate();
	document.getElementById('enddayvalue').innerHTML=tmp;
	
 	tmp=startdate.getHours();
 	tmp1=startdate.getMinutes();
 	if(tmp<10)
		tmp='0'+tmp;
	if(tmp1<10)
		tmp1='0'+tmp1;
	document.getElementById('starthourvalue').innerHTML=tmp+colon+tmp1;
	tmp=enddate.getHours();
	tmp1=enddate.getMinutes();
	if(tmp<10)
		tmp='0'+tmp;
	if(tmp1<10)
		tmp1='0'+tmp1;
	document.getElementById('endhourvalue').innerHTML=tmp+colon+tmp1;
	}
function changeYear(i,amount)
	{
	var year,date;
	if(i)
		date=enddate;
	else date=startdate;
	year=date.getFullYear();
	if(amount>0)
		year++;
	else year--;
	date.setFullYear(year);
	set_dates();
	}

function changeMonth(i,amount)
	{
	var month, date;
	if(i)
		date=enddate;
	else date=startdate;
	month=date.getMonth();
	if(amount>0)
		month++;
	else month--;
	date.setMonth(month);
	set_dates();
	}

function changeDay(i,amount)
	{
	var day,date;
	if(i)
		date=enddate;
	else date=startdate;
	day=date.getDate();
	if(amount>0)
		day++;
	else day--;
	date.setDate(day);
	set_dates();
	}

function changeHours(i,amount)
	{
	var date, time;
	if(i)
		date=enddate;
	else date=startdate;
	time=date.getHours();
	if(amount>0)
		time++;
	else time--;
	date.setHours(time);
	set_dates();
	}
function changeMinutes(i,amount)
	{
	var date, time;
	if(i)
		date=enddate;
	else date=startdate;
	time=date.getMinutes();
	if(amount>0)
		time+=5;
	else time-=5;
	date.setMinutes(time);
	set_dates();
	}	
function clear_none_all()
	{
	document.getElementById('noneid').style.backgroundImage='url(bitmaps/LEDoff.gif)';
	document.getElementById('allid').style.backgroundImage='url(bitmaps/LEDoff.gif)';
	}
function toggle(id,hiddenvar)
	{
	var tmp;
	tmp=document.getElementsByName(hiddenvar)[0];
	if (tmp.getAttribute("value")=="off")
		{
		tmp.setAttribute("value","on");
		document.getElementById(id).style.backgroundImage='url(bitmaps/LEDon.gif)';
		}
	else 
		{
		tmp.setAttribute("value","off");
		document.getElementById(id).style.backgroundImage='url(bitmaps/LEDoff.gif)';
		}
	// after a time, clear down the 'none' variable and the 'all' variable.
	window.setTimeout(clear_none_all,750);
	}
function resetall()
	{
	<?
	foreach($switches as $legend=>$name)
		{
		printf("document.getElementsByName('%s')[0].setAttribute(\"value\",\"off\");\n",$name);
		printf("document.getElementById('%s').style.backgroundImage='url(bitmaps/LEDoff.gif)';\n",$name.'id');
		}
	printf("toggle('noneid','none');\n");
	?>
	}
function setall()
	{
	<?
	foreach($switches as $legend=>$name)
		{
		printf("document.getElementsByName('%s')[0].setAttribute(\"value\",\"on\");\n",$name);
		printf("document.getElementById('%s').style.backgroundImage='url(bitmaps/LEDon.gif)';\n",$name.'id');
		}
	printf("toggle('noneid','none');\n");
	?>	
	}
</script>
	<?
	$x=$x1=50;
	$y=100;
	foreach($switches as $legend=>$name)
		{
		selector_switch ($x,$y,$legend, $name.'id',$name,($name=='none' || $name=='all') ?"off":"on");
		$x+=75;
		}
	// take current x and use to determine caption width
	$width=$x-$x1;
	$y1=$y;
	$y+=200;
	$x=$x1+22;
	$spacing=325;
	double_bezel($x,$y,'Time','startimeid','starthour',22,'startminute',30);
	$x+=$spacing;
	single_bezel($x,$y,'Day','startdayid','startday',13);
	$x+=$spacing;
	single_bezel($x,$y,'Month','startmonthid','startmonth',0);
	$x+=$spacing;
	single_bezel($x,$y,'Year','startyearid','startyear','2009');
	$y+=220;
	$x=$x1+22;
	double_bezel($x,$y,'Time','endtimeid','endhour',0,'endminute',0);
	$x+=$spacing;
	single_bezel($x,$y,'Day','endayid','endday',26);
	$x+=$spacing;
	single_bezel($x,$y,'Month','endmonthid','endmonth',0);
	$x+=$spacing;
	single_bezel($x,$y,'Year','endyearid','endyear','2013');
	?>
	<div style="position: absolute; top:<?echo $y1-90?>px; left:<?echo $x1?>px; width:<?echo $width?>px; font-weight:bold; text-align:center; font-size:30px;">
	Download Data Sets from the GridWatch Database
	</div>
	<div style="position: absolute; top:<?echo $y1-40?>px; left:<?echo $x1?>px; width:<?echo $width?>px; font-weight:bold; text-align:center; font-size:24px;">
	Select Fields from data source
	</div>
	<div style="position: absolute; top:<?echo $y1+160?>px; left:<?echo $x1?>px; width:<?echo $width?>px; font-weight:bold; text-align:center; font-size:24px;">
	Select Start time of Dataset
	</div>
	<div style="position: absolute; top:<?echo $y1+380?>px; left:<?echo $x1?>px; width:<?echo $width?>px; font-weight:bold; text-align:center; font-size:24px;">
	Select End time of Dataset
	</div>
	<?
	web_button(50,20,"&lt;&lt;Gridwatch", "Download Data","/");
	submit_button(620,700,"DOWNLOAD","Download Data","do_download.php");
	?>
</form>
</body>
<?


function web_button($x,$y,$label, $alt,$url)
{
?>
<div class="button" onmouseover="this.className='hot-button'"  onmouseout="this.className='button'" style="Position: absolute; top:<?echo $y;?>px; left:<?echo $x;?>px; height: 20px; width: 150px; text-align:center;">
<a class="mybutton" href="<?echo $url?>"><? echo $label?></a>
</div>
<?
}
function submit_button($x,$y,$label, $alt,$url)
{
?>
<div class="button" 
onmouseover="this.className='hot-button'"  
onmouseout="this.className='button'" 
onclick="document.myform.submit()"
style="Position: absolute; top:<?echo $y;?>px; left:<?echo $x;?>px; height: 20px; width: 150px; text-align:center;">
<? echo $label?>
</div>
<?
}

function selector_switch($x,$y,$legend,$id,$hiddenvar,$initial_value)
	{
	?>
	<div id="<?echo $id?>" style="position: absolute; left: <?echo $x ?>px;top:<?echo $y ?>px; width: 100px; height: 180px; font-size:11px;  
	background-image: url(<?echo ($initial_value=='on'? "bitmaps/LEDon.gif":"bitmaps/LEDoff.gif")?>);
	background-position: center top;
	background-repeat: no-repeat;
	text-align: center;
	padding-top:120px;" >
	<input type="hidden" name="<?echo $hiddenvar; ?>" value="<?echo ($initial_value=='on'? 'on':'off')?>">
	<b><? echo $legend?></b>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+40 ?>px;top:<?echo $y+70?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	<?
	if($hiddenvar=='none')
		{
		?>
		onclick="resetall()"
		<?
		}
	else if($hiddenvar=='all')
		{
		?>
		onclick="setall()"
		<?
		}
	else 
		{
		?>
		onclick="toggle('<?echo $id?>','<?echo $hiddenvar?>')"
		<?
		}
	?>
	>
	</div>
	<?
	}
function single_bezel($x,$y,$legend,$id,$hiddenvar,$initial_value)
	{
	// work out if we are start or end and what javascript function
	if(strstr($hiddenvar,'end'))
		$dobj=1;
	else $dobj=0;
	if(strstr($hiddenvar,'year'))
		$jobj='changeYear';
	else if (strstr($hiddenvar,'month'))
		$jobj='changeMonth';
	else $jobj='changeDay';
	?>
	<div id="<?echo $id?>" style="position: absolute; left: <?echo $x ?>px;top:<?echo $y ?>px; width: 280px; height: 147px; 
	background-image: url(bitmaps/singlebezel.gif);
	background-position: center top;
	background-repeat: no-repeat;
	text-align: center;
	font-size: 24px;
	font-weight:bold;
	padding-top:112px;" >
	<input type="hidden" name="<?echo $hiddenvar; ?>" value="<?echo $initial_value?>">
	<b><? echo $legend?></b>
	</div>
	<div id="<?echo ($hiddenvar).'value'?>"
	style="position: absolute; left: <?echo $x ?>px;top:<?echo $y?>px; width: 280px; height: 147px; 
	text-align: center;
	font-size: 60px;
	font-weight:bold;
	padding-top:30px;" >
	<? echo $initial_value ?>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+247 ?>px;top:<?echo $y+10?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("%s(%s,1)",$jobj,$dobj);?>"
	>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+247 ?>px;top:<?echo $y+110?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("%s(%s,-1)",$jobj,$dobj);?>"
	>
	</div>

	<?
	}
function double_bezel($x,$y,$legend,$id,$hiddenvar1,$initial_value1,$hiddenvar2,$initial_value2)
	{
	// work out if we are start or end and what javascript function
	if(strstr($hiddenvar1,'end'))
		$dobj=1;
	else $dobj=0;
	
	?>
	
	<div id="<?echo $id?>" style="position: absolute; left: <?echo $x ?>px;top:<?echo $y ?>px; width: 280px; height: 147px; 
	background-image: url(bitmaps/doublebezel.gif);
	background-position: center top;
	background-repeat: no-repeat;
	text-align: center;
	font-size: 24px;
	font-weight:bold;
	padding-top:112px;" >
	<input type="hidden" name="<?echo $hiddenvar1; ?>" value="<?echo $initial_value1?>">
	<input type="hidden" name="<?echo $hiddenvar2; ?>" value="<?echo $initial_value2?>">
	<b><? echo $legend?></b>
	</div>
	<div id="<?echo ($hiddenvar1).'value'?>"
	style="position: absolute; left: <?echo $x ?>px;top:<?echo $y?>px; width: 280px; height: 147px; 
	text-align: center;
	font-size: 60px;
	font-weight:bold;
	padding-top:30px;" >
	<? printf("%02d:%02d",$initial_value1,$initial_value2);?>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+247 ?>px;top:<?echo $y+10?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("changeMinutes(%d,1)",$dobj)?>"
	>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+247 ?>px;top:<?echo $y+110?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("changeMinutes(%d,-1)",$dobj)?>"
	>
	</div>
	<div 
	style="position: absolute; left: <?echo $x+10 ?>px;top:<?echo $y+10?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("changeHours(%d,1)",$dobj)?>">
	</div>
	<div 
	style="position: absolute; left: <?echo $x+10 ?>px;top:<?echo $y+110?>px; width: 20px; height: 20px; 
	background-image: url(bitmaps/button.gif);
	background-position: center top;
	background-repeat: no-repeat;
	cursor: pointer;"
	onmouseover="this.style.backgroundImage='url(bitmaps/hotbutton.gif)' " 
	onmouseout="this.style.backgroundImage='url(bitmaps/button.gif)'"
	onclick="<?printf("changeHours(%d,-1)",$dobj)?>"
	>
	>
	</div>

	<?
	}
?>

