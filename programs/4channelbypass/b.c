/** @file simple_client.c
 *
 * @brief This simple client demonstrates the most basic features of JACK
 * as they would be used by many applications.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#error "sorry, 4channelbypass is not available for Windows."
#endif
#include <jack/jack.h>

jack_port_t *input_port[4];
jack_port_t *output_port[4];
jack_client_t *client;

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int
process (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *in[4], *out[4];
	int i;
        for (i=0; i<4; i++) {
	in[i] = jack_port_get_buffer (input_port[i], nframes);
	out[i] = jack_port_get_buffer (output_port[i], nframes);
	memcpy (out[i], in[i],
		sizeof (jack_default_audio_sample_t) * nframes);
       }
	return 0;      
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}

int
main (int argc, char *argv[])
{
	const char **ports;
	const char *client_name = "mccapture";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/

	jack_set_process_callback (client, process, 0);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* display the current sample rate. 
	 */

	printf ("engine sample rate: %" PRIu32 "\n",
		jack_get_sample_rate (client));

	/* create two i mean eight ports */

	input_port[0] = jack_port_register (client, "input0",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);
	output_port[0] = jack_port_register (client, "output0",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);
        input_port[1] = jack_port_register (client, "input1",
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsInput, 0);
        output_port[1] = jack_port_register (client, "output1",
                                          JACK_DEFAULT_AUDIO_TYPE,
                                          JackPortIsOutput, 0);
        input_port[2] = jack_port_register (client, "input2",
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsInput, 0);
        output_port[2] = jack_port_register (client, "output2",
                                          JACK_DEFAULT_AUDIO_TYPE,
                                          JackPortIsOutput, 0);
        input_port[3] = jack_port_register (client, "input3",
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsInput, 0);
        output_port[3] = jack_port_register (client, "output3",
                                          JACK_DEFAULT_AUDIO_TYPE,
                                          JackPortIsOutput, 0);


	if ((input_port == NULL) || (output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	/* keep running until stopped by the user */

	sleep (-1);

	/* this is never reached but if the program
	   had some other way to exit besides being killed,
	   they would be important to call.
	*/

	jack_client_close (client);
	exit (0);
}
