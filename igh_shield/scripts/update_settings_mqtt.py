import paho.mqtt.client as mqtt

broker="farmshield.illuminumgreenhouses.com"
uname = "shields"
pword = "940610b43b1"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    data_start = [0x3C,0x00,0x53,0x55,0xE0,0x0F,0xCE,0x68,0xD7,0xE9,0x6E,0xD9,0x67,0xE4,0xCF,0x0C,0x50]
    
    #change this to add new settings
    new_settings = [0x10,0x06,0x02,0x04,0x80,0xEE,0x36,0x00]
    
    data_end = [0x3E]
    data_to_send = data_start + new_settings + data_end

    #add the data length
    data_to_send[1] = len(data_to_send)

    device_id = "e00fce68d7e96ed967e4cf0c"
    device_settings_topic = "44" + device_id

    client.publish( device_settings_topic , bytearray(data_to_send) )

client = mqtt.Client()
client.username_pw_set( uname, pword)
client.on_connect = on_connect

client.connect(broker)#connect

client.loop_forever()