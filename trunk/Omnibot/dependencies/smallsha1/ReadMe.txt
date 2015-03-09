You only need sha1.h and sha1.cpp when including this in your own project.
main.cpp and 1000sha1.txt is only used for unittesting and benchmarking.

The 1000sha1.txt file was gennerated with a PHP script, to make sure that this implementation was correct.
This is the PHP script used:

<?php
$f=fopen("1000sha1.txt","w");
$str='';
for($i=0;$i<1000;$i++)
{
	$j=$i%10;
	$str=$str."$j";
	fwrite($f,sha1($str));
}
fclose($f);
echo strlen($str);
?>
