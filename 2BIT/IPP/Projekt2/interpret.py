#IPP project2 Interpret
#Author xzigot00

import xml.etree.cElementTree as ET 
import optparse
import sys
import os
import re

#Class that initializes variables
class Var:
	name = ''
	v_type = None
	value = None

	def __init__(self, arg):
		self.name = arg

#Class performs actions with symb types
class Symb:
	line_counter = 0

	#Checks if is initialized
	def check_init(type1, type2):
		if ((type1 == None or type1 == 'nil') or 
			(type2 == None or type2 == 'nil')):
				print("Error - non initialized variable!", file=sys.stderr)
				sys.exit(56)

	#Performs actions when Exit instruction is set
	def exit_inst(element):
		if (element.getchildren()[0].get('type') != 'int' and 
			element.getchildren()[0].get('type') != 'var'):
			print("Error - non valid symb value!", file=sys.stderr)
			sys.exit(53)
		if element.getchildren()[0].get('type') == 'var':
			data = Variables.check_to_push(((element.getchildren())[0].text)[3:], (element.getchildren())[0].text[:3])
			if data.v_type != 'int':
				print("Error - non valid var value!", file=sys.stderr)
				sys.exit(56)
			data = data.value
		else:
			data = (element.getchildren())[0].text
		if (not data.isdigit() or 
			not (int(data) <= 49 and int(data) >=0)):
			print("Error - non valid symb value!", file=sys.stderr)
			sys.exit(57)
		sys.exit(int(data))

	#Converts string escape sequences
	def string_convert(string):
		try:
			if string == None:
				return ''
			i = 0
			help_string = ''
			while i < len(string):
				if string[i] == '\\':
					counter = int(100)
					escape = int(0)
					for x in range(0,3):
						i += 1
						if string[i].isdigit():
							escape += counter * int(string[i])
							counter /= int(10)
						else: 
							raise 
					help_string += chr(int(escape))
				elif (string[i] != '#' and not string[i].isspace()):
					help_string += string[i]
				else:
					raise
				i += 1
			string = help_string
			return string
		except Exception:
			print("Error - wrong symb format!", file=sys.stderr)
			sys.exit(32)
		
	#Checks sym type and value raise exception if not right
	def check_symb(arg_type, symbol):
		try:
			if (arg_type != 'string' and arg_type != 'int' and 
				arg_type != 'bool' and arg_type != 'nil'):
				return False
			if arg_type == 'int':
				if not symbol:
					return True
				if all(c in "0123456789+-" for c in symbol):
					return True
				raise
			elif arg_type == 'bool':
				if (symbol == 'true' or symbol == 'false'):
					return True
				raise
			elif arg_type == 'nil':
				if (symbol == 'nil'):
					return True
				raise
			elif arg_type == 'string':
				if not symbol:
					return True
				if re.search(r'[#\s]', symbol):
					raise
				return True
			else:
				return False
		except Exception:
			print("Error - wrong symb format!", file=sys.stderr)
			sys.exit(32)

						
#Class performs actions with Variables
class Variables:

	#Checks variable if is in frame and returns var from frame 
	def check_to_push(variable, frame):
		if not Frames.check_frame(frame):
			print("Error - unknown frame type!", file=sys.stderr)
			sys.exit(32)
		if frame == 'GF@':
			if not variable in Frames.glob_frame:
				print("Error - variable do not exists!", file=sys.stderr)
				sys.exit(54)
			return Frames.glob_frame[variable]
		if frame == 'LF@':
			if Frames.created_frames < 1:
				print("Error - LF does not exist!", file=sys.stderr)
				sys.exit(55)
			if not variable in Frames.local_frames[Frames.created_frames-1]:
				print("Error - variable does not exists!", file=sys.stderr)
				sys.exit(54)
			return Frames.local_frames[Frames.created_frames-1][variable]
		else:
			if Frames.is_act == False:
				print("Error - TF is not active!", file=sys.stderr)
				sys.exit(55)
			if variable not in Frames.temp_frame:
				print("Error - variable does not exists!", file=sys.stderr)
				sys.exit(54)
			return Frames.temp_frame[variable]

	#Checks var format
	def check_var(variable, frame):
		if not Frames.check_frame(frame):
			print("Error - wrong frame type!", file=sys.stderr)
			sys.exit(32)
		char = variable[0]
		if (char == '_' or char == '-' or char == '$' or 
			char == '&' or char == '%' or char == '!' or 
			char == '*' or char == '?' or char.isalpha() == True):
			for x in variable:
				if (x != '_' and x != '-' and x != '$' and 
					x != '&' and x != '%' and x != '!' and 
					x != '*' and x != '?' and x.isalnum() == False):
					print("Error - wrong var format!", file=sys.stderr)
					sys.exit(32)
		else:
			print("Error - wrong var format!", file=sys.stderr)
			sys.exit(32)
	#Performs actions to define variable and stores it in frame
	def define_var(element):
		if (element.getchildren())[0].get('type') != 'var':
			print("Error - wrong argument type!", file=sys.stderr)
			sys.exit(32)
		variable = ((element.getchildren())[0].text)[3:]
		Variables.check_var(variable, ((element.getchildren())[0].text)[:3])
		Frames.var_to_frame(element,variable)


#Class performs actions with labels, stores all labels 
class Labels:
	act_call = []
	labels = {}

	#Checks label format
	def check_label(l_type, label_name):
		if l_type != 'label':
			print("Error - wrong argument type!", file=sys.stderr)
			sys.exit(32)
		char = label_name[0]
		if (char == '_' or char == '-' or char == '$' or 
			char == '&' or char == '%' or char == '!' or 
			char == '*' or char == '?' or char.isalpha() == True):
			for x in label_name:
				if (x != '_' and x != '-' and x != '$' and 
					x != '&' and x != '%' and x != '!' and 
					x != '*' and x != '?' and x.isalnum() == False):
					print("Error - wrong label format!", file=sys.stderr)
					sys.exit(32)
		else:
			print("Error - wrong label format!", file=sys.stderr)
			sys.exit(32)

	#Can return from label ?
	def return_label():
		if len(Labels.act_call) == 0:
			print("Error - no label where to return!", file=sys.stderr)
			sys.exit(56)

	#Creates label and checks if already exists
	def create_label(element, instr_num):
		label_name = (element.getchildren())[0].text
		Labels.check_label(element.getchildren()[0].get('type'), label_name)
		if (label_name in Labels.labels):
			print("Error - label already exists!", file=sys.stderr)
			sys.exit(52)
		Labels.labels[label_name] = instr_num

	#Call create labels for every label in xml code
	def create_labels(instructions):
		for i in range(len(instruction)):
			if(instruction[i].get('opcode', i) == 'LABEL'):
				Labels.create_label(instruction[i], i)

	#Performs calling on some label, return instruction number	
	def call_label(element, instr_num):
		label_name = (element.getchildren())[0].text
		Labels.check_label(element.getchildren()[0].get('type'), label_name)
		if (label_name not in Labels.labels):
			print("Error - label does not exist!", file=sys.stderr)
			sys.exit(52)
		Labels.act_call.append(instr_num)
		return(Labels.labels[label_name])

	#Performs jumping on some label, return instruction number
	def jump_to_label(l_type, name):
		Labels.check_label(l_type, name)
		if (name not in Labels.labels):
			print("Error - label does not exist!", file=sys.stderr)
			sys.exit(52)
		return(Labels.labels[name])

#Performs actions with frames and stores all frames in lists or dictionaries
class Frames:
	temp_frame = {}
	glob_frame = {}
	local_frames = []
	data_stack = []
	created_frames = 0
	is_act = False

	#Pops variable
	def pops(element):
		if not Frames.data_stack:
			print("Error - data stack is empty!", file=sys.stderr)
			sys.exit(56)
		if (element.getchildren())[0].get('type') != 'var':
			print("Error - wrong argument type!", file=sys.stderr)
			sys.exit(32)
		variable = ((element.getchildren())[0].text)[3:]
		frame = ((element.getchildren())[0].text)[:3]
		Variables.check_var(variable,frame)
		data = Variables.check_to_push(variable, frame)
		data_to_push = Frames.data_stack.pop()
		if frame == 'GF@':
			Frames.glob_frame[variable].v_type = data_to_push.v_type
			Frames.glob_frame[variable].value = data_to_push.value
		elif frame == 'LF@':
			Frames.local_frames[Frames.created_frames-1][variable].v_type = data_to_push.v_type
			Frames.local_frames[Frames.created_frames-1][variable].value = data_to_push.value 
		else:
			Frames.temp_frame[variable].v_type = data_to_push.v_type
			Frames.temp_frame[variable].value = data_to_push.value

	#Stores variable in frame
	def var_to_frame(element, variable):
		if ((element.getchildren())[0].text)[:3] == 'GF@':
			Frames.glob_frame[variable] = Var(variable)
		elif ((element.getchildren())[0].text)[:3] == 'LF@':
			if Frames.created_frames < 1:
				print("Error - LF does not exist!", file=sys.stderr)
				sys.exit(55)
			Frames.local_frames[Frames.created_frames-1][variable] = Var(variable)
		elif ((element.getchildren())[0].text)[:3] == 'TF@':
			if Frames.is_act == False:
				print("Error - TF does not exist!", file=sys.stderr)
				sys.exit(55)
			Frames.temp_frame[variable] = Var(variable)
		else:
			print("Error - wrong frame type!", file=sys.stderr)
			sys.exit(32)

	#Checks frame type
	def check_frame(frame):
		if (frame == 'LF@' or frame == 'TF@' or frame == 'GF@'):
			return True
		return False

	#Pushframe and checks if TF is created
	def push_frame():
		if Frames.is_act == False:
			print("Error - TF not created!", file=sys.stderr)
			sys.exit(55)
		Frames.created_frames += 1
		Frames.local_frames.append({})
		Frames.local_frames[Frames.created_frames-1] = Frames.temp_frame.copy()
		Frames.temp_frame.clear()
		Frames.is_act = False

	#Popframe and checks if there is frame to pop
	def pop_frame():
		if Frames.created_frames < 1:
			print("Error - no frame to pop!", file=sys.stderr)
			sys.exit(55)
		Frames.created_frames -= 1
		Frames.is_act = True
		Frames.temp_frame = Frames.local_frames.pop()

#Checks instructions and returns number of arguments needed
def instruction_args(name, n_args):
	if (name == 'CREATEFRAME' or name == 'PUSHFRAME' or
		name == 'POPFRAME' or name == 'RETURN' or 
		name == 'BREAK'):
		if n_args > 0:
			print("Error - wrong arguments count!", file=sys.stderr)
			sys.exit(32)
		return n_args
	if (name == 'DEFVAR' or name == 'CALL' or 
		name == 'PUSHS' or name == 'POPS' or 
		name == 'WRITE' or name == 'LABEL' or 
		name == 'JUMP' or name == 'EXIT' or
		name == 'DPRINT'):
		if n_args != 1:
			print("Error - wrong arguments count!", file=sys.stderr)
			sys.exit(32)
		return n_args
	if (name == 'MOVE' or name == 'NOT' or 
		name == 'INT2CHAR' or name == 'READ' or 
		name == 'STRLEN' or name == 'TYPE'):
		if n_args != 2:
			print("Error - wrong arguments count!", file=sys.stderr)
			sys.exit(32)
		return n_args
	if (name == 'ADD' or name == 'SUB' or 
		name == 'MUL' or name == 'IDIV' or 
		name == 'IDIV' or name == 'LT' or 
		name == 'GT' or name == 'EQ' or 
		name == 'AND' or name == 'OR' or 
		name == 'STRI2INT' or name == 'CONCAT' or 
		name == 'GETCHAR' or name == 'SETCHAR' or 
		name == 'JUMPIFEQ' or name == 'JUMPIFNEQ'):
		if n_args != 3:
			print("Error - wrong arguments count!", file=sys.stderr)
			sys.exit(32)
		return n_args
	print("Error - wrong operation code!", file=sys.stderr)
	sys.exit(32)

#Whole function of interpret ordered by number of arguments
def instruction_parse(name, element, Frames, Labels, i, input_data):
	global inst_done 
	inst_done += 1
	n_args = instruction_args(name, len(list(element)))
	if n_args == 0:
		if name == 'CREATEFRAME':
			Frames.is_act = True
			Frames.temp_frame.clear()
		elif name == 'PUSHFRAME':
			Frames.push_frame()
		elif name == 'POPFRAME':
			Frames.pop_frame()
		elif name == 'RETURN':
			Labels.return_label()
			return Labels.act_call.pop()
		elif name == 'BREAK':
			print("GF:", Frames.glob_frame,'\nLF:', Frames.local_frames, '\nTF:', 
				Frames.temp_frame, '\nInstructions done:', inst_done, file=sys.stderr)

	#Instructions with 1 argument needed		
	elif n_args == 1:	
		if element.getchildren()[0].tag != 'arg1':
			print("Error - wrong xml format!", file=sys.stderr)
			sys.exit(32)
		if name == 'DEFVAR':
			if Frames.check_frame(((element.getchildren())[0].text)[:3]):
				Variables.define_var(element)
			else:
				print("Error - wrong frame type!", file=sys.stderr)
				sys.exit(32)
		elif name == 'CALL':
			return(Labels.call_label(element, i))
		elif name == 'PUSHS':
			arg_type = (element.getchildren())[0].get('type')
			if arg_type == 'var':
				variable = ((element.getchildren())[0].text)[3:]
				frame = ((element.getchildren())[0].text)[:3]
				Variables.check_var(variable, frame)
				data = Variables.check_to_push(variable, frame)
				if data.value == None or data.value == 'nil':
					print("Error - no value in variable!", file=sys.stderr)
					sys.exit(56)
				Frames.data_stack.append(data)
			elif Symb.check_symb(arg_type, (element.getchildren())[0].text):
				if arg_type == 'nil':
					print("Error - non valid value!", file=sys.stderr)
					sys.exit(56)
				if arg_type == 'string':
					(element.getchildren())[0].text = Symb.string_convert((element.getchildren())[0].text)
				data = Var('')
				data.v_type = (element.getchildren())[0].get('type')
				data.value = (element.getchildren())[0].text
				Frames.data_stack.append(data)
			else:	
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
		elif name == 'POPS':
			Frames.pops(element)
		elif name == 'WRITE':
			arg_type = (element.getchildren())[0].get('type')
			if arg_type == 'var':
				variable = ((element.getchildren())[0].text)[3:]
				frame = ((element.getchildren())[0].text)[:3]
				Variables.check_var(variable, frame)
				data = Variables.check_to_push(variable, frame)
				if data.value == None:
					print("Error - not initialized variable!", file=sys.stderr)
					sys.exit(56)
				if data.value != 'nil':
					print(data.value, end='')
			elif Symb.check_symb(arg_type, (element.getchildren())[0].text):
				(element.getchildren())[0].text = Symb.string_convert((element.getchildren())[0].text)
				print((element.getchildren())[0].text, end='')
			else:	
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
		elif name == 'JUMP':
			return(Labels.jump_to_label((element.getchildren())[0].get('type'), (element.getchildren())[0].text))
		elif name == 'EXIT':
			Symb.exit_inst(element)
		elif name == 'DPRINT':
			arg_type1 = (element.getchildren())[0].get('type')
			if arg_type1 == 'var':
				variable = (element.getchildren())[0].text[3:]
				frame = (element.getchildren())[0].text[:3]
				Variables.check_var(variable, frame)
				data = Variables.check_to_push(variable, frame)
				if data.value == None:
					print("Error - not initialized variable!", file=sys.stderr)
					sys.exit(56)
				if data.value != 'nil':
					print(data.value, file=sys.stderr)
			elif Symb.check_symb(arg_type1, (element.getchildren())[0].text):
				print((element.getchildren())[0].text, file=sys.stderr)
			else:
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)

	#Instructions with two arguments
	elif n_args == 2:
		if (element.getchildren()[0].tag == 'arg1' and 
			element.getchildren()[1].tag == 'arg2'):
			arg1 = element.getchildren()[0]
			arg2 = element.getchildren()[1]
		elif (element.getchildren()[0].tag == 'arg2' and 
			element.getchildren()[1].tag == 'arg1'):
			arg1 = element.getchildren()[1]
			arg2 = element.getchildren()[0]
		else:
			print("Error - wrong xml format!", file=sys.stderr)
			sys.exit(32)
		arg_type1 = arg1.get('type')
		arg_type2 = arg2.get('type')
		if arg_type1 == 'var':
			variable1 = arg1.text[3:]
			frame1 = arg1.text[:3]
			Variables.check_var(variable1, frame1)
			cpy_where = Variables.check_to_push(variable1, frame1)
		else:
			print("Error - wrong operand type!", file=sys.stderr)
			sys.exit(32)
		if name == 'MOVE':
			if arg_type2 == 'var':
				variable2 = arg2.text[3:]
				frame2 = arg2.text[:3]
				Variables.check_var(variable2, frame2)
				cpy_from = Variables.check_to_push(variable2, frame2)
				cpy_where.value = cpy_from.value
				cpy_where.v_type = cpy_from.v_type
			elif Symb.check_symb(arg_type2, arg2.text):
					if arg_type2 == 'string':
						arg2.text = Symb.string_convert(arg2.text)
					cpy_where.value = arg2.text
					cpy_where.v_type = arg_type2
			else:
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
		elif name == 'NOT':
			try:
				negate = ''
				if arg_type2 == 'var':
					variable2 = arg2.text[3:]
					frame2 = arg2.text[:3]
					Variables.check_var(variable2, frame2)
					cpy_from = Variables.check_to_push(variable2, frame2)
					if cpy_from.v_type != 'bool':
						raise
					negate = cpy_from.value
				elif Symb.check_symb(arg_type2, arg2.text) and arg_type2 == 'bool':
					negate = arg2.text
				else:
					raise
				if negate == 'true':
					cpy_where.value = 'false'
				else:
					cpy_where.value = 'true'
				cpy_where.v_type = 'bool';
			except Exception:
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
		elif name == 'INT2CHAR' or name == 'STRLEN' or name == 'TYPE':
			try:
				data = ''
				data_type = ''
				if arg_type2 == 'var':
					variable2 = arg2.text[3:]
					frame2 = arg2.text[:3]
					Variables.check_var(variable2, frame2)
					cpy_from = Variables.check_to_push(variable2, frame2)
					if (name == 'STRLEN' and cpy_from.v_type != 'string'):
						raise
					data = cpy_from.value
					data_type = cpy_from.v_type
				elif Symb.check_symb(arg_type2, arg2.text):
					if (name == 'STRLEN' and arg_type2 != 'string'):
						raise
					if arg_type2 == 'string':
						arg2.text = Symb.string_convert(arg2.text)
					data = arg2.text
					data_type = arg_type2
				else:
					raise
				if name == 'INT2CHAR':
					try:
						if data_type != 'int':
							print("Error - non-valid int value!", file=sys.stderr)
							sys.exit(53)
						cpy_where.value = chr(int(data))
						cpy_where.v_type = 'string'
					except Exception:
						print("Error - non-valid ord value!", file=sys.stderr)
						sys.exit(58)
				elif name == 'STRLEN':
					cpy_where.value = len(data)
					cpy_where.v_type = 'int'
				else:
					if data_type == None:
						cpy_where.value = ''
					else:
						cpy_where.value = data_type
					cpy_where.v_type = 'string'
			except Exception:
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(56)
		elif name == 'READ':
			type_name = arg2.text
			if (arg_type2 != 'type' or 
				(type_name != 'int' and type_name != 'bool' and type_name != 'string')):
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
			if not intsource:
				data = input()
			else:
				data = input_data[Symb.line_counter][:-1]
				Symb.line_counter += 1
			if type_name == 'int':
				if all(c in "0123456789+-" for c in data):
					cpy_where.value = data
				else:
					cpy_where.value = 0
				cpy_where.v_type = 'int'
			elif type_name == 'string':
				if re.search(r'[#\s]', data):
					cpy_where.value = ''
				else:
					cpy_where.value = data
				cpy_where.v_type = 'string'
			elif type_name == 'bool':
				if data.lower() == 'true':
					cpy_where.value = data.lower()
				else:
					cpy_where.value = 'false'
				cpy_where.v_type = 'bool'

	#Instructions with 3 arguments
	else:
		arg1 = ''
		arg2 = ''
		arg3 = ''
		for x in range(0,3):
			if element.getchildren()[x].tag == ('arg1'):
				arg1 = element.getchildren()[x]
			elif element.getchildren()[x].tag == ('arg2'):
				arg2 = element.getchildren()[x]
			elif element.getchildren()[x].tag == ('arg3'):
				arg3 = element.getchildren()[x]
		if arg1 == '' or arg2 == '' or arg3 == '':
			print("Error - wrong xml format!", file=sys.stderr)
			sys.exit(32)
		arg_type1 = arg1.get('type')
		arg_type2 = arg2.get('type')
		arg_type3 = arg3.get('type')
		if arg_type2 == 'var':
			variable2 = arg2.text[3:]
			frame2 = arg2.text[:3]
			Variables.check_var(variable2, frame2)
			symb1 = Variables.check_to_push(variable2, frame2)
			symb1_type = symb1.v_type
			symb1_value = symb1.value
		elif Symb.check_symb(arg_type2, arg2.text):
			if arg_type2 == 'string':
				arg2.text = Symb.string_convert(arg2.text)
			symb1_type = arg_type2
			symb1_value = arg2.text
		else:
			print("Error - wrong operand type!", file=sys.stderr)
			sys.exit(32)
		if arg_type3 == 'var':
			variable3 = arg3.text[3:]
			frame3 = arg3.text[:3]
			Variables.check_var(variable3, frame3)
			symb2 = Variables.check_to_push(variable3, frame3)
			symb2_type = symb2.v_type
			symb2_value = symb2.value
		elif Symb.check_symb(arg_type3, arg3.text):
			if arg_type3 == 'string':
				arg3.text = Symb.string_convert(arg3.text)
			symb2_type = arg_type3
			symb2_value = arg3.text
		else:
			print("Error - wrong operand type!", file=sys.stderr)
			sys.exit(32)
		if name == 'JUMPIFEQ' or name == 'JUMPIFNEQ':
			if symb1_type != symb2_type:
				print("Error - wrong operand types!", file=sys.stderr)
				sys.exit(53)
			if ((symb1_value == symb2_value and name == 'JUMPIFEQ') or 
				(symb1_value != symb2_value and name == 'JUMPIFNEQ')):
				return(Labels.jump_to_label(arg_type1, arg1.text))
		else:
			if arg_type1 == 'var':
				variable1 = arg1.text[3:]
				frame1 = arg1.text[:3]
				Variables.check_var(variable1, frame1)
				cpy_where = Variables.check_to_push(variable1, frame1)
			else:
				print("Error - wrong operand type!", file=sys.stderr)
				sys.exit(32)
			if name == 'ADD' or name == 'SUB' or name == 'MUL' or name == 'IDIV':
				if (symb1_type != 'int' or symb2_type != 'int'):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				if name == 'ADD':
					cpy_where.value = int(symb1_value) + int(symb2_value)
				elif name == 'SUB':
					cpy_where.value = int(symb1_value) - int(symb2_value)
				elif name == 'MUL':
					cpy_where.value = int(symb1_value) * int(symb2_value)
				elif name == 'IDIV':
					if int(symb2_value) == 0:
						print("Error - dividing by zero!", file=sys.stderr)
						sys.exit(57)
					cpy_where.value = int(symb1_value) // int(symb2_value)
				cpy_where.v_type = 'int'
			elif name == 'AND' or name == 'OR':
				if (symb1_type != 'bool' or symb2_type != 'bool' ):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				if name == 'AND':
					if symb1_value == 'true' and symb2_value == 'true':
						cpy_where.value = 'true'
					else:
						cpy_where.value = 'false'
				else:
					if symb1_value == 'true' or symb2_value == 'true':
						cpy_where.value = 'true'
					else:
						cpy_where.value = 'false'
				cpy_where.v_type = 'bool'
			elif name == 'LT' or name == 'GT' or name == 'EQ':
				if (symb1_type != symb2_type):
					if name == 'EQ' and (symb1_type == 'nil' or symb2_type == 'nil'):
						pass
					else:
						print("Error - types are not same!", file=sys.stderr)
						sys.exit(53)
				if symb1_type == 'nil' and name != 'EQ':
					print("Error - nil is not allowed!", file=sys.stderr)
					sys.exit(53)
				if name == 'LT':
					if symb1_type == 'int':
						if int(symb1_value) < int(symb2_value):
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
					elif symb1_type == 'bool':
						if symb1_value == 'false' and symb2_value == 'true':
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
					else:
						if symb1_value < symb2_value:
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
				elif name == 'GT':
					if symb1_type == 'int':
						if int(symb1_value) > int(symb2_value):
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
					elif symb1_type == 'bool':
						if symb1_value == 'true' and symb2_value == 'false':
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
					else:
						if symb1_value > symb2_value:
							cpy_where.value = 'true'
						else:
							cpy_where.value = 'false'
				else:
					if symb1_value == symb2_value:
						cpy_where.value = 'true'
					else:
						cpy_where.value = 'false'
				cpy_where.v_type = 'bool'
			elif name == 'STRI2INT':
				if (symb1_type != 'string' or symb2_type != 'int'):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				try:
					cpy_where.value = ord(symb1_value[int(symb2_value)])
					cpy_where.v_type = 'int'
				except Exception:
					print("Error - wrong index!", file=sys.stderr)
					sys.exit(58)
			elif name == 'CONCAT':
				Symb.check_init(symb1_type, symb2_type)
				if (symb1_type != symb2_type or symb1_type != 'string'):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				cpy_where.value = symb1_value + symb2_value
				cpy_where.v_type = 'string'
			elif name == 'GETCHAR':
				Symb.check_init(symb1_type, symb2_type)
				if (symb1_type != 'string' or symb2_type != 'int'):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				try:
					cpy_where.value = symb1_value[int(symb2_value)]
					cpy_where.v_type = 'string'
				except Exception:
					print("Error - wrong index!", file=sys.stderr)
					sys.exit(58)
			elif name == 'SETCHAR':
				if (symb1_type != 'int' or symb2_type != 'string'):
					print("Error - wrong operand type!", file=sys.stderr)
					sys.exit(53)
				try:
					if int(symb1_value) >= len(cpy_where.value) or int(symb1_value) < 0:
						raise
					cpy_where.value = cpy_where.value[:int(symb1_value)] + symb2_value[0] + cpy_where.value[int(symb1_value)+1:]
					cpy_where.v_type = 'string'
				except Exception:
					print("Error - wrong index!", file=sys.stderr)
					sys.exit(58)
	return i

#Get interpret arguments
parser = optparse.OptionParser()
parser.add_option('--source', dest="xmlfile", help="input file with XML code")
parser.add_option('--input', dest="intfile", help="input file for interpret")

try:
	options,args = parser.parse_args()
except Exception:
	print("Error - wrong parameters!", file=sys.stderr)
	sys.exit(10)

if options.xmlfile and os.path.getsize(options.xmlfile) == 0:
	sys.exit(0)

#Set source and input files
source = options.xmlfile
intsource = options.intfile

if not source and not intsource:
	print("Error - no parameter set!", file=sys.stderr)
	sys.exit(10)

if not source:
	source = sys.stdin
if intsource:
	try:
		fp = open(intsource)
		input_data = fp.readlines()
	except Exception:
		print("Error - opening file!", file=sys.stderr)
		sys.exit(11)
	finally:
		fp.close()
else:
	input_data = ''
try:
	tree = ET.parse(source)
except Exception:
	print("Error - wrong XML format!", file=sys.stderr)
	sys.exit(31)

#Get root from xml file and sort code by order number
root = tree.getroot()
root_tag = root.tag
root[:] = sorted(root, key = lambda child: (child.tag, int(child.get('order'))))

#Check xml format
try:
	if (root_tag != "program" or 
		root.attrib["language"] != "IPPcode19" or
		len(root.attrib) > 3 or
		(len(root.attrib) == 2 and ('name' not in root.attrib and 'description' not in root.attrib)) or
		(len(root.attrib) == 3 and ('name' not in root.attrib or 'description' not in root.attrib))):
		print("Error - wrong XML format!", file=sys.stderr)
		sys.exit(31)
except Exception:
	print("Error - wrong XML format!", file=sys.stderr)
	sys.exit(31)

#Create all labels and call function of interpret on every instruction
try:
	instruction = tree.findall('.//instruction')
	Labels.create_labels(instruction)
	inst_done = 0;
	i = 0
	while i < len(instruction):
		if int(instruction[i].get('order')) != i + 1:
			raise
		actInst = instruction[i].get('opcode', i)
		if not actInst:
			print("Error - wrong XML format!", file=sys.stderr)
			sys.exit(31)
		i = instruction_parse(actInst, instruction[i], Frames, Labels, i, input_data)+1
except Exception:
	print("Error - wrong XML format!", file=sys.stderr)
	sys.exit(32)
