import mido

DEFAULT_BPM = 120  

def midi_to_durations(midi_file):
    mid = mido.MidiFile(midi_file)

    bpm = DEFAULT_BPM
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                bpm = mido.tempo2bpm(msg.tempo)
                break

    print(f"Detected BPM: {bpm}")

    ticks_per_beat = mid.ticks_per_beat
    ms_per_tick = (60000 / bpm) / ticks_per_beat

    durations = []

    for track in mid.tracks:
        current_time = 0
        for msg in track:
            current_time += msg.time
            if msg.type == 'note_on' and msg.velocity > 0:
                durations.append(int(current_time * ms_per_tick))
                current_time = 0
    print("\nU16 tones[] = {", ", ".join(map(str, durations)), "};")

midi_to_durations("/home/sony/Downloads/Sonic the Hedgehog - Green Hill Zone.mid")
