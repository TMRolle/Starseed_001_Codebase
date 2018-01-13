#!/usr/bin/env python

import sqlite3
import paho.mqtt.client as mqtt
from datetime import datetime, date
import math
import atexit
import time

print("Connecting to MQTT Server")
client = mqtt.Client("mqttLoggerApp")
client.connect('localhost')
client.subscribe("#",2)

print("Connecting to database")
conn=sqlite3.connect('mqttlog.db',check_same_thread=False)
curs=conn.cursor()

print("Setting up callbacks")

def insertMessage(topic, msgvalue):
    curs.execute("CREATE TABLE IF NOT EXISTS %s([timestamp] timestamp, msgvalue REAL)"%(topic))
    curs.execute("INSERT INTO %s(timestamp, msgvalue) values (?, ?)"%topic, ( datetime.now(),msgvalue))
    conn.commit()

def on_message(client, userdata, message):
    insertMessage(str(message.topic), float(message.payload))
    print("%s: %f"%(str(message.topic), float(message.payload)))

client.on_message=on_message

def exit_handler():
    conn.close()
    client.loop_stop()
atexit.register(exit_handler)

print("Starting listener")
client.loop_start()

while True:
    time.sleep(1)
