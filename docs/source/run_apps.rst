**********************
Run apps on Linux side
**********************

Docker on the Portenta
======================

Version 436 of the Portenta OS is running v20.10.7 of the Docker engine.

Review https://docs.docker.com/engine/release-notes/#20107 to see what current
functionality is missing -> e.g. Need to run `docker-compose` and not `docker compose`

Linux commands available
========================
The Portenta is using BusyBox as its command toolbox. Installed version is 1.33.1 (latest is 1.34.1)

List of commands is here:
https://busybox.net/downloads/BusyBox.html

Detect breakout board
=====================
https://docs.arduino.cc/tutorials/portenta-breakout/getting-started

The :ref:`Portenta web shell <web-interface>` comes with some python scripts to
work with connected carriers.

.. code-block:: console

    # cd examples
    # ls

    adcs.py            gpios.py
    carrier_detect.py  pwms.py

See if board is being detected

.. code-block:: console
    :caption: bad result

    # python carrier-detect.py

    Detected board:
    Are we on a carrier board?
    No carrier board detected.

.. code-block:: console
    :caption: good result

    # python carrier-detect.py

    Detected board: portenta-x8
    Are we on a carrier board? yes
    Detected carrier board: breakout

The breakout comes with a library in the Arduino IDE:
https://github.com/arduino-libraries/Arduino_PortentaBreakout

All current documentation is based on the H7 rather than the X8. Can't see any
way to load libraries on the Linux side.



Hardware monitor
================

HTOP isn't installed on the Portenta and there's no package manager to install :(

Google has made a docker container that provides detailed info on all things hardware:

https://github.com/google/cadvisor

.. code-block:: console

    $ VERSION=v0.45.0

    $ sudo docker run \
       --rm \
       --volume=/:/rootfs:ro \
       --volume=/var/run:/var/run:ro \
       --volume=/sys:/sys:ro \
       --volume=/var/lib/docker/:/var/lib/docker:ro \
       --volume=/dev/disk/:/dev/disk:ro \
       --publish=8080:8080 \
       --detach=true \
       --name=cadvisor \
       --privileged \
       --device=/dev/kmsg \
       gcr.io/cadvisor/cadvisor:$VERSION

Monitor Arduino using Linux
===========================

Mentioned briefly in this tutorial:
https://docs.arduino.cc/tutorials/portenta-x8/python-arduino-data-exchange#debugging-the-arduino-sketch

In order to monitor Arduino output, can't use the usual serial monitor. Have to
use a python script on the Linux side and print the container logs to stdout.

Script files: https://docs.arduino.cc/beb93128c955e3edbec71e7a148f9961/py-serialrpc.zip

Commands on portenta:

NOTE: There is an error in the docker-compose.yml file provided in py-serialrpc.zip.
See: https://forum.arduino.cc/t/portenta-x8-rpc-method-not-found/1016310/5

Should read:

.. code-block::

    extra_hosts:
    - "m4-proxy:host-gateway"

Commands on Portenta to install and run script in a docker container

.. code-block:: console


    $ wget https://docs.arduino.cc/beb93128c955e3edbec71e7a148f9961/py-serialrpc.zip
    $ unzip py-serialrpc.zip
    $ cd py-serialrpc/serialrpc
    $ sudo docker build . -t py-serialrpc
    $ cd ..
    $ sudo docker-compose up -d
    $ sudo docker-compose logs -f --tail 20



This uses the m4-proxy service to send/receive messages. To check the service status:

.. code-block:: console

    $ sudo journalctl -fu m4-proxy

    -- Journal begins at Wed 2021-03-24 10:25:19 UTC. --
    Sep 01 16:06:41 portenta-x8-1421c209dab6fad9 systemd[1]: Started M4 builtin led forwarder.
    Sep 01 16:06:41 portenta-x8-1421c209dab6fad9 m4_proxy[1107]: charDevListener

Send data from Arduino side to Linux side
=========================================

Official tutorial: https://docs.arduino.cc/tutorials/portenta-x8/python-arduino-data-exchange

Blink application files
-----------------------

If you don't want to use (or don't have) the mentioned sensors you can use the provided code below.
Use the following directory structure:::

    python-blink-rpc
    |-- arduino
    |   └-- Blink-RPC
    |       └-- Blink-RPC.ino
    |-- src
    |   └-- blink-rpc.py
    |-- docker-compose.yml
    |-- Dockerfile
    └-- requirements.txt

**Blink-RPC.ino**

.. code-block:: arduino

    #include <RPC.h>
    #include <SerialRPC.h>

    int DELAY_LEN = 1000;
    int COUNT = 0;
    int LED_STATE = HIGH;

    void setup() {
      pinMode(LED_BUILTIN, OUTPUT);
      Serial.begin(115200);
      Serial.println("Blink test on M4");

      RPC.bind("count", []{ return COUNT; });
      RPC.bind("led", []{ return LED_STATE; });

      Serial.println("Starting");

    }

    void loop() {
      toggleLED();
      countUp();
      delay(DELAY_LEN);
    }

    void toggleLED() {
      LED_STATE = (LED_STATE==HIGH)? LOW:HIGH;
      digitalWrite(LED_BUILTIN, LED_STATE);
    }

    void countUp() {
      COUNT++;
    }

**blink-rpc.py**

.. code-block:: python

    import time
    from msgpackrpc import Address as RpcAddress, Client as RpcClient, error as RpcError

    publish_interval = 1

    # The M4 Proxy address needs to be mapped via Docker's extra hosts
    m4_proxy_address = 'm4-proxy'
    m4_proxy_port = 5001


    def get_data_from_m4():
        rpc_address = RpcAddress(m4_proxy_address, m4_proxy_port)

        data = ()

        try:
            rpc_client = RpcClient(rpc_address)
            count = rpc_client.call('count')

            rpc_client = RpcClient(rpc_address)
            led = rpc_client.call('led')

            data = count, led

        except RpcError.TimeoutError:
            print("Unable to retrieve data from the M4.")

        return data


    if __name__ == '__main__':

        print()
        print("============================================")
        print("==       Portenta X8 M4 output            ==")
        print("============================================")
        print()

        try:
            while True:
                data = get_data_from_m4()
                if len(data) > 0:
                    print("count: ", data[0])
                    print("led: ", data[1])
                else:
                    print("no data!")
                time.sleep(publish_interval)
        except KeyboardInterrupt:
            print('Stopped.')

**Dockerfile**

.. code-block:: dockerfile

    FROM python:3.10.6-alpine

    WORKDIR /usr/src/app

    COPY requirements.txt ./
    RUN pip install --no-cache-dir -r requirements.txt

    COPY src/ .

    ENTRYPOINT [ "python", "blink-rpc.py" ]


**docker-compose.yml**

.. code-block:: yaml

    services:
      python-blink-rpc:
        image: python-blink-rpc
        build: .
        restart: unless-stopped
        environment:
          - PYTHONUNBUFFERED=1
        extra_hosts:
          - "m4-proxy:host-gateway"

**requirements.txt**

.. code-block:: text

    msgpack-rpc-python

Steps to take
-------------

1. Load sketch onto Arduino side using Arduino IDE

You'll need to have :ref:`added the Portenta to the Arduino IDE <arduino-ide>` to do this.

2. Upload the python-blink-rpc directory to the X8

.. code-block:: console
    :caption: On your PC

    adb push python-blink-rpc /home/fio


3. Build the Docker image

.. code-block:: console
    :caption: On the x8

    cd /home/fio/python-blink-rpc
    sudo docker-compose build

4. Run the container

.. code-block:: console
    :caption: On the x8

    sudo docker-compose up

You should see output like this:

.. code-block::

    python-blink-rpc_1  |
    python-blink-rpc_1  | ============================================
    python-blink-rpc_1  | ==       Portenta X8 M4 output            ==
    python-blink-rpc_1  | ============================================
    python-blink-rpc_1  |
    python-blink-rpc_1  | count:  1
    python-blink-rpc_1  | led:  1
    python-blink-rpc_1  | count:  2
    python-blink-rpc_1  | led:  0
    python-blink-rpc_1  | count:  3
    python-blink-rpc_1  | led:  1
