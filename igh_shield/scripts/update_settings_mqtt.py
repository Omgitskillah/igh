#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import sys

broker="farmshield.illuminumgreenhouses.com"
uname = "shields"
pword = "940610b43b1"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    SENSOR_DATA = 0x44
    EVENT_MSG = 0x45
    SETTINGS_MSG = 0x53
    
    global settings_hex_list
    device_id = sys.argv[1]

    split_device_id = [device_id[index : index + 2] for index in range(0, len(device_id), 2)]
    device_sn = []
    for i in split_device_id:
        device_sn += [int(i, 16)]

    data_start     = [0x3C]
    data_len       = [0x00]
    message_type   = [SETTINGS_MSG]
    data_direction = [0x44]
    message_id     = [0x12]
    data_end       = [0x3E]
    
    data_to_send = data_start + data_len + message_type + data_direction + device_sn + message_id + settings_hex_list + data_end

    #add the data length
    data_to_send[1] = len(data_to_send)

    device_settings_topic = "44" + device_id

    print("sending ", end="")
    for j in data_to_send:
        print( format(j, '02X'), end="" )
    print(" to " + device_id )

    client.publish( device_settings_topic , bytearray(data_to_send) )

def on_publish(client, userdata, mid):
    exit()


client = mqtt.Client()
client.username_pw_set( uname, pword)
client.on_connect = on_connect
client.on_publish = on_publish

split_settings = [sys.argv[2][index : index + 2] for index in range(0, len(sys.argv[2]), 2)]

settings_hex_list = []

for i in split_settings:
    settings_hex_list += [int(i, 16)]

client.connect(broker)#connect

client.loop_forever()