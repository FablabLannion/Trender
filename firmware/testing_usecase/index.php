<?php
ob_start();

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
//$thedate="lala";
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


$api_key="XVBCFB8J63HS8G10";
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
