import paho.mqtt.client as mqtt

broker="farmshield.illuminumgreenhouses.com"
uname = "shields"
pword = "940610b43b1"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    
    device_id = "e00fce680e0cb67573fc4658"

    split_device_id = [device_id[index : index + 2] for index in range(0, len(device_id), 2)]
    device_sn = []
    for i in split_device_id:
        device_sn += [int(i, 16)]

    SENSOR_DATA = 0x44
    EVENT_MSG = 0x45
    SETTINGS_MSG = 0x53

    data_start     = [0x3C]
    data_len       = [0x00]
    message_type   = [EVENT_MSG]
    data_direction = [0x55]
    message_id     = [0x12]
    # instert payload here
    payload        = [0xFE,0x00,0x17,0x0C,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x02,0x04,0x16,0xB8,0x1C,0x60,0x1B,0x03,0x00,0x00,0x23,0xFC,0x01,0x50] 
    payload[1] = len(payload)
    # instert payload here
    data_end       = [0x3E]
    
    data_to_send = data_start + data_len + message_type + data_direction + device_sn + message_id + payload + data_end

    #add the data length
    data_to_send[1] = len(data_to_send)

    device_settings_topic = "55" + device_id

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

client.connect(broker)#connect

client.loop_forever()