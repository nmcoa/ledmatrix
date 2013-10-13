/** 
 * Trivial REPly handler copied from ZMQ example: https://raw.github.com/imatix/zguide/master/examples/C/interrupt.c
 *
 * TODO: from https://www.kernel.org/doc/Documentation/spi/spidev_test.c copy the SPI interface
 */


//  Shows how to handle Ctrl-C

#include <zmq.h>
#include <stdio.h>
#include <signal.h>

//  Signal handling
//
//  Call s_catch_signals() in your application at startup, and then
//  exit your main loop if s_interrupted is ever 1. Works especially
//  well with zmq_poll.

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

int main (void)
{
    void *context = zmq_ctx_new ();
    void *socket = zmq_socket (context, ZMQ_REP);
    zmq_bind (socket, "tcp://*:5555");

    s_catch_signals ();
    while (1) {
        //  Blocking read will exit on a signal
        char buffer [255];
        zmq_recv (socket, buffer, 255, 0);
        if (s_interrupted) {
            printf ("W: interrupt received, killing server...\n");
            break;
        }
    }
    zmq_close (socket);
    zmq_ctx_destroy (context);
    return 0;
}
