i.MX 8M Mini (Linux) to M4 (Arduino) comms
##########################################

The Portenta x8 communicates between the Arduino and Linux sides using the
`m4-proxy <https://github.com/arduino/portentax8-m4-proxy>`_ service running on
the x8's Linux microPlaftorm.

The `m4-proxy` forwards RPC calls from the Arduino (M4) side from the M4 only to an RPC server running on localhost.

Linux side
==========

Python: msgpackrpc-python
-------------------------
https://github.com/msgpack-rpc/msgpack-rpc-python

https://pypi.org/project/msgpack-rpc-python/

Python library which hooks into the m4-proxy service.

Because the m4-proxy is running on the LMP, when running from container, need to
add the mapping ``"x4-proxy:host-gateway"`` in the container run config.

.. code-block:: console
    :caption: docker run command

    docker run --add-host"m4-proxy:host-gateway" demo-container

.. code-block:: yaml
    :caption: docker-compose.yml

    services:
        demo-container:
            image: demo-image
            extra_hosts:
                - "m4-proxy:host-gateway"


Create an RPC client which sends to a server

.. code-block:: python

    import msgpackrpc

    m4_proxy_register = msgpackrpc.Address('m4-proxy', 5000)

    client = msgpackrpc.Client(m4_proxy_register)
    hello = client.call("hello")

    client = msgpackrpc.Client(m4_proxy_register)  # needs to be initialised for each client call
    world = client.call("world")

Create an RPC server which receives RPC calls

.. code-block:: python

    import msgpackrpc

    # create the RPC client
    m4_proxy_register = msgpackrpc.Address('m4-proxy', 5000)
    client = msgpackrpc.Client(m4_proxy_register)

    # register script as RPC server listening on port 5015
    client.call('register', 5015, ['tty'])

    # create
    class SerialRPC(object):
        """Class that holds all implementations for the procedure the service exposes"""

        def tty(self, msg):
            """Special procedure called by the Arduino SerialRPC library"""
            print(f'{msg.decode()}', end=None)

    try:
        # create RPC server and await incoming RPC calls
        rpc_server = msgpackrpc.Server(SerialRPC())
        rpc_server.listen(msgpackrpc.Address("localhost", 5015))
        rpc_server.start()

    except KeyboardInterrupt:
        rpc_server.stop()
        rpc_server.close()


Arduino side
============

RPC
---

When you add the Portenta X8 to the Arduino IDE, it automatically adds the RPC library.
Documentation on this library doesn't seem to be on the Arduino website though that
may change at some point.

Example usage (not tested so might be broken)

.. code-block:: arduino

    #include <RPC.h>

    char hello = "HELLO";

    void setup() {
        RPC.bind("hello", []{ return hello; });
        RPC.bind("world", []{ return world(); });
    }

    void loop() {}

    char world() {
        char world = "WORLD!";
        return world;
    }

To call these bindings you'd use client.call("hello") on the Python side.

SerialRPC
---------

When an Arduino Sketch includes the ``<SerialRPC.h>`` header, all the Serial calls
are actually RPC calls to the ``tty`` procedure, passing the string to print as parameter.


.. code-block:: arduino

    #include <SerialRPC.h>

    void setup() {
        Serial.begin(115200);
    }

    void loop() {
        Serial.println("Hello world");
        delay(1);
    }
