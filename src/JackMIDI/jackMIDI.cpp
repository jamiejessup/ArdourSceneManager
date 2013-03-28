/*
 * Author: Jamie Jessup
 * File: jackMIDI.cpp
 * Description: Source Code for handling MIDI Events related to Ardour mixer levels
 */

#include "../ASMView.h"
#include "../Scene/Scene.h"

Jack::Jack(ASMView* mw) :
		pASMView(mw) {
	bpm = 120.0;

	if ((client = jack_client_open("Ardour Scene Manager", JackNullOption, NULL))
			== 0) {
		std::cout << "jack server not running?" << std::endl;
	}

	inputPort = jack_port_register(client, "midi_in", JACK_DEFAULT_MIDI_TYPE,
			JackPortIsInput, 0);

	outputPort = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
			JackPortIsOutput, 0);

	jack_set_process_callback(client, staticProcess, static_cast<void*>(this));

	playbackIndex = 0;

	activate();

}

Jack::~Jack() {
}

void Jack::activate() {
	if (jack_activate(client) != 0) {
		std::cout << "cannot activate client" << std::endl;
		return;
	}
	/**
	 * Here we automatically connect to ardour's midi ports if we can
	 */
	const char **ports;
	ports = jack_get_ports(client, NULL, JACK_DEFAULT_MIDI_TYPE, 0);
	int i = 0;
	while (ports[i] != NULL) {
		if (strcmp(ports[i], ARDOUR_MIDI_CTL_IN) == 0) {
			jack_connect(client, jack_port_name(outputPort), ports[i]);
		}
		if (strcmp(ports[i], ARDOUR_MIDI_CTL_OUT) == 0) {
			jack_connect(client, ports[i], jack_port_name(inputPort));
		}
		i++;
	}

}

int Jack::staticProcess(jack_nframes_t nframes, void *arg) {
	return static_cast<Jack*>(arg)->process(nframes);
}

int Jack::process(jack_nframes_t nframes) {
	jack_midi_event_t in_event;

	Scene* pMyScene = pASMView->getScene();

	void *inputPortBuf = jack_port_get_buffer(inputPort, nframes);
	void *outputPortBuf = jack_port_get_buffer(outputPort, nframes);

	/* Clear the buffer so we don't send the same thing all the time */
	jack_midi_clear_buffer(outputPortBuf);

	//Get the transport state

	// input: get number of events, and process them.
	jack_nframes_t event_count = jack_midi_get_event_count(inputPortBuf);
	if (event_count > 0) {
		for (unsigned int i = 0; i < event_count; i++) {
			jack_midi_event_get(&in_event, inputPortBuf, i);
			if ((int) *(in_event.buffer) == CC_MASK) {
				//Calculate track number from the CC #. Subtract one since vector of tracks is
				//numbered starting at zero
				int trackNo = (int) *(in_event.buffer + sizeof(char)) - 1;
				char volume = *(in_event.buffer + 2 * sizeof(char));
				if (pMyScene != NULL) {
					if (trackNo == MASTER_CC || trackNo == MASTER_CC2) {
						pMyScene->master.setTrackGain(volume);
						pMyScene->master.setModified();
					} else {
						pMyScene->tracks[trackNo].setTrackGain(volume);
						pMyScene->tracks[trackNo].setModified();
					}
					if (pASMView != NULL) {
						pASMView->showSceneDetails();
					}
				}
			}
		}
	}

	//Try and lock the resource of data to be sent, if not next time
	if (pthread_mutex_trylock(&txMutex) == 0) {
		// go through data to be sent!
		for (unsigned int i = 0; i < nframes; i++) {
			if (eventVector.size() > 0) {
				MidiEvent midiEvent = eventVector.front();
				/* Write midi data to the buffer */
				unsigned char* buffer = jack_midi_event_reserve(outputPortBuf,
						0, 3);

				if (buffer == 0) {
					cout << "Midi write failed -- write buffer == 0" << endl;
				} else {
					buffer[0] = midiEvent.data[0];
					buffer[1] = midiEvent.data[1];
					buffer[2] = midiEvent.data[2];
				}
				//Take the midi event out of the queue to be written
				eventVector.erase(eventVector.begin());
			}
		}
		//Give up the resource
		pthread_mutex_unlock(&txMutex);
	}

	return 0;
}

float Jack::midiCCToFaderGain(char dataByte) {
	//Make up gain to make highest MIDI CC return +6db, and 99 return 0dB
	static float gain = 6 / log10(127 / (float) 99.0);
	if (dataByte == 0) {
		return -600;
	} else
		return gain * log10((int) dataByte / (float) 99.0);
}

