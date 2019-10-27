<?php
//IPP project 1 - parse.php
//Tomas Zigo xzigot00@stud.fit.vutbr.cz

/*Class checks for various syntax and lexical errors. Every function has 
string that need to be checked as $string2check parameter*/
class checker
{
	/*Function case-insensitively compares given instruction in
	$string2check with arrays containing valid instructions, if
	instruction is valid, function returns how many	parameters will be
	needed*/
	public static function check_instruction($string2check)
	{
		$instructions0 = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"];
		$instructions1 = ["DEFVAR", "CALL", "PUSHS", "POPS", "WRITE", "LABEL", "JUMP", "EXIT", "DPRINT"];
		$instructions2 = ["MOVE", "INT2CHAR", "READ", "STRLEN", "TYPE", "NOT"];
		$instructions3 = ["ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"];
		for ($index=0; $index < count($instructions0); $index++) { 
			if (strcasecmp($instructions0[$index], $string2check) == 0) {
				return 0;	
			}
		}
		for ($index=0; $index < count($instructions1); $index++) { 
			if (strcasecmp($instructions1[$index], $string2check) == 0) {
				return 1;	
			}
		}
		for ($index=0; $index < count($instructions2); $index++) { 
			if (strcasecmp($instructions2[$index], $string2check) == 0) {
				return 2;	
			}
		}
		for ($index=0; $index < count($instructions3); $index++) { 
			if (strcasecmp($instructions3[$index], $string2check) == 0) {
				return 3;	
			}
		}
		return -1;
	}

	//Case-sensitive check for var frames, returns true if string is frame
	public static function check_var($string2check)
	{
		$frames = ["GF", "TF", "LF"];
		for ($i=0; $i < 3; $i++) { 
			if ($string2check === $frames[$i]) {
				return TRUE;
			}
		}
		return FALSE;
	}

	//Function compare every character with rules for labels in IPPcode19, returns true if label is valid
	public static function check_label($string2check)
	{
		$actual_char = $string2check[0];
		if ($actual_char === '_' || $actual_char === '-' || $actual_char === '$' || $actual_char === '&' || $actual_char === '%' || $actual_char === '*' || $actual_char === '!' || $actual_char === '?' || ctype_alpha($actual_char)) {
			for ($i=1; $i < strlen($string2check); $i++) { 
				$actual_char = $string2check[$i];
				if (!($actual_char === '_') && !($actual_char === '-') && !($actual_char === '$') && !($actual_char === '&') && !($actual_char === '%') && !($actual_char === '*') && !($actual_char === '!') && !($actual_char === '?') && !(ctype_alnum($actual_char))) {
					return FALSE;
				}
			}
			return TRUE;
		}
		else
			return FALSE;
	}	

	//Case-sensitive check for given data type 
	public static function check_symb($string2check)
	{
		$data_types = ["nil", "int", "bool", "string"];
		for ($i=0; $i < 4; $i++) { 
			if ($string2check === $data_types[$i]) {
				return TRUE;
			}
		}
		return FALSE;
	}

	/*Function validates rightness of data types with values entered. Value
	is stored in $string2check parameter and data type in $type*/
	public static function check_type($string2check, $type)
	{
		switch ($type) {
			case 'bool':
				if ($string2check === 'true' || $string2check === 'false') {
					return TRUE;
				}
				else
					return FALSE;
				break;
			case 'int':
				if (ctype_digit(ltrim((string)$string2check, '-+')))
					return TRUE;
				else if ($string2check[0] === "\n") {
					return TRUE;
				}
				else
					return FALSE;
				break;
			case 'string':
				for ($i = 0; $i < strlen($string2check); $i++) {
					$actual_char = $string2check[$i];
					if ($actual_char === '\\') {
						$i++;
						for ($index=$i; ($index < $i+3) && $index < strlen($string2check); $index++){
							if (!ctype_digit($string2check[$index])) {
								return FALSE;
							}
						}
						$i = $i+2;
						continue;
					}
				}
				return TRUE;
				break;
			case 'nil':
				if ($string2check === 'nil') {
					return TRUE;
				}
				else
					return FALSE;
				break;
			default:
				# cod
				return FALSE;
				break;
		}
	}
}

//Function prints help message on stdout
function help_message()
{
	echo "Skript typu ﬁltr načte ze standardního vstupu zdrojový kód v IPPcode19, zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní výstup XML reprezentaci programu dle speciﬁkace.\n";
}

/*Runs whole program functions like, checks parameters, gets data from stdin, lexical and syntax analysis.
If error occure exception is catched and error message is printed on stderr with appropriate error code*/
try {
	$filename=NULL;
	if ($argc > 1) 
		arguments::arg_check($argv, $argc);
	$input_code = stream_get_contents(STDIN);
	$statistics = parse::do_parse($input_code);
	if ($filename != NULL) {
		arguments::write_stats($statistics, $argv, $argc);
	}
} catch (Exception $e) {
	fwrite(STDERR, 'Exit code: ['.$e->getCode()."] ".$e->getMessage()."\n");
	die($e->getCode());
}

/*Class performs actions with arguments and checks their correctness. $argv and $argc are given as parameters to every function in this class*/
class arguments
{
	//Controls number of arguments if --help is set and call function to print help message
	public static function arg_work($argv, $argc)
	{
		if ($argc == 2) {
			if ($argv[1] == '--help') {
				help_message();
				exit(0);
			}
			else
				throw new Exception("Bad argument!", 10);
		}
		elseif ($argc > 2) {
			throw new Exception("Too much arguments!", 10);		
		}
	}

	/*Function checks which arguments are set and performs actions to
	check their validity. Function uses $filename as global variable
	and stores there name or path to file where extension data will
	be written*/
	public static function arg_check($argv, $argc)
	{
		$shortops = "";
		$longtops = array("help", "stats:", "loc", "comments", "labels", "jumps");
		$loaded_arguments = getopt($shortops, $longtops);
		if (isset($loaded_arguments["help"])) {
			self::arg_work($argv, $argc);
		}
		elseif (isset($loaded_arguments["stats"])) {
			global $filename;
			$filename = $loaded_arguments["stats"];
			$longtops = preg_filter('/^/', '--', $longtops);
			if (in_array($filename, $longtops)) {
				throw new Exception("File not set!", 10);
			}
			if (count($loaded_arguments)==1) {
				throw new Exception("No parameter set!", 10);
			}
			$arg_count = 0;
			foreach ($loaded_arguments as $value) {
				if (is_countable($value)) {
					$arg_count+=count($value);
				}
				else
					$arg_count++;
			}
			if ($arg_count != $argc-1) {
				throw new Exception("Bad arguments!", 10);
			}
		}
		else {
			throw new Exception("Bad arguments!", 10);	
		}
	}

	/*Prepares file for writing. Writes data needed from lexical and syntax analysis to file, according to program arguments. Parameter $stats is array with statistics data from analyzer*/
	public static function write_stats($stats, $argv, $argc)
	{
		global $filename;
		$file=fopen($filename, "w");
		if ($file) {
			foreach ($argv as $key => $value) {
				if ($value === "--loc") {
					fwrite($file, $stats["loc"]."\n");
				}
				if ($value === "--comments") {
					fwrite($file, $stats["comments"]."\n");
				}
				if ($value === "--labels") {
					fwrite($file, count($stats["labels"])."\n");
				}
				if ($value === "--jumps") {
					fwrite($file, $stats["jumps"]."\n");
				}
			}
			fclose($file);
		}
		else
			throw new Exception("Error opening file!", 12);		
	}
}

/*Class execute lexical and syntax analysis and performs all generating
to xml code which is stored in $xml*/
class parse
{
	/*Generates start of instruction in $inst and returns number of loaded
	instructions*/
	public static function xml_arg_start($xml, $loaded_instructions, $inst)
	{
		xmlwriter_start_element($xml, 'instruction');
		xmlwriter_start_attribute($xml, 'order');
		xmlwriter_text($xml, $loaded_instructions);
		$loaded_instructions++;
		xmlwriter_end_attribute($xml);
		xmlwriter_start_attribute($xml, 'opcode');
		xmlwriter_text($xml, strtoupper($inst));
		xmlwriter_end_attribute($xml);
		return $loaded_instructions;
	}

	/*Checks for symbol parameter validity and generates
	xml text for symbols. In case of variable, calls function for variables.
	Returns index where check ended. $code_words contains data from
	stdin*/
	public static function xml_symb($xml, $code_words, $index)
	{
		if (checker::check_symb($code_words[$index])) {
			xmlwriter_text($xml, $code_words[$index]);
			$data_type = strtolower($code_words[$index]);
			xmlwriter_end_attribute($xml);
			$index++;
			if (($index == count($code_words)-1) || ($code_words[$index] != '@')) {
				throw new Exception("Lexical error!", 23);
			}
			$index++;
			if ($index != count($code_words)-1) {
				if (!checker::check_type($code_words[$index], $data_type)) {
					throw new Exception("Lexical error!", 23);
				}
				if ($code_words[$index] != "\n") {
					xmlwriter_text($xml, $code_words[$index]);
				}
			}
			elseif ($data_type === 'bool' || $data_type === 'nil') {
				throw new Exception("Lexical error!", 23);
			}
		}
		else
			$index = self::xml_var($xml, $code_words, $index);
		if ($code_words[$index] === "\n") {
			return $index-1;
		}
		return $index;
	}

	/*Generates variables xml code. Also executes all checks
	needed for var. $code_words should be controlled from this array at $index. Returns index where check ended*/
	public static function xml_var($xml, $code_words, $index)
	{
		if (checker::check_var($code_words[$index])) {
			xmlwriter_text($xml, 'var');
			xmlwriter_end_attribute($xml);
			xmlwriter_text($xml, $code_words[$index]);
			$index++;
			if (($index == count($code_words)-1) || ($code_words[$index] != '@')) {
				throw new Exception("Lexical error!", 23);
			}
			$index++;
			if ($index == count($code_words)-1) {
				throw new Exception("Lexical error!", 23);
			}
			xmlwriter_text($xml, '@');
			if (checker::check_label($code_words[$index])) {
				xmlwriter_text($xml, $code_words[$index]);
			}
			else
				throw new Exception("Lexical error!", 23);
		}
		else
			throw new Exception("Lexical error!", 23);
		if ($code_words[$index] === "\n") {
			return $index-1;
		}
		return $index;
	}

	/*Controls all parsing processes. Edits data from stdin given as 
	$input_code parameter, with regular expression, prepares all data in simple one dimensional array $code_words. 
	Generates xml document to $xml and prints it at the end. Returns array with analysis data needed for project extension work*/
	public static function do_parse($input_code)
	{
		if ($input_code == NULL || !(preg_match('/[[:alnum:]]/', $input_code))) {
			throw new Exception("Missing header!", 21);
		}
		$used_labels = array();
		$jumps = 0;
		$comments = preg_match_all('/#.*\n/', $input_code);

		//split as words with regular expression
		$code_words  = preg_split('/[\h]+|#.*+|([\n]+|[@]+|string@+[^#\s]*)/', $input_code, -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_DELIM_CAPTURE);
		array_push($code_words, "\n");
		$no_enter = FALSE;

		//deletes duplicit \n and split string data type with its value
		while (!$no_enter) {
			$i=0;
			for ($k=1; $k < count($code_words);$k++) {
				if (strpos($code_words[$k], 'string@') !== false) 
					array_splice($code_words, $k,1, array('string', '@', substr($code_words[$k],7)));
				if ($code_words[$k-1][0] == "\n" && $code_words[$k][0] == "\n") {
					unset($code_words[$k-1]);
					$code_words = array_values($code_words);
					$i++;
				}
				if ($i == 0 && $k == count($code_words)-1) {
					$no_enter = TRUE;
				}
			}			
		}
		//generates xml header
		$xml = xmlwriter_open_memory();
		xmlwriter_set_indent($xml, 1);
		xmlwriter_start_document($xml, '1.0', 'UTF-8');
		xmlwriter_start_element($xml, 'program');
		xmlwriter_start_attribute($xml, 'language');
		xmlwriter_text($xml, 'IPPcode19');
		xmlwriter_end_attribute($xml);
		if ($code_words==NULL ||(strcasecmp($code_words[0], ".IPPcode19")!=0) || (count($code_words) == 1 || $code_words[1][0] != "\n")) {
			throw new Exception("Missing or wrong header!", 21);
		}
		$loaded_instructions = 1;

		//checks every line of stdin and generates xml code for all instructions
		for ($i=1; $i < count($code_words); $i++) {
			if ($code_words[$i][0] != "\n") {
				throw new Exception("Lexical error!", 23);	
			} 
			if ($i == count($code_words)-1) {
				break;
			}
			$i++;
			$param_no = checker::check_instruction($code_words[$i]);

			//decides how many parameters are needed for entered instruction
			switch ($param_no) {
				case 0:
					$loaded_instructions = self::xml_arg_start($xml, $loaded_instructions, $code_words[$i]);
					xmlwriter_end_element($xml);
					break;
				case 1:
					$loaded_instructions = self::xml_arg_start($xml, $loaded_instructions, $code_words[$i]);	
					$instruct = strtoupper($code_words[$i]);
					$i++;
					if($i == count($code_words))
						throw new Exception("Lexical error!", 22);
					xmlwriter_start_element($xml, 'arg1');
					xmlwriter_start_attribute($xml, 'type');
					if ($instruct === 'DEFVAR' || $instruct === 'POPS') {
						#var needed
						$i = self::xml_var($xml, $code_words, $i);
					}
					elseif ($instruct === 'CALL' || $instruct === 'LABEL' || $instruct === 'JUMP') {
						if (checker::check_label($code_words[$i])) {
							xmlwriter_text($xml, 'label');
							xmlwriter_end_attribute($xml);
							xmlwriter_text($xml, $code_words[$i]);
							if ($instruct === 'LABEL') 
								array_push($used_labels, $code_words[$i]);
							if ($instruct === 'JUMP')
								$jumps++;
						}
						else
							throw new Exception("Lexical error!", 23);
					}
					else {
						$i = self::xml_symb($xml, $code_words, $i);
					}
					xmlwriter_end_element($xml);
					xmlwriter_end_element($xml);
					break;
				case 2:
					$loaded_instructions = self::xml_arg_start($xml, $loaded_instructions, $code_words[$i]);
					$instruct = strtoupper($code_words[$i]);
					$i++;
					if ($i == count($code_words)) {
						throw new Exception("Lexical error!", 22);	
					}
					xmlwriter_start_element($xml, 'arg1');
					xmlwriter_start_attribute($xml, 'type');
					$i = self::xml_var($xml, $code_words, $i);
					xmlwriter_end_element($xml);
					$i++;
					if ($i == count($code_words)) {
						throw new Exception("Lexical error!", 23);	
					}
					xmlwriter_start_element($xml, 'arg2');
					xmlwriter_start_attribute($xml, 'type');
					if ($instruct === 'READ') {
						if (checker::check_symb($code_words[$i]) && $code_words[$i] != 'nil') {
							xmlwriter_text($xml, 'type');
							xmlwriter_end_attribute($xml);
							xmlwriter_text($xml, $code_words[$i]);
						}
						else
							throw new Exception("Lexical error!", 23);	
					}
					else {
						$i = self::xml_symb($xml, $code_words, $i);
					}
					xmlwriter_end_element($xml);									
					xmlwriter_end_element($xml);
					break;
				case 3:
					$loaded_instructions = self::xml_arg_start($xml, $loaded_instructions, $code_words[$i]);
					$instruct = strtoupper($code_words[$i]);
					$i++;
					if ($i == count($code_words)) {
						throw new Exception("Lexical error!", 22);	
					}
					xmlwriter_start_element($xml, 'arg1');
					xmlwriter_start_attribute($xml, 'type');
					if ($instruct === 'JUMPIFEQ' || $instruct === 'JUMPIFNEQ') {
						if (checker::check_label($code_words[$i])) {
							xmlwriter_text($xml, 'label');
							xmlwriter_end_attribute($xml);
							xmlwriter_text($xml, $code_words[$i]);
							xmlwriter_end_element($xml);
							$jumps++;
						}
						else
							throw new Exception("Lexical error!", 23);
					}
					else {
						$i = self::xml_var($xml, $code_words, $i);
						xmlwriter_end_element($xml);
					}
					for ($cycle=2; $cycle < 4; $cycle++) { 
						$i++;
						if ($i == count($code_words)) {
							throw new Exception("Lexical error!", 23);	
						}
						xmlwriter_start_element($xml, 'arg'.$cycle);
						xmlwriter_start_attribute($xml, 'type');
						$i = self::xml_symb($xml, $code_words, $i);
						xmlwriter_end_element($xml);
					}									
					xmlwriter_end_element($xml);
					break;
				default:
					throw new Exception("Lexical error!", 22);	
					break;
			}
			if ($i == count($code_words)-1) {
				if ($param_no == 0) {
					throw new Exception("Lexical error!", 22);
				}
			}
		}
		//generates end of xml code
		$labels = array_unique($used_labels);
		xmlwriter_end_element($xml);
		xmlwriter_end_document($xml);
		echo xmlwriter_output_memory($xml);	
		$return_data = array("loc"=>$loaded_instructions-1, "comments"=>$comments, "labels"=>$labels, "jumps"=>$jumps);
		return $return_data;
	}
}
?>
