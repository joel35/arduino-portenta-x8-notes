*********************
Update Portenta x8 OS
*********************

Check currently installed OS
============================

.. code-block:: console

    $ cat /etc/os-release

    ID=lmp-xwayland
    NAME="Linux-microPlatform XWayland"
    VERSION="3.3.2-399-83-21-g3ad61e0"
    VERSION_ID=3.3.2-399-83-21-g3ad61e0
    PRETTY_NAME="Linux-microPlatform XWayland 3.3.2-399-83-21-g3ad61e0"
    HOME_URL="https://foundries.io/"
    SUPPORT_URL="https://support.foundries.io/"
    LMP_MACHINE="portenta-x8"
    LMP_FACTORY="arduino"
    LMP_FACTORY_TAG="devel"

``VERSION="3.3.2-399-83-21-g3ad61e0"`` means release 399

Flash new OS
============

Official tutorial: https://docs.arduino.cc/tutorials/portenta-x8/image-flashing

Need to change both DIP switches on the breakout to ON first.

All OS releases should be available on GitHub
https://github.com/arduino/lmp-manifest/releases

MacOS terminal commands to prepare to flash version 436:

.. code-block:: console

    ❯ wget https://github.com/arduino/lmp-manifest/releases/download/436-portenta-x8/436.tar.gz
    ❯ mkdir 436
    ❯ tar -xvf 436.tar.gz -C 436/
    ❯ cd 436
    ❯ mkdir mfgtool-files-portenta-x8
    ❯ tar -xvf mfgtool-files-portenta-x8.tar.gz -C mfgtool-files-portenta-x8/
    ❯ gunzip lmp-vendor-arduino-image-portenta-x8.wic.gz
    ❯ ls

    imx-boot-portenta-x8
    lmp-vendor-arduino-image-portenta-x8.wic
    mfgtool-files-portenta-x8
    sit-portenta-x8.bin
    u-boot-portenta-x8.itb

All the necessary utilities to flash the firmware are in the imx-boot-portenta-x8 directory. This includes the uuu binary to run.

If you prefer, you can actually install this to the system (in MacOS) using brew.
https://formulae.brew.sh/formula/uuu

Either way, cd into the tools directory

.. code-block:: console

    ❯ cd mfgtool-files-portenta-x8

.. note::

    Replace ``uuu`` with ``./uuu_mac`` if using provided binary

Using uuu, check if the Portenta is connected:

.. code-block:: console

    ❯ uuu -lsusb

    Connected Known USB Devices

        Path	 Chip	 Pro	 Vid	 Pid	 BcdVersion
        ==================================================
        0:1	 MX8MM	 SDP:	 0x1FC9	0x0134	 0x0101

Run the update script as sudo (important)

.. code-block:: console

    ❯ sudo uuu full_image.uuu

    Success 1    Failure 0

    0:1     12/12 [Done                                  ] FB: done

After that switch off, change the DIP switches to OFF and power on.

Check OS version to ensure was successful.

.. code-block:: console

    $ cat /etc/os-release

    ID=lmp-xwayland
    NAME="Linux-microPlatform XWayland"
    VERSION="3.3.2-436-399-production-portenta-x8"
    VERSION_ID=3.3.2-436-399-production-portenta-x8
    PRETTY_NAME="Linux-microPlatform XWayland 3.3.2-436-399-production-portenta-x8"
    HOME_URL="https://foundries.io/"
    SUPPORT_URL="https://support.foundries.io/"
    LMP_MACHINE="portenta-x8"
    LMP_FACTORY="arduino"
    LMP_FACTORY_TAG="devel"

``VERSION="3.3.2-436-399-production-portenta-x8"`` means release 436
