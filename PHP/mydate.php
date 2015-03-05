<?php

function mydate($filetime)
{
$datestrings=array(
1=>"1st.",
2=>"2nd.",
3=>"3rd.",
4=>"4th.",
5=>"5th.",
6=>"6th.",
7=>"7th.",
8=>"8th.",
9=>"9th.",
10=>"10th.",
11=>"11th.",
12=>"12th.",
13=>"13th.",
14=>"14th.",
15=>"15th.",
16=>"16th.",
17=>"17th.",
18=>"18th.",
19=>"19th.",
20=>"20th.",
21=>"21st.",
22=>"22nd.",
23=>"23rd.",
24=>"24th.",
25=>"25th.",
26=>"26th.",
27=>"27th.",
28=>"28th.",
29=>"29th.",
30=>"30th.",
31=>"31st.");

date_default_timezone_set('Europe/London');
$weekday=date('l',$filetime);
$day=$datestrings[date('j',$filetime)];
$month=date('F',$filetime);
$year=date('Y',$filetime);
$time24=date('H:i',$filetime);
$zone=(date('I')? "BST":"GMT");
$datestring=sprintf("%s the %s of %s, %s at %s %s",$weekday,$day,$month,$year,$time24,$zone);
return $datestring;
}
?>