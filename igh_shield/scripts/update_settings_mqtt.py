import paho.mqtt.client as mqtt
import sys

broker="farmshield.illuminumgreenhouses.com"
uname = "shields"
pword = "940610b43b1"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    data_start = [0x3C,0x00,0x53,0x55,0xE0,0x0F,0xCE,0x68,0xD7,0xE9,0x6E,0xD9,0x67,0xE4,0xCF,0x0C,0x50]
    
    #change this to add new settings
    global settings_hex_list
    new_settings = settings_hex_list
    data_end = [0x3E]
    data_to_send = data_start + new_settings + data_end

    #add the data length
    data_to_send[1] = len(data_to_send)

    device_id = sys.argv[1]
    device_settings_topic = "44" + device_id

    print("sending ", end="")
    for j in new_settings:
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