*************
Get connected
*************

Access board via adb
====================

https://docs.arduino.cc/tutorials/portenta-x8/out-of-the-box#adb

.. _arduino-ide:

Install option 1 - Use Arduino provided ADB binary
--------------------------------------------------

Need to `add Portenta board to Arduino IDE <https://support.arduino.cc/hc/en-us/articles/360016119519-Add-boards-to-Arduino-IDE>`_ which will install adb binary here:

    ~/LibraryArduino15/packages/arduino/tools/adb/32.0.0/adb

Can run adb directory by calling via cmd -> might need to chmod 744 first

Can also add to $PATH via:

.. code-block:: console

    ❯ export PATH="$PATH:/Users/joel/Library/Arduino15/packages/arduino/tools/adb/32.0.0"

Install option 2 - Install ADB on your machine
----------------------------------------------

You can instead `install adb to your system using brew <https://formulae.brew.sh/cask/android-platform-tools>`_ which will handle all the $PATH config for you.


Commands
--------

**Check if the Portenta is accessible**

.. code-block:: console

    ❯ adb devices
    List of devices attached
    1421C209DAB6FAD9	device

**Connect via adb**

.. code-block:: console

    ❯ adb shell
    fio@portenta-x8-1421c209dab6fad9:/var/rootdirs/home/fio$

Access board via SSH
====================
https://docs.arduino.cc/tutorials/portenta-x8/out-of-the-box#ssh

You can get the IP address either by looking for Portenta-x8-XXX on your network or using the adb command to get either the hostname or ip address.

**Get hostname**

.. code-block:: console

    ❯ adb shell
    fio@portenta-x8-1421c209dab6fad9:/var/rootdirs/home/fio$ hostname
    portenta-x8-1421c209dab6fad9

**Connect with hostname**

.. code-block:: console

    ❯ ssh fio@portenta-x8-1421c209dab6fad9
    fio@portenta-x8-1421c209dab6fad9's password: fio

    fio@portenta-x8-1421c209dab6fad9:~$

.. note::

    If you get a connection error of some sort trying to connect via SSH, you may need to update the OS to the latest version first.


.. _web-interface:

Access board via web interface
==============================
https://docs.arduino.cc/tutorials/portenta-x8/out-of-the-box#connecting-to-the-board

Go to: http://192.168.8.1/

If you press `Launch Shell` you'll go into an Alpine container as root.

.. note::

    This isn't actual OS of the machine, just a container running inside.
