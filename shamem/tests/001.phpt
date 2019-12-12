--TEST--
Check if shamem is loaded
--SKIPIF--
<?php
if (!extension_loaded('shamem')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "shamem" is available';
?>
--EXPECT--
The extension "shamem" is available
