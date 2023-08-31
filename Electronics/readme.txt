Disclamer:
    These are my own electronic designs. You can use them however you like, but i take no
    responsibilty for them. It's up to you to use them well. 

Versions:
    32Bit shift register - This is my first iterration, i had some problems with them due to
    high frequency signals and using wires instead of pcb traces resulting in reflections.
    I have changed the software/hardware interface, so these may work, but i haven't tested this.

    32Bit shift register - This is the 2nd iterration. Fixed the reflexions and works flaweless
    for me. I recommond using JXT XH connectors and not dupont, since the touch screen can't handle
    iffy wiring. Just put a big cap i use 470uF near the power input. Other components are smd and
    you can have them assembled.
    Upload the Gerbers "32bit sr v2.0.zip" from the "32Bit SR Gerbers" folder to jlcpcb.com
    Turn on pcb assembly
    Upload the BOM-file "32Bit shift register.xlsx" from the "32Bit SR Assembly" folder
    Upload the CPL-file "32Bit shift register-all-pos.xlsx" from the "32Bit SR Assembly" folder
    Verify the components are in stock and place correctly.
    Order pcb

    Mega-Cube-Board - I wanted more functionality so i developed this board. The LCD can be soldered
    directly on this board without wires, so has more stability. It also has a buck converter and
    esp32's on it for gaming. A microphone and can drive speakers. The price to order this board
    was over $100 so i stopped working on it.

WARNING:
    The teensy 4.0 has no protection on the USB power. So powering through USB and a power supply
    may blow things up (I don't know because i'm carefull). You can use a usb cable and cut the 5V
    line. Or solder in a diode somewhere (look on pjrc site how to do that). Also never connect the
    5V power from the supplies together (I don't know what will happen, but probably real fireworks)
    All grounds from all power supplies must be tied together (NOT to the earth protective ground)
    If things are flickering you most likely have a disconnected wire or a grounding issue somewhere.

    Hope this clears thing up. Happy soldering.
    Malt Whiskey