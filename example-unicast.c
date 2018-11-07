//Jamie Cotter R00154256

#include "contiki.h"
#include "net/rime.h"

#include "dev/button-sensor.h"

#include "dev/sht11-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;

/*---------------------------------------------------------------------------*/


static int print_val;

static int values[4];

static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from){
  
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

  rimeaddr_t destination;

  char str[10];

  sprintf(str, "%d", print_val);

  packetbuf_copyfrom(str, 5);

  destination.u8[0] = from->u8[0];
  destination.u8[1] = from->u8[1];

  if(!rimeaddr_cmp(&destination, &rimeaddr_node_addr)) {
      unicast_send(&uc, &destination);
  }

  

}

static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static struct broadcast_conn broadcast;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);

  broadcast_open(&broadcast, 146, &broadcast_call);

  unicast_open(&uc, 140, &unicast_callbacks);

  int i = 0;

  for(i = 0; i < 4; i++){
	values[i] = 0;
  }

  while(1) {

    static struct etimer et;

    rimeaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    int store = print_val = sht11_sensor.value(SHT11_SENSOR_TEMP);

    

    for(i = 0; i < 4; i++){
	if((i + 1) < 4){
	  values[i] = values[i + 1];

	}
    }

    values[3] = store;

    store = 0;

	for(i = 0; i < 4; i++){
		store = store + values[i];
	}

        print_val = store / 4;



  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/