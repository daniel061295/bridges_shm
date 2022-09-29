#
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
import mysql.connector
import json
import threading


conn = mysql.connector.connect(host="127.0.0.1",user="root",passwd="Manzana2132881+",database="proyecto")
topic = 'SHM_PROYECTO/#'


def upload_message (data):
    for element in data:
        #print(element)
        fechahora = str(element["fecha"])
        nodo = int(element["nodo"])
        valor_x = float(int(element["x"])/256000)
        valor_y = float(int(element["y"]) / 256000)
        valor_z = float(int(element["z"]) / 256000)
        print('fechahora\t=\t{}\nnodo\t\t=\t{}\nvalor_x\t\t=\t{}\nvalor_y\t\t=\t{}\nvalor_z\t\t=\t{}\n'.format(fechahora,nodo,valor_x,valor_y,valor_z))
        cursor = conn.cursor()
        query = "INSERT INTO acelerometros (`FECHAYHORA`,`NODO`,`VALOR_X`,`VALOR_Y`,`VALOR_Z`) VALUES(%s, %s, %s, %s, %s)"
        val = (fechahora, nodo, valor_x, valor_y, valor_z)
        cursor.execute(query, val)
        conn.commit()
        time.sleep(0.01)

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
    if msg.topic != 'SHM_PROYECTO/TRIGGER':
        message = msg.payload.decode('utf8').replace("'", '"')
        data = json.loads(message)
        uploading_message = threading.Thread(target=upload_message, args=(data,))
        uploading_message.start()
    else:
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))


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
client.connect("a33454d332054780b8feaf83950ed54a.s2.eu.hivemq.cloud", 8883)

# setting callbacks, use separate functions like above for better visibility
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

# subscribe to all topics of encyclopedia by using the wildcard "#"
client.subscribe(topic, qos=2)

# a single publish, this can also be done in loops, etc.
#client.publish("encyclopedia/temperature", payload="hot", qos=1)

# loop_forever for simplicity, here you need to stop the loop manually
# you can also use loop_start and loop_stop
client.loop_forever()


