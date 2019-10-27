<?php
	/** 
	* IPP parse script 
	* Author: xzigot00
	*/
	
	// Class checks arguments and prepares data
	class Arguments
	{
		public $recursive = FALSE;
		public $parse_only = FALSE;
		public $int_only = FALSE;
		public $directory;
		public $p_dir;
		public $i_dir;
		public $tests_count = 0;

		public function __construct($argc)
		{
			$longtops = array('help', 'directory:', 'recursive', 'parse-script:', 'int-script:', 'parse-only', 'int-only');
			$shortops = '';
			$loaded_arguments = getopt($shortops, $longtops);
			if (isset($loaded_arguments['help'])) {
				if ($argc == 2) {
					echo "Skript slouží pro automatické testování postupné aplikace parse.php a interpret.py. Skript projde zadaný adresář s testy a využije je pro automatické otestování správné funkčnosti obou předchozích programů včetně vygenerování přehledného souhrnu v HTML 5 do standardního výstupu.\n";
					exit(0);
				}
				else
					throw new Exception("Too much arguments for --help option!", 10);
			}
			if (isset($loaded_arguments['directory']))
				$this->directory = $loaded_arguments['directory'];
			else
				$this->directory = getcwd();

			if ($this->directory[-1] != '/')
				$this->directory .= '/';

			if (isset($loaded_arguments['recursive']))
				$this->recursive = TRUE;

			if (isset($loaded_arguments['parse-script'])) 
				$this->p_dir = $loaded_arguments['parse-script'];
			else
				$this->p_dir = getcwd().'/parse.php';
			
			if (isset($loaded_arguments['int-script'])) 
				$this->i_dir = $loaded_arguments['int-script'];
			else
				$this->i_dir = getcwd().'/interpret.py';

			if (isset($loaded_arguments['parse-only'])) {
				if (isset($loaded_arguments['int-only']) or isset($loaded_arguments['int-script'])) {
					throw new Exception("Wrong arguments combination!", 10);
				}
				$this->parse_only = TRUE;
			}
			
			if (isset($loaded_arguments['int-only'])) {
				if (isset($loaded_arguments['parse-only']) or isset($loaded_arguments['parse-script'])) {
					throw new Exception("Wrong arguments combination!", 10);
				}
				$this->int_only = TRUE;
			}
		}
	}

	//Runs tests, creates missing files and returns results in array
	function run_tests($arguments, $directory, $file)
	{
		global $tests_ok;
		global $tests_fail;
		global $test_out;
		global $tests_completion;
		$test_name = substr($file, 0, -4);
		$test_path = $directory.$test_name;
		if (!file_exists($test_path.'.in'))
			touch($test_path.'.in');
		if (!file_exists($test_path.'.out'))
			touch($test_path.'.out');
		if (!file_exists($test_path.'.rc')) {
			touch($test_path.'.rc');
			$f = fopen($test_path.'.rc', 'w');
			fwrite($f, '0');
			fclose($f);
		}
		//only parse
		if ($arguments->parse_only) {
			touch($test_path.'tmp1');
			$filename = $test_path.'tmp1';
			touch('./'.$test_name.'tmp2');
			touch('./'.$test_name.'tmp3');
			exec('php7.3 '.$arguments->p_dir.' <'.$directory.$file.' >'.$test_path.'tmp1', $out, $rv);
			$tmp = fopen('./'.$test_name.'tmp3', 'w');
			fwrite($tmp, $rv);
			fclose($tmp);
			unset($rv);
			exec('diff -w '.$test_path.'.rc ./'.$test_name.'tmp3', $out, $rv);
			if ($rv == 0) {
				$test_rc = TRUE;
				$tests_completion['rc_ok'] += 1;
			}
			else
				$test_rc = FALSE;
			$tests_completion['rc_all'] += 1;
			unset($rv);
			if (filesize($test_path.'.out') != 0 and filesize($filename) != 0) {
				exec('java -jar /pub/courses/ipp/jexamxml/jexamxml.jar '.$test_path.'.out '.$test_path.'tmp1', $out, $rv);
				if ($rv == 0) {
					$test_out = TRUE;
					$tests_completion['out_ok'] += 1;
				}
				else {
					$test_out = FALSE;
				}
			}
			elseif (filesize($test_path.'.out') == 0 and filesize($filename) == 0) {
				$test_out = TRUE;
				$tests_completion['out_ok'] += 1;
			}
			else
				$test_out = FALSE;	

			$tests_completion['out_all'] += 1;
			unlink($test_path.'tmp1');
			unlink('./'.$test_name.'tmp2');
			unlink('./'.$test_name.'tmp3');
			return(array($test_name, $test_rc, $test_out));
		}
		if ($arguments->int_only) {
			#only interpret
			touch('./'.$test_name.'tmp2');
			touch('./'.$test_name.'tmp3');
			exec('python3.6 '.$arguments->i_dir.' --source='.$directory.$file.' --input='.$test_path.'.in >./'.$test_name.'tmp2', $out, $rv);
			$tmp = fopen('./'.$test_name.'tmp3', 'w');
			fwrite($tmp, $rv);
			fclose($tmp);
			unset($rv);
			exec('diff -w '.$test_path.'.rc ./'.$test_name.'tmp3', $out, $rv);
			if ($rv == 0) {
				$test_rc = TRUE;
				$tests_completion['rc_ok'] += 1;
			}
			else
				$test_rc = FALSE;
			$tests_completion['rc_all'] += 1;
			unset($rv);
			exec('diff '.$test_path.'.out ./'.$test_name.'tmp2', $out, $rv);
			if ($rv == 0) {
				$test_out = TRUE;
				$tests_completion['out_ok'] += 1;
			}
			else
				$test_out = FALSE;
			$tests_completion['out_all'] += 1;
			unlink('./'.$test_name.'tmp3');
			unlink('./'.$test_name.'tmp2');
			return(array($test_name, $test_rc, $test_out));
		}
		else {
			touch('./'.$test_name.'tmp1');
			exec('php7.3 '.$arguments->p_dir.' <'.$directory.$file.' >./'.$test_name.'tmp1');
			touch('./'.$test_name.'tmp2');
			touch('./'.$test_name.'tmp3');
			exec('python3.6 '.$arguments->i_dir.' --source=./'.$test_name.'tmp1 --input='.$test_path.'.in >./'.$test_name.'tmp2', $out, $rv);
			$tmp = fopen('./'.$test_name.'tmp3', 'w');
			fwrite($tmp, $rv);
			fclose($tmp);
			unset($rv);
			exec('diff -w '.$test_path.'.rc ./'.$test_name.'tmp3', $out, $rv);
			if ($rv == 0) {
				$test_rc = TRUE;
				$tests_completion['rc_ok'] += 1;
			}
			else
				$test_rc = FALSE;
			$tests_completion['rc_all'] += 1;
			unset($rv);
			exec('diff '.$test_path.'.out ./'.$test_name.'tmp2', $out, $rv);
			if ($rv == 0) {
				$test_out = TRUE;
				$tests_completion['out_ok'] += 1;
			}
			else
				$test_out = FALSE;
			$tests_completion['out_all'] += 1;
			unlink('./'.$test_name.'tmp3');
			unlink('./'.$test_name.'tmp2');
			unlink('./'.$test_name.'tmp1');
			return(array($test_name, $test_rc, $test_out));
		}

	}

	//Find all files and run every test
	function find_files($arguments, $directory)
	{
		global $all_tests;
		$files_dirs = scandir($directory);
		$files_dirs = array_diff($files_dirs, array('.','..'));
		foreach ($files_dirs as $file) {
			if (is_dir($directory.$file)) {
				#it is directory
				if ($arguments->recursive) {
					find_files($arguments, $directory.$file.'/');
				}
			}
			else {
				#it is file
				if (preg_match('/^.*\.src$/', $file)) {
					$all_tests[$arguments->tests_count] = run_tests($arguments, $directory, $file);
					$arguments->tests_count += 1;
				}
			}
		}	
	}

	//Creates HTML code
	function create_html($data_array, $tests_completion)
	{
		global $arguments;
		if ($arguments->tests_count > 0) {
			$percentage = (($tests_completion['out_ok']+$tests_completion['rc_ok'])/($tests_completion['out_all']+$tests_completion['rc_all']))*100;
		}
		if ($percentage < 50) {
			$color = '#ff0000;';
		}
		else
			$color = '#008000';
		echo "<!DOCTYPE html>\n<html>\n<head>\n<title>Test</title>
</head>\n<body>";
		echo '<head>
		<title>Test</title>
		</head>
		<body>
			<header><h1 style="text-align: center;">Test results</h1></header>';
		echo '<h3>Test pass statistics:</h4>
			<ul>
			<li>Output tests: '.$tests_completion['out_ok'].'/'.$tests_completion['out_all'].'</li>
			<li>Return code tests: '.$tests_completion['rc_ok'].'/'.$tests_completion['rc_all'].'</li>
			<li>Pass percentage:<span style="color: '.$color.'">'.intval($percentage).'%</span></li>
			</ul>';
		echo '<table width="80%", align="center", border="1">
				<thead>
		   			<tr>
		      		<th>ID</th>
		      		<th>Test Name</th>
		      		<th>Output</th>
		      		<th>Return code</th>
		    		</tr>
		  		</thead>
		  		<tbody align="center">';
		$counter = 1;
		foreach ($data_array as $key) {
			echo "<tr>";
			echo "<td>".$counter."</td>";
			$counter += 1;
			echo "<td>".$key[0]."</td>";
			if ($key[2])
				echo "<td>&#10004;</td>";
			else
				echo "<td>&#10006;</td>";
			if ($key[1])
				echo "<td>&#10004;</td>";
			else
				echo "<td>&#10006;</td>";
		}
		echo "</tbody></table></body>\n</html>";
	}

	//Main function of test
	try {
		$tests_completion = array(
			'out_ok' => 0,
			'out_all' => 0,
			'rc_ok' => 0,
			'rc_all' => 0
		);
		$all_tests = array();
		//Get all arguments
		$arguments = new Arguments($argc);
		//Performs tests
		find_files($arguments, $arguments->directory);
		//Create HTML code
		create_html($all_tests, $tests_completion);
			
	} catch (Exception $e) {
		fwrite(STDERR, 'Exit code: ['.$e->getCode()."] ".$e->getMessage()."\n");
		die($e->getCode());
	}
?>
