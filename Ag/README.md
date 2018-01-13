Agricultural Automation codebase for Starseed 001

The current agricultural computation and control setup consists of a number of devices.
Central to the setup is a Raspberry Pi running the latest version of Raspbian. This runs an MQTT broker and coordinates all the peripheral devices over a network.
One or more NodeMCU modules are used to interface with all the sensors and actuators. Shift register modules are used to expand the available number of devices.
An arduino nano is used as an interface for the AC dimmer switch(es), as an inexpensive way to offload the task of z-crossing from the NodeMCU modules.
