<?php
if (isset($_GET['type']))
	$type=$_GET['type'];
// $value=$_GET['value'];
if (isset($_GET['legend']))
	$legend=$_GET['legend'];
	// pick up required size..60-600 suitable values. 600 is no scaling at all.
if (isset($_GET['scale']))
	$newsize=$_GET['scale'];
else $newsize=0;
$force_gap=0;
// $legend_color=$ivory; // may use $black if on a pale background;
//$legend_color=$black;
$legend_fontsize=30; // adjust for your style.
// catalogue of types. Add your own here.
switch($type)
	{
	case 'GRID-POWER':
		$min=25;
		$max=70;
		$warnlevel=58;
		$dangerlevel=60;
		$fontsize=24;
		$label="UK Demand (GW)";
		break;
	case 'CCGT-POWER':
		$min=0;
		$max=25;
		$warnlevel=10;
		$dangerlevel=5;
		$fontsize=24;
		$label="CCGT (GW)";
		break;
	case 'OCGT-POWER':
		$min=0;
		$max=5;
		$warnlevel=1;
		$dangerlevel=2;
		$fontsize=24;
		$label="OCGT (GW)";
		break;
	case 'COAL-POWER':
		$min=0;
		$max=25;
		$warnlevel=10;
		$dangerlevel=5;
		$fontsize=24;
		$label="Coal (GW)";
		break;
	case 'NUCLEAR-POWER':
		$min=0;
		$max=12;
		$warnlevel=8;
		$dangerlevel=5;
		$fontsize=24;
		$label="Nuclear (GW)";
		break;
	case 'HYDRO-POWER':
		$min=0;
		$max=1;
		$warnlevel=.1;
		$dangerlevel=.05;
		$fontsize=24;
		$label="Hydro (GW)";
		break;
	case 'PUMPED-POWER':
		$min=0;
		$max=2;
		$warnlevel=0;
		$dangerlevel=0;
		$fontsize=24;
		$label="Pumped Storage (GW)";
		break;
	case 'BIG-WIND':
		$min=0;
		$max=5.5;
		$warnlevel=.1;
		$dangerlevel=.05;
		$fontsize=24;
		$label="Wind (GW)";
		break;
	case 'WIND-POWER':
		$min=0;
		$max=3.5;
		$warnlevel=.1;
		$dangerlevel=.05;
		$fontsize=24;
		$label="Wind (GW)";
		break;
	case 'OTHER-POWER':
		$min=0;
		$max=1.5;
		$warnlevel=0;
		$dangerlevel=0;
		$fontsize=24;
		$label="Biomass (GW)";
		break;
	case 'FRENCH-POWER':
		$min=-2.5;
		$max=2.5;
		$warnlevel=2.5;
		$dangerlevel=2.6;
		$fontsize=24;
		$label="French ICT (GW)";
		break;
	case 'IRISH-POWER':
		$min=-1;
		$max=1;
		$warnlevel=1.1;
		$dangerlevel=1.2;
		$fontsize=24;
		$label="Irish ICT (GW)";
		break;
	case 'DUTCH-POWER':
		$min=-1;
		$max=1;
		$warnlevel=1.1;
		$dangerlevel=1.2;
		$fontsize=24;
		$label="Dutch ICT (GW)";
		break;
	case 'EW-POWER':
		$min=-1;
		$max=1;
		$warnlevel=1.1;
		$dangerlevel=1.2;
		$fontsize=24;
		$label="East-West ICT (GW)";
		break;
	case 'OIL-POWER':
		$min=0;
		$max=2.5;
		$warnlevel=0;
		$dangerlevel=0;
		$fontsize=24;
		$label="Oil (GW)";
		break;
	case '50HZ':
		$min=49.5;
		$max=50.5;
		$warnlevel=49.4;
		$dangerlevel=49.3;
		$fontsize=20;
		$label="UK GRID Frequency (Hz)";
		break;
	}
?>
