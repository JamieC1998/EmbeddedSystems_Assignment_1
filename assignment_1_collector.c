#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>

//--------------------------------------------------
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
//--------------------------------------------------

static void recv_uc(struct unicast_conn *c, const linkaddr_t *from){
  
	printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);


}

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {l }

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

static const struct unicast_callbacks unicast_callbacks = {recv_uc};

//Unicast connection for receiving temperature
static struct unicast_conn uc;

//Broadcast connection for requesting temperature 
static struct broadcast_conn broadcast;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data){
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  unicast_open(&uc, 146, &unicast_callbacks);

  broadcast_open(&broadcast, 146, &broadcast_call);

  while(1) {

    static struct etimer et;
    linkaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND * 4);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	/*
	Broadcast signal goes here
	*/

    broadcast_send(&broadcast);

  }

  PROCESS_END();
}
