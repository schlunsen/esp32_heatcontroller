import paho.mqtt.client as mqtt
import arrow

IS_ACTIVATED = 1
CONTROL_MIN = 19
CONTROL_MAX = 22
SENSOR_ID = 'temp1'
heat_started = None
HEAT_MAX_TIME_SECONDS = 60*60

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("esp32/temp")
    client.subscribe("heatautomatic")
    client.subscribe("heat/controlmin")
    client.subscribe("heat/controlmax")
    client.subscribe("heat/sensorid")
    client.subscribe("heat/switch")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global IS_ACTIVATED, CONTROL_MIN, CONTROL_MAX, SENSOR_ID, heat_started

    if heat_started and (arrow.now().datetime - heat_started).seconds > HEAT_MAX_TIME_SECONDS and False:
        pass

#        client.publish('heat/switch', 0)

#    print(msg.topic+" "+str(msg.payload))
    if msg.topic == 'esp32/temp' and IS_ACTIVATED and SENSOR_ID == 'temp1':
        temp = float(msg.payload)
        print("HEY", temp)
        if temp <= float(CONTROL_MIN):
            client.publish('heat/switch', 1, retain=True)
        elif temp >= float(CONTROL_MAX):
            client.publish('heat/switch', 0, retain=True)

    if msg.topic == 'heat/switch':
        payload = int(msg.payload)
        if payload == 1:
            heat_started = arrow.now().datetime
        else:
            heat_started = None

    if msg.topic == 'heat/sensorid':
        SENSOR_ID  = msg.payload.decode('utf-8')
        print("SNESOR SAT", SENSOR_ID)

    if msg.topic == 'heatautomatic':
        IS_ACTIVATED = int(msg.payload)


    if msg.topic == 'heat/controlmin':
        CONTROL_MIN = msg.payload

    if msg.topic == 'heat/controlmax':
        CONTROL_MAX = msg.payload

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)
client.publish('heat/controlmin',CONTROL_MIN)
client.publish('heat/controlmax',CONTROL_MAX)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.o
from time import sleep
pubcount = 0
while 1:
    client.loop()
    sleep(1)
    if pubcount >= 10:
        print("Publish")
        client.publish("heatautomatic", IS_ACTIVATED, retain=True)
        client.publish("heat/sensorid", SENSOR_ID, retain=True)
        pubcount = 0
    pubcount += 1