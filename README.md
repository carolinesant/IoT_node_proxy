# IN2140-Home_exam: IoT node proxy
This code was created for the home exam in IN2140 at University of Oslo

## The inspiration
This assignment is inspired by IoT systems for home automation. These automation systems comprise a huge number of devices, where some can be tiny low-power nodes, while others have a lot of computing power. Even Cloud servers are part of many IoT systems.

To make development and debugging easy, communication between such IoT nodes is often implemented by sending messages in the XML format. However, some IoT nodes have such little computing power and such bad network connections that the huge overhead of XML cannot be supported. These IoT nodes are then included in the network by adding a proxy very close to that tiny IoT node, but with a bit more computing power and a better network connection. This proxy converts a very efficient but difficult-to-understand binary format to XML on behalf of the IoT node.

## The code
The implementation consists of a proxy and a variety of helper functions of the proxy that converts student records back and forth between an XML format and a binary format.
