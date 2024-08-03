'''

Composizione messaggi MIDI
[0xAB, C, D]
A = identifica il tipo di messaggio da inviare (es. nota_on, nota_off, program_change ecc)
B = identifica su che canale inviare il messaggio
    possiamo associare un canale per ogni tipologia di strumento e li possiamo gestire indipendentemente
C e D = sono i parametri relativi al messaggio A (es. per nota_on che tipo di nota suonare e a che velocità) 

'''

import rtmidi
import time

# Crea un oggetto MidiOut
midiout = rtmidi.MidiOut()

# Apre la prima porta MIDI disponibile
available_ports = midiout.get_ports()

if available_ports:
    midiout.open_port(0)
else:
    midiout.open_virtual_port("My virtual output")

# Funzione per inviare un messaggio di Program Change
def send_program_change(midiout, canale, program_number):
    program_change = [0xC0 + canale, program_number]  
    midiout.send_message(program_change)

# Funzione per inviare un messaggio di Nota On
def send_note_on(midiout, canale, note_number, velocity):
    note_on_status = 0x90 + canale
    nota_on = [note_on_status, note_number, velocity]
    midiout.send_message(nota_on)

# Funzione per inviare un messaggio di Nota Off
def send_note_off(midiout, canale, note_number, velocity):
    note_off_status = 0x80 + canale
    nota_off = [note_off_status, note_number, velocity]
    midiout.send_message(nota_off)

# Funzione per inviare un messaggio di Control Change
def send_control_change(midiout, canale, controller, value):
    control_change_status = 0xB0 + canale
    control_change = [control_change_status, controller, value]
    midiout.send_message(control_change)


# INIZIO ---------------------------------------------------------------------------------------------------------

# Set dei canali MIDI (esempio: ch0=piano acustico, ch1=chitarra acustica, ch2=violino)
send_program_change(midiout, 0, 0)   # Canale 0: Piano Acustico
send_program_change(midiout, 1, 24)  # Canale 1: Chitarra Acustica
send_program_change(midiout, 2, 40)  # Canale 2: Violino
                                     # canale 9: Batteria e percussioni di default

# Definiamo una sequenza di eventi che inviino dei messaggi MIDI (come inizio e fine di una nota) 
# per simulare le stesse dinamiche del real time
start_time = time.time()
# costruisco un array di eventi
event_schedule = [
    # ogni evento è composto da: 
    #  - istante in cui viene triggerato
    #  - tipo di messaggio MIDI
    #  - parametri del messaggio MIDI 

    # parametri MIDI per nota_on:
    #  - oggetto MIDI
    #  - canale
    #  - nota
    #  - velocità

    # parametri MIDI per control_change:
    #  - oggetto MIDI
    #  - canale
    #  - tipo di controllo
    #  - valore 

    (0.0, send_note_on, [midiout, 0, 35, 100]),  # nota on
    (1.0, send_note_off, [midiout, 0, 35, 100]),  # nota off
    (2, send_note_on, [midiout, 2, 42, 110]),  # nota on
    (3, send_control_change, [midiout, 2, 7, 150]),  # change volume
    (4, send_control_change, [midiout, 2, 7, 40]),  # change volume
    (5, send_note_off, [midiout, 2, 42, 100]),  # nota off
]

# avvio simulazione
while event_schedule:
    current_time = time.time() - start_time
    for event in event_schedule:
        event_time, event_function, event_args = event
        if current_time >= event_time:
            print(event_time)
            event_function(*event_args)
            event_schedule.remove(event)

    # Aggiungi un piccolo ritardo per evitare di sovraccaricare la CPU
    time.sleep(0.01)

# Chiude la porta MIDI
midiout.close_port()
