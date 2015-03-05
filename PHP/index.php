<?php
if (isset($_SERVER['HTTP_ACCEPT_ENCODING']) && substr_count($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')) ob_start("ob_gzhandler");
else ob_start();
if (strncmp($_SERVER['REMOTE_ADDR'],'93.89.132.226',13))
	$refresh=537;
else $refresh=120;
include('mydate.php');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
        "http://www.w3.org/TR/1998/REC-html40-19980424/loose.dtd">
<html >
<head>
<TITLE>U.K. National Grid status</TITLE>
      <meta http-equiv="Content-Type" content="text/html;charset=utf-8" >
	  <meta http-equiv="refresh" content="<? echo $refresh ?>" >
	  <link rel=stylesheet href="style.css" type="text/css">
	  <LINK REL="apple-touch-icon" HREF="/apple-touch-icon.png" />
	  <? include('tooltips.php') ?>
</head>
<body>
<span id="dummy"></span>
<div style="position: absolute;top:0px;left:200px; height: 15px; width: 1250px; text-align: center"><H1>U.K. National Grid Status</H1>
</div>
<div style="position: absolute;top:40px;left:200px; width: 1250px; text-align: center">
<i>Data courtesy of <a href="http://www.bmreports.com/bsp/additional/soapserver.php" target="_blank">BM Reports</a></i></div>
<?
date_default_timezone_set('GMT');
$filetime=filemtime("/tmp/nationalgrid");

$last_update_time=mydate($filetime); 
$retval=file_get_contents("/tmp/nationalgrid"); //no more live updates - stresses BM reports too much. Just pick up the last values that the cronjob left.
$results=explode("\n",$retval);
$demand=$results[12];
$freq=$results[13];
$nuclear=sprintf("%0.2f",$results[4]/1000);
$coal=sprintf("%0.2f",$results[3]/1000);
$ccgt=sprintf("%0.2f",$results[0]/1000);
$wind=sprintf("%0.2f",$results[5]/1000);
$hydro=sprintf("%0.2f",$results[7]/1000);
$pumped=sprintf("%0.2f",$results[6]/1000);
$french=sprintf("%0.2f",$results[9]/1000);
$irish=sprintf("%0.2f",$results[10]/1000);
$dutch=sprintf("%0.2f",$results[11]/1000);
$ew=sprintf("%0.2f",$results[14]/1000);
$oil=sprintf("%0.2f",$results[2]/1000);
$ocgt=sprintf("%0.2f",$results[1]/1000);
$other=sprintf("%0.2f",$results[8]/1000);

dial(-20,60,"Demand ".sprintf("%.2f",$demand/1000)."GW","GRID-POWER",sprintf("%.3f",$demand/1000), "demand",FALSE);
dial(160,60,"Frequency ".$freq."Hz","50HZ",sprintf("%.3f",$freq),"frequency",FALSE);
dial(700,60,"CCGT ".$ccgt."GW","CCGT-POWER",$ccgt,"ccgt",TRUE);
dial(880,60,"Wind ".$wind."GW","BIG-WIND",$wind,"wind",TRUE);
dial(340,60,"Coal ".$coal."GW","COAL-POWER",$coal,"coal",TRUE);
dial(520,60,"Nuclear ".$nuclear."GW","NUCLEAR-POWER",$nuclear, "nuclear",TRUE);

//Intercnnectors
small_dial(1100,154,"French ICT ".$french."GW","FRENCH-POWER",$french,"frenchict");
small_dial(1195,154,"Dutch ICT ".$dutch."GW","DUTCH-POWER",$dutch,"dutchict");
small_dial(1290,154,"Irish ICT ".$irish."GW","IRISH-POWER",$irish,"irishict");
small_dial(1384,154,"E-W ICT ".$ew."GW","EW-POWER",$ew, "ewict");

// auxiliaries
small_dial(1060,4,"Pumped ".$pumped."GW","PUMPED-POWER",$pumped,"pumped");
small_dial(1150,4,"Hydro ".$hydro."GW","HYDRO-POWER",$hydro,"hydro");
small_dial(1240,4,"Biomass ".$other."GW","OTHER-POWER",$other,"biomass");
small_dial(1330,4,"Oil ".$oil."GW","OIL-POWER",$oil,"oil");
small_dial(1420,4,"OCGT ".$ocgt."GW","OCGT-POWER",$ocgt,"ocgt");
do_graph(10,290,"demand-day.png","Daily Demand (GW)");
do_graph(10,420,"demand-week.png","Weekly Demand (GW)");
do_graph(10,550,"demand-month.png","Monthly Demand (GW)");
do_graph(10,680,"demand-year.png","Yearly Demand (GW)");
do_graph(380,290,"coal-nuke-ccgt-day.png","Daily Nuclear/Coal/CCGT (GW)");
do_graph(380,420,"coal-nuke-ccgt-week.png","Weekly Nuclear/Coal/CCGT (GW)");
do_graph(380,550,"coal-nuke-ccgt-month.png","Monthly Nuclear/Coal/CCGT (GW)");
do_graph(380,680,"coal-nuke-ccgt-year.png","Yearly Nuclear/Coal/CCGT (GW)");
do_graph(750,290,"wind-hydro-pumped-day.png","Daily Hydro/Pumped/Wind/Bio. (GW)");
do_graph(750,420,"wind-hydro-pumped-week.png","Weekly Hydro/Pumped/Wind/Bio. (GW)");
do_graph(750,550,"wind-hydro-pumped-month.png","Monthly Hydro/Pumped/Wind/Bio. (GW)");
do_graph(750,680,"wind-hydro-pumped-year.png","Yearly Hydro/Pumped/Wind/Bio.(GW)");
do_graph(1120,290,"ict-day.png","Daily French/Irish/Dutch/EW ICT (GW)");
do_graph(1120,420,"ict-week.png","Weekly French/Irish/Dutch/EW ICT (GW)");
do_graph(1120,550,"ict-month.png","Monthly French/Irish/Dutch/EW ICT (GW)");
do_graph(1120,680,"ict-year.png","Yearly French/Irish/Dutch/EW ICT (GW)");
?>	
<div style="position: absolute;top:810px;left:200px; height: 15px; width: 1250px; text-align: center"><b>Data last recorded on 
<? echo $last_update_time;?></b>
</div>
<div class="bezel" style="position: absolute;top:22px;left:10px; width: 190px; text-align: right">
Download data sets (CSV):</div>
<?
web_button(200,20,"Download", "Download Data","download.php");
web_button(320,20,"Links", "Download Data","/links.html");
web_button(440,20,"Info", "Download Data","/about.html");
?>
</body> 
</html>
<?

//  modify to display background plus moving needle.

function small_dial($x,$y,$legend,$type,$val,$tooltip)
{
	global $demand;	
	$legend1=sprintf("%s<br>(%0.2f%%)",$legend,$val*100000/$demand);$dial=sprintf("bitmaps/dial-%s-125.png",$type);
	$_GET['type']=$type;
	include('gauges.php');
	$needlepercentage=round((($val-$min)/($max-$min))*100);
	if($needlepercentage<0) $needlepercentage=0;
	else if ($needlepercentage>100) $needlepercentage=100;
	$url=sprintf("bitmaps/needle-125-%d",$needlepercentage);
	tooltip($x+62,$y+52,$tooltip);
	?>
	<div style="position: absolute;top:<?echo $y?>px;left:<? echo $x?>px; width: 125px; height: 125px; background-image: url(<? echo $dial?>)" class="bezel" onmouseover="show('<?echo $tooltip?>')" onmouseout="hide('<?echo $tooltip?>')" >
	<img src="<?echo $url?>" alt="<? echo $legend?>" align="middle">
	</div>
	<div style="position: absolute;top:<?echo $y+88?>px;left:<? echo $x?>px; width: 125px; height: 25px;" class="small_legend">
	<center><b><? echo $legend1 ?></b></center>
	</div>
	<?
}

function dial($x,$y,$legend,$type,$val,$tooltip,$nopercent)
{
	global $demand;	
	$legend1=sprintf("%s<br>(%0.2f%%)",$legend,$val*100000/$demand);
	$dial=sprintf("bitmaps/dial-%s-250.png",$type);
	$_GET['type']=$type;
	include('gauges.php');
	$needlepercentage=round((($val-$min)/($max-$min))*100);
	if($needlepercentage<0) $needlepercentage=0;
	else if ($needlepercentage>100) $needlepercentage=100;
	$url=sprintf("bitmaps/needle-250-%d",$needlepercentage);
	tooltip($x+124,$y+144,$tooltip);
	?>
	<div style="position: absolute;top:<?echo $y?>px;left:<? echo $x?>px; width: 250px; height: 250px; background-image: url(<? echo $dial?>)" class="bezel" onmouseover="show('<?echo $tooltip?>')" onmouseout="hide('<?echo $tooltip?>')">
	<img src="<?echo $url?>" alt="<? echo $legend?>" align="middle">
	
	</div>
	<div style="position: absolute;top:<?echo $y+180?>px;left:<? echo $x?>px; width: 250px; height: 50px;" class="medium_legend">
	<center><b><? echo ($nopercent? $legend1:$legend) ?></b></center>
	</div>
	<?
	
}

function do_graph($x,$y, $image, $title)
{
	$url=$image;
	?>
	<div style="position: absolute;top:<? echo $y+12?>px;left:<? echo $x+1?>px; width: 370px; height: 12px;text-align:center; font-weight: bold" >
	<IMG src="<? echo $url ?> " alt="<?echo $title?>"></div>
	<div style="position: absolute;top:<? echo $y?>px;left:<? echo $x?>px; width: 370px; height: 12px;text-align:center; font-weight: bold" >
	<IMG src="bitmaps/graph-bezel.gif"  alt="bezel"> </div>
	<div class="bezel" style="position: absolute;top:<? echo $y+5?>px;left:<? echo $x?>px; width: 365px; height: 12px;text-align:center;  " >
	<? echo $title?></div>
	
	<?
}
function web_button($x,$y,$label, $alt,$url)
{
?>
<div class="button" onmouseover="this.className='hot-button'"  onmouseout="this.className='button'" style="Position: absolute; top:<?echo $y;?>px; left:<?echo $x;?>px; height: 20px; width: 150px; text-align:center;">
<a class="mybutton" href="<?echo $url?>"><? echo $label?></a>
</div>
<?
}
?>

