import json
import socket
import sys

#Get api and city name from arguments
if (len(sys.argv) != 3) or ((not sys.argv[1]) or (not sys.argv[2])):
	print("Arguments not set!")
	sys.exit(1);
city = sys.argv[2]
city = city.lower()
appid = sys.argv[1]

#Try to create socket
try:
	s_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error as err:
	print("Socket creation failed!")
	sys.exit(1)

#Define server name and port for HTTP
server = "api.openweathermap.org"
port = 80

#Create request
request = "GET /data/2.5/weather?q="+city+"&APPID="+appid+"&units=metric HTTP/1.1\r\nHost:"+server+"\r\nConnection:close\r\n\r\n"

#Try to communicate with openweathermap.org. If client can not connect/send request/recieve data return error message
try:
	s_socket.connect((server,port))
	s_socket.send(request.encode())
	res =  s_socket.recv(1024)	
except socket.error as err:
	print("Communication with server failed!")
	sys.exit(1);

#Decode data. When send -> encode / recieve -> decode data
data = res.decode()

#Split HTTP header and get state code
error_check = (data.split('\n',1)[-2]).split(' ',1)[-1] 
if error_check.split(' ',1)[-2] != '200':
	if error_check.split(' ',1)[-2] == '401':
		print('401 Unauthorized - wrong api key!')
		sys.exit(1)
	elif error_check.split(' ',1)[-2] == '404':
		print('404 Not Found - wrong city name!')
		sys.exit(1)
	else:
		print(error_check)
		sys.exit(1)

#Edit message to have only important data for weather and load it to json
data = '{'+data.split("{",1)[-1]
data = json.loads(data)

#Print data needed on output
print(data['name'])
print(data['weather'][0]['main'])
temp = data['main']['temp']
print("Temp: "+str(temp)+"°C")
humidity = data['main']['humidity']
print("Humidity: "+str(humidity)+'%')
pressure = data['main']['pressure']
print("Pressure: "+str(pressure)+'hPa')
speed = data['wind']['speed'] * 3.6
print("Wind-speed: "+str(speed)+'km/h')
if 'deg' in data['wind']:
	direction = data['wind']['deg']
	print("Wind-deg: "+str(direction))
else:
	print("Wind-deg: n/a")
