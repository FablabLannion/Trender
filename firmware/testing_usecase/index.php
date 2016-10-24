<?php
/**
 *  written by Julien JACQUES - Fablab de Lannion
 *
 *  author: jjacques@legtux.org
 *  date  : 19-oct-2016
 *  desc  : simulate a CI tool (such as Jenkins)
 *          and send the test result value to a ThingSpeak.com's channel
 *          identified by its write api_key (must be given below before run this script !)
 *  usage : index.php?wait=30&maxi=2
 *            wait : time (in seconds) between two sendings to thingspeak.com
 *            maxi : total values sent before script ending
 *  notes : 
 */
ob_start();

/*
 * TODO - Enter the ThingSpeak Write API Key between the "" below:
 */
$api_key="";


$t_wait=30;
//print_r($_GET);
if(! empty($_GET['wait'])) {
	$t_wait=$_GET['wait'];
}
$max_iter=2;
if(! empty($_GET['maxi'])) {
	$max_iter=$_GET['maxi'];
}
$thedate=date("D, d M Y H:i:s");

// Generage HTML page here
//generate_full_html_page();
echo "<h1>Hello, I'm your Test result generator !</h1>";
echo "<p>Run starts at $thedate </p>";
echo "<p>Values are sent every $t_wait seconds during $max_iter iterations</p>";

// All magic goes here
$output = ob_get_clean();
ignore_user_abort(true);
set_time_limit(0);

header("Connection: close");
header("Content-Length: ".strlen($output));
header("Content-Encoding: none");

echo $output.str_repeat(' ', 10000) ."\n\n\n";
flush();



$opts = array(
    'http' => array(
        'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
        'method'  => 'GET' // 'POST',
//        'content' => http_build_query($data)
    )
);
$context = stream_context_create($opts);

//while(1) {
for($i=0;$i<$max_iter;$i++) {
	$value = rand(15,75);

	$url = "https://api.thingspeak.com/update?key=$api_key&field1=$value";
	$result = file_get_contents($url, false, $context);
	if ($result === FALSE) { 
		/* Handle error */ 
		echo "<p>error</p>";
	}
	// var_dump($result);

	sleep($t_wait);

}
?>
