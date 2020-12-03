import time
import random
import string
import datetime
import struct
import paho.mqtt.client as paho

broker="farmshield.illuminumgreenhouses.com"
uname = "shields"
pword = "940610b43b1"
topic1 = "55e00fce680f88ae453f7fbc4a"
topic2 = "55e00fce684666bdddcaa57021"
topic3 = "55e00fce682c5cb31007bc431"
topic4 = "55e00fce68e9d014125add3e8e"
topic5 = "55e00fce680e0cb67573fc4658"


# Temp Humidity constants
SOIL_TEMPERATURE_MULTIPLIER_D1 = -39.66
SOIL_TEMPERATURE_MULTIPLIER_D2 = 0.01
SOIL_HUMIDITY_MULTIPLIER_C1    = -2.0468
SOIL_HUMIDITY_MULTIPLIER_C2    = 0.0367
SOIL_HUMIDITY_MULTIPLIER_C3    = (-1.59955) * (10**(-6))
SOIL_HUMIDITY_MULTIPLIER_T1    = 0.01
SOIL_HUMIDITY_MULTIPLIER_T2    = 0.00008
ROOM_TEMPERATURE               = 25.0

# TOPICS = [(topic3,0)]
# TOPICS = [(topic3,0),(topic2,0)]
TOPICS = [(topic3,0),(topic2,0),(topic1,0),(topic4,0),(topic5,0)]

def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

def get_tuple_name( tuple_id ):
    switcher = {
        0x00 : "MSG_ACK_TUPLE             ",
        0x01 : "SPEAR_ID                  ",
        0x02 : "STORE_TIMESTAMP           ",
        0x03 : "SEND_TIMESTAMP            ",
        0x04 : "SOIL_MOISTURE             ",
        0x05 : "AIR_HUMIDITY              ",
        0x06 : "SOIL_HUMIDITY             ",
        0x07 : "WATER_DISPENSED           ",
        0x08 : "CARBON_DIOXIDE            ",
        0x09 : "AIR_TEMPERATURE           ",
        0x0A : "SOIL_TEMPERATURE          ",
        0x0B : "SOIL_NPK                  ",
        0x0C : "LIGHT_INTENSITY           ",
        0x0D : "SHIELD_BATTERY_LEVEL      ",
        0x0E : "SPEAR_BATTERY_LEVEL       ",
        0x0F : "VALVE_POSITION            ",
        0x10 : "IGH_SEND_SETTINGS         ",
        0x11 : "IGH_READ_SETTINGS         ",
        0x12 : "SPEAR_DATA                ",
        0x13 : "SPEAR_RF_ID               ",
        0x14 : "SHIELD_RF_ID              ",
        0x15 : "SEND_INTERVAL             ",
        0x16 : "OP_STATE                  ",
        0x17 : "SHIELD_ID                 ",
        0x18 : "SPEAR_BATT_LOW_THRESHOLD  ",
        0x19 : "SHIELD_BATT_LOW_THRESHOLD ",
        0x1A : "BUTTON_PRESS              ",
        0xFD : "RESTART                   ",
        0xFE : "DATA_PKT                  ",
        0xFF : "END_OF_PKT_ID             "
        }
    return switcher.get( tuple_id ,"UNKNOWN")

def process_spear_tuples( packet, start, stop ):
    byte_tracker = start
    soil_humidity = 0
    soil_temperature = 0
    while byte_tracker < stop:
        tuple_id = packet[byte_tracker]
        if 0x3e == tuple_id:
            break
        tuple_len = packet[(byte_tracker + 1)]
        tuple_data = packet[ (byte_tracker + 2) : (byte_tracker + 2 + tuple_len) ]
        if tuple_id == 0x01:
            print( get_tuple_name(tuple_id), " : ", str(tuple_data.hex()) )
        elif tuple_id == 0x06:
            # this assumes that the humidity tuple will always appear first in the packet
            [soil_humidity] = struct.unpack('<H', tuple_data)
        elif tuple_id == 0x0A:
            [soil_temperature] = struct.unpack('<H', tuple_data)
            linearHumidity = SOIL_HUMIDITY_MULTIPLIER_C1 + SOIL_HUMIDITY_MULTIPLIER_C2 * soil_humidity + SOIL_HUMIDITY_MULTIPLIER_C3 * soil_humidity * soil_humidity
            temperature = SOIL_TEMPERATURE_MULTIPLIER_D1 + SOIL_TEMPERATURE_MULTIPLIER_D2 * soil_temperature
            correctedHumidity = (temperature - ROOM_TEMPERATURE) * (SOIL_HUMIDITY_MULTIPLIER_T1 + SOIL_HUMIDITY_MULTIPLIER_T2 * soil_humidity) + linearHumidity
            print( "SOIL_HUMIDITY               : ", "%.2f%%" % correctedHumidity )
            print( "SOIL_TEMPERATURE            : ", "%.2fC" % temperature )
        else:
            [value] = struct.unpack('<H', tuple_data)
            print( get_tuple_name(tuple_id), " : ", value )
        byte_tracker += tuple_len + 2

def process_tuples( packet, start, stop ):
    byte_tracker = start
    current_unix_time = 0
    while byte_tracker < stop:
        tuple_id = packet[byte_tracker]
        if 0x3e == tuple_id:
            break
        tuple_len = packet[(byte_tracker + 1)]
        tuple_data = packet[ (byte_tracker + 2) : (byte_tracker + 2 + tuple_len) ]
        if tuple_id == 18:
            process_spear_tuples( tuple_data, 0, tuple_len )
        else:
            if tuple_id == 0x02:
                [current_unix_time] = struct.unpack('<I', tuple_data)
                print( get_tuple_name(tuple_id), " : ", current_unix_time )
            elif tuple_id == 0x0F:
                print( get_tuple_name(tuple_id), " : ", ("OPEN" if tuple_data == 0x01 else "CLOSED") )
            elif tuple_id == 0x0D:
                [shield_bat_level] = struct.unpack('<f', tuple_data)
                print( get_tuple_name(tuple_id), " : ", "%.2f%%" % shield_bat_level)
            elif tuple_id == 0x07:
                [total_water_dispensed] = struct.unpack('<f', tuple_data)
                print( get_tuple_name(tuple_id), " : ", total_water_dispensed )
            else:
                print( get_tuple_name(tuple_id), " : ", str(tuple_data.hex()) )
        byte_tracker += tuple_len + 2
    timestamp = datetime.datetime.fromtimestamp(current_unix_time)
    print("***********************************************************" )
    print("STORED TIME STAMP: ", timestamp.strftime('%Y-%m-%d_%H-%M-%S'))
    print("***********************************************************" )

#define callback
def on_message(client, userdata, message):
    time.sleep(1)
    print("\n\n\n***********************************************************" )
    if message.payload[1] == len(message.payload):
        print("BORON_ID                    : ", str(message.payload[4:16].hex()) )
        print("MSG_COUNTER                 : ", str(message.payload[16]) )
        payload_len = message.payload[18]
        process_tuples( message.payload, 19, payload_len + 18 )

new_client_id = get_random_string(12)
client= paho.Client(new_client_id)
client.username_pw_set( uname, pword)
client.on_message=on_message
print("connecting to broker ",broker, " client id:", new_client_id)
print("topics ", TOPICS)
client.connect(broker)#connect

client.loop_start() #start loop to process received messages
print("subscribing ")
client.subscribe(TOPICS)#subscribe

while True:
    time.sleep(1)
client.disconnect() #disconnect
client.loop_stop() #stop loop