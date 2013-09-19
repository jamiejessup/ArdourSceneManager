/*
This file is part of Ardour Scene Manager.
Copyright 2013, Jamie Jessup

Ardour Scene Manager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ardour Scene Manager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ardour Scene Manager. If not, see <http://www.gnu.org/licenses/>.
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

            //Go through the list of tracks comparing ID to CC number
            char id = *(in_event.buffer + sizeof(char));
            char gain = *(in_event.buffer + 2 * sizeof(char));

            if ((int) *(in_event.buffer) == CC_MASK) {
                //This is a message for master, a track, or a bus
                bool found = false;
                if (pMyScene != NULL) {
                    if (id == MASTER_CC || id == MASTER_CC2) {
                        pMyScene->master.setGain(gain);
                        pMyScene->master.setModified(true);
                        found = true;
                        if (pASMView != NULL) {
                            pASMView->showSceneDetails();
                        }
                    }

                    if(!found) {
                        for(unsigned int i =0; i<pMyScene->tracks.size(); i++) {
                            if(id == pMyScene->tracks[i].getId()) {
                                pMyScene->tracks[i].setGain(gain);
                                pMyScene->tracks[i].setModified(true);
                                found = true;
                                if (pASMView != NULL) {
                                    pASMView->showSceneDetails();
                                }
                            }
                        }
                    }

                    //Go through the busses
                    if(!found) {
                        for(unsigned int i =0; i<pMyScene->busses.size(); i++) {
                            if(id == pMyScene->busses[i].getId()) {
                                pMyScene->busses[i].setGain(gain);
                                pMyScene->busses[i].setModified(true);
                                found = true;
                                if (pASMView != NULL) {
                                    pASMView->showSceneDetails();
                                }
                            }
                        }
                    }
                }
            }
            //Check to see if the first four most significant bits match CC_MASK
            // The remaining four bits is the send id.
            else if () {
                //do stuff
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

