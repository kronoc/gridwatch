<?php
?>
<script type="text/javascript">
function hide(x)
	{
	document.getElementById(x).style.display='none';
	}
function show(x)
	{
	document.getElementById(x).style.display='inline';
	}
</script>
<?
// our tooltips
$tooltips= array (
	"demand"=> array("Demand", "This is the total demand of the entire country (plus or minus exports) <b>less</b> any unmetered generating sources like wind (about half as much as is shown on the meter) and solar installations (average about 0.25GW, mostly in summer). At all times supply must exactly match demand. There is no storage in the grid itself."),
	"frequency"=> array("Frequency", "Grid frequency is controlled to be exactly 50Hz on <i>average</i>, but varies slightly. A lower frequency corresponds to a higher potential demand than actual generating capacity: by allowing the frequency and voltage to go lower, the demand is reduced slightly to keep the balance, and <i>vice versa.</i>"),
	"coal"=> array("Coal", "Coal is the largest contributor to the UK grid - especially so in recent years as gas prices have risen. Some coal plants like Drax, also co-fire biomass with coal as well, which allows them to gain access to renewable subsidies. Coal plants are now restricted in running hours for emissions, so tend to run in winter when prices are higher."),
	"nuclear"=> array("Nuclear", "Currently the UK has one old Magnox plant still operating, seven AGR designs and one relatively modern PWR. Nuclear power stations are run flat-out to maximise income. Since the cost of fuel is almost insignificant, it pays them to sell at any price they can get. Variations in output are generally signs that refuelling or maintenance is ongoing."),
	"ccgt"=> array("CCGT", "Combined Cycle Gas Turbines are gas turbines whose hot exhausts are used to drive a boiler and steam turbine. This two stage process makes them very efficient in gas usage. They are also quite fast to get online - less than an hour in general, so they are used to cover (profitable) peak demand and to balance wind output."),
	"wind"=> array("Wind", "This is the total contributed by <i>metered</i> wind farms. Wind power contributes about another 50% from <i>embedded</i> (or unmetered) wind turbines that shows only as a drop in demand. Wind like nuclear, will sell into any market price because turbines are expensive, wind is not and subsidies are always paid. The variability of wind leads to very high fluctuations in output."),
	"pumped"=> array("Pumped Storage", "These are small hydro-electric stations that can use overnight electricity to recharge their reservoirs. Mainly used to meet very short term peak demands (the water soon runs out). They represent the nearest thing to 'storage' that is attached to the grid."),
	"hydro"=> array("Hydroelectric power", "The UK has no major hydroelectric power stations, but a collection of smaller ones, mainly in Scotland, that provide very useful power (if it's rained recently!). There would be a little more, but many stations deliberately reduce output to get the best renewable subsidy rates."),
	"biomass"=> array("Biomass", "These power stations are either (parts of) old coal plants that have been converted to run on imported timber - e.g. Drax 2 and Ironbridge 1 & 2, thus enabling them to qualify as 'renewable' and gain subsidies thereby, or purpose built biomass burners like Stevens Croft (40MW)  built to use sawmill waste."),
	"oil"=> array("Oil", "These are stations running thick <b>fuel oil</b> or <b>bunker oil</b> Due to the price of this, they are not economic to run, but are held in reserve for potential peak winter demand. They are tested about once a year to see if they still function."),
	"ocgt"=> array("OCGT", "Open Cycle Gas Turbines, are gas turbines without steam plant to maximise their efficiency. They are cheap to build, but expensive to run, so are seldom used except in emergencies in winter, when very high market prices of electricity make them profitable."),
	"frenchict"=> array("French Interconnector", "This is a 2GW bi-directional link to France which (when fully operational, which is seldom) is able to import up to 2GW of power from France - usually in summer when France has a nuclear power surplus - and export in winter, when the UK's excess of backup plant  and coal  power can be profitably sold to meet continental shortfalls."),
	"dutchict"=> array("BritNed Interconnector", "This is 1GW connector to Holland Its usage seems to reflect a surplus or a deficit of NW europe wind energy."),
	"irishict"=> array("Moyle interconector.", "This is a 500MW (0.5GW) bi-directional link from Scotland to N Ireland, currently limited to 250MW pending a new cable being laid (2019). When it is working it is mostly used to top up the Irish grid, only when the wind blows a gale does it sometimes supply the UK mainland."),
	"ewict"=> array("East-West Interconnector", "This is a new 500MW (0.5GW) bi-directional link between Wales and the Irish Republic, enabling access to the UK (and continental) grid, and prices, for the Irish consumers. In general it feeds one way more or less in step with the Moyle interconnector. ")
	);
	
function tooltip($x,$y,$name)
	{
		global $tooltips;
	// is this a left, right or center tooltip?
	// we have 1250 pixels to play with so split into thirds
	if($x<(1500/3) )
		{
		$img="/bitmaps/tipleft.png";
		$x1=$x-15;
		}
	else if ($x>(2*1500/3))
		{
		$img="/bitmaps/tipright.png";
		$x1=$x-275;
		}
	else 
		{
		$img="/bitmaps/tipcenter.png";
		$x1=$x-145;	
		}
	$y1=$y+50;
	//extract the title and the text
	foreach( $tooltips as $key=>$data)
		{
		if($key==$name) // OUR tooltip of the day
			{
			$title=$data[0];
			$text=$data[1];
			break;
			}
		}
	// draw main frame with background
	?>
	<div id="<?echo $name?>" style="position: absolute; top:<?echo $y1?>px;left: <?echo $x1 ?>px;width: 260px; height: 123px; overflow: none; background-image:url('<?echo $img?>'); padding-top: 45px; padding-left: 15px; padding-right: 25px; padding-bottom: 5px; color:#000000;font-size: 10px; z-index:1; display: none">
	<b> <?echo $title?>: </b> <?echo $text?>
	</div>
	<?
	
	}
?>
