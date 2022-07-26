# coding: utf-8
# Copyright 2021 HiveMQ GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import time
import paho.mqtt.client as paho
from paho import mqtt
import os

# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def uploadMQTT(message):
    # using MQTT version 5 here, for 3.1.1: MQTTv311, 3.1: MQTTv31
    # userdata is user defined data of any type, updated by user_data_set()
    # client_id is the given name of the client
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
    client.on_connect = on_connect
    # enable TLS for secure connection
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
    # set username and password
    client.username_pw_set("danielcardenaz", "Manzana2132881")
    # connect to HiveMQ Cloud on port 8883 (default for MQTT)
    print('Conectando al broker...')
    client.connect("a33454d332054780b8feaf83950ed54a.s2.eu.hivemq.cloud", 8883)

    # setting callbacks, use separate functions like above for better visibility
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    client.on_publish = on_publish

    # subscribe to all topics of encyclopedia by using the wildcard "#"
    client.loop_start()
    print('Subscribiendose al topic...')
    client.subscribe("encyclopedia/#", qos=1)

    # a single publish, this can also be done in loops, etc.
    print('Publicando...')
    for line in message:
        client.publish("encyclopedia/temperature", payload=line, qos=1)
    client.loop_stop()
    # loop_forever for simplicity, here you need to stop the loop manually
    # you can also use loop_start and loop_stop
    # client.loop_forever()

while True:
    if os.path.exists('/home/pi/LORA-MQTT-GATEWAY/single_chan_pkt_fwd/datos.txt'):
        print('Existe archivo')
        try:
            text_file = open('/home/pi/LORA-MQTT-GATEWAY/single_chan_pkt_fwd/datos.txt','r')
            data = text_file.readlines()
            print('Archivo leido exitosamente')
            text_file.close()
            if data != []:
                print('Enviando datos...')
                uploadMQTT(data)
                print('Datos enviados')
                # print('Limpiando buffer...')
                open('/home/pi/LORA-MQTT-GATEWAY/single_chan_pkt_fwd/datos.txt', "w").close() #Borra el contenido del archivo
                #os.remove('/home/pi/LORA-MQTT-GATEWAY/single_chan_pkt_fwd/datos.txt')
            else:
                print('Archivo vacio')

        except Exception as e:
            print('No se subieron lecturas, error>{}'.format(e))
            open('/home/pi/LORA-MQTT-GATEWAY/single_chan_pkt_fwd/datos.txt', "w").close()
        finally:
            time.sleep(10)
    else:
        print('No existe archivo')
        time.sleep(10)