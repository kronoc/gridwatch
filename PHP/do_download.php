<?php

// open databse connection
$con=mysqli_connect("localhost","username","password","gridwatch");

// Check connection
if (mysqli_connect_errno($con))
  {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
  }
// set up the mysql query and header line
$query="id, timestamp";
if ($_POST['demand']=='on')
	$query.=', demand';
if ($_POST['frequency']=='on')
	$query.=', frequency';	
if ($_POST['coal']=='on')
	$query.=', coal';	
if ($_POST['nuclear']=='on')
	$query.=', nuclear';	
if ($_POST['ccgt']=='on')
	$query.=', ccgt';	
if ($_POST['wind']=='on')
	$query.=', wind';	
if ($_POST['french_ict']=='on')
	$query.=', french_ict';	
if ($_POST['dutch_ict']=='on')
	$query.=', dutch_ict';	
if ($_POST['irish_ict']=='on')
	$query.=', irish_ict';	
if ($_POST['ew_ict']=='on')
	$query.=', ew_ict';	
if ($_POST['pumped']=='on')
	$query.=', pumped';	
if ($_POST['hydro']=='on')
	$query.=', hydro';	
if ($_POST['oil']=='on')
	$query.=', oil';
if ($_POST['ocgt']=='on')
	$query.=', ocgt';
if ($_POST['other']=='on')
	$query.=', other';
// set up a header string as well
$hstr=$query."\r\n";  // needs a little patching
// add in a tail
// from day where timestamp <= '2012-01-31 00:00:00' and timestamp >= '2012-01-00 00:30:00' order by id 
$query= sprintf("select %s from day where timestamp <= '%d-%02d-%02d %02d:%02d:00' and timestamp >= '%d-%02d-%02d %02d:%02d:00' order by id",
	$query,
	$_POST['endyear'],$_POST['endmonth']+1,$_POST['endday'],$_POST['endhour'],$_POST['endminute'],
	$_POST['startyear'],$_POST['startmonth']+1,$_POST['startday'],$_POST['starthour'],$_POST['startminute']);
// set up headers
header("Content-Disposition: attachment; filename=\"gridwatch.csv\"");
header("Content-Type: text/csv");
echo ($hstr);	
$result=mysqli_query($con, $query);
while ($row = mysqli_fetch_array($result, MYSQLI_NUM)) 
	{
	$fields=count($row);
	for($i=0;$i<$fields;$i++)
		{
		if($i!=($fields-1))
			printf("%s, ",$row[$i]);
		else 
			printf("%s\r\n",$row[$i]);
		}
	}
?>
