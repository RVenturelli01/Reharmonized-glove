import time
import rtmidi

midiout = rtmidi.MidiOut()
available_ports = midiout.get_ports()

if available_ports:
    midiout.open_port(0)
else:
    midiout.open_virtual_port("My virtual output")

with midiout:
    #program_change = [0xC5, 40]

    # Invia il messaggio "Program Change"
    #midiout.send_message(program_change)

    #note_on = [0x95, 70, 112] # channel 1, middle C, velocity 112
    #note_off = [0x85, 70, 0]
    #midiout.send_message(note_on)
    #time.sleep(0.5)
    #midiout.send_message(note_off)
    #time.sleep(0.1) 

    program_change = [0xC0, 13]
    midiout.send_message(program_change)
    midiout.send_message([0x90, 58,115])
    time.sleep(1) 
    midiout.send_message([0x90, 61,115])
    time.sleep(1) 
    midiout.send_message([0x90, 63,115])
    time.sleep(1) 

del midiout
