<?php

if(!isset($_GET['sign'])){
        exit(0);
}

function celsiusToFahrenheit(float $celsius) : float
{
    return $celsius * 1.8 + 32;
}

$dt = new DateTime("now", new DateTimeZone('America/New_York'));
$out = $dt->format('m-d h:iA');
$outTime = substr($out, 0, strlen($out)-1);

$weather_json_str = file_get_contents("nyc_weather_dump");
$weather_json_all = json_decode($weather_json_str);
$weather_json_today = $weather_json_all->consolidated_weather[0];

$expected_conditions = $weather_json_today->weather_state_abbr;
$maxF = round(celsiusToFahrenheit($weather_json_today->max_temp));
$minF = round(celsiusToFahrenheit($weather_json_today->min_temp));
$F = round(celsiusToFahrenheit($weather_json_today->the_temp));
$outWeather = "$expected_conditions $minF $F $maxF";

$user = $_GET['sign'];
if($user=="AG1"){
        echo "$outTime\n";
        echo "5 5\n";
        echo "$outWeather\n";
        echo "6 5 7 6\n";
	//some sample data for the second display because my system uses a mysql database meant for another project
        echo "39AV N 3-7";
        echo "\n6 7 5\n";
        echo "36ST R 4-8";
        echo "\n6 7 5";
}
