import time
from gpiozero import MCP3008
import warnings

warnings.filterwarnings("ignore")

#Connect LM35 to the channel 1 and MQ9 to channel 0 of MCP3008
temp_sensor = MCP3008(channel = 1)
gas_sensor = MCP3008(channel = 0)

while True:
    temp_level = temp_sensor.value
    voltage = temp_level * 3.3
    temperature = voltage * 100
    print("Temperature :", "{:.2f}".format(temperature), "Celcius", )
    time.sleep(1)
    
    COLevel = gas_sensor.value * 0.01
    print ("Current carbon monoxide level is ", "{:.3f}".format(COLevel))
    if COLevel > 0.2:
        print("Carbonmonoxide detected")
    time.sleep(1)
