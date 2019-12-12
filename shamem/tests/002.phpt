--TEST--
shamem_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('shamem')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = shamem_test1();

var_dump($ret);
?>
--EXPECT--
The extension shamem is loaded and working!
NULL
