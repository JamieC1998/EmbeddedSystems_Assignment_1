//Jamie Cotter R00154256

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>


/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

//This block of code executes when the collector receives a broadcast
static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from){

}

//This struct lets the collector know what method to invoke when a broadcast is received
static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static struct broadcast_conn broadcast;

/*
This block of code executes when the collector receives a unicast
It prints out the average temperature received from what node
*/
static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from) {
  printf("Unicast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}

//This struct lets the collector know what method to invoke when a unicast is received
static const struct unicast_callbacks unicast_callbacks = { recv_uc };
static struct unicast_conn uc;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{

  //Creating our ETimer
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  /*
  Here we open a broadcast connection on band 146
  and open a unicast on band 140
  */
  broadcast_open(&broadcast, 146, &broadcast_call);

  unicast_open(&uc, 140, &unicast_callbacks);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /*
    Every 2 - 4 seconds we copy a string into the packetbuffer
    and then send out the packet in a broadcast
    */
    packetbuf_copyfrom("Hello", 6);
    broadcast_send(&broadcast);

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
