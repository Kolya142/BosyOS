import mido

DEFAULT_BPM = 120  

def midi_to_notes_and_durations(midi_file):
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

    notes = []
    durations = []
    i = 0
    n = False
    for track in mid.tracks:
        current_time = 0
        for msg in track:
            current_time += msg.time
            if msg.type == 'note_on' and msg.velocity > 0:
                durations.append(int(current_time * ms_per_tick))
                notes.append(msg.note)
                current_time = 0
            i += 1
            if i >= 700:
                n = True
                break
        if n:
            break

    print("\nU16 tones[] = {", ", ".join(map(str, notes)), "};")
    print("\nU16 durations[] = {", ", ".join(map(str, durations)), "};")

midi_to_notes_and_durations("/home/sony/Downloads/touhou-bad-apple-20220916160557-nonstop2k.com.mid")
