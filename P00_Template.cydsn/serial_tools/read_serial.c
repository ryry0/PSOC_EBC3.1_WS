#ifdef __linux__
#include <getch.h>
#else
#include <conio.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <serial.h>

#include <packet_hybrid.h>
#include <packet.h>
#include <ged_fsm.h>

#define TEST_PACKET_LENGTH 3
#define DEFAULT_SERIAL "COM3"

sr_port_t serial_port;
bool print_hex = false;
bool print_char = true;
bool raw_write = false;

void sequentialWrite(sr_port_t serial_port, uint8_t *buff, size_t len) {

  for (size_t i = 0; i < len; ++i) {
      sr_WritePort(serial_port, &buff[i], 1);
  }
}

static void getPacket(pkt_generic_t *packet) {
  uint8_t input_byte = 0;

  //wait for whole packet to populate
  do {
    while(sr_ReadPort(serial_port, &input_byte, 1) == 0);
  } while (!pkt_readByte(packet, input_byte));
  pkt_decodePacket(packet);
}

static void sendStateMsg(uint8_t state) {
  pkt_generic_t state_message;
  pkt_init(&state_message);
  pkt_setHeader(&state_message, PKT_SET_STATE, sizeof(pkt_set_state_t));
  pkt_set_state_t *set_state_payload = pkt_interp(pkt_set_state_t,
      state_message);

  set_state_payload->state = state;

  sequentialWrite(serial_port, pkt_encodeBuffer(&state_message),
      pkt_getTotalLength(&state_message));
}

void *readThread(void *args) {
  char input = 0;
  pkt_generic_t packet;

  for (;;) {
    if (print_char) {
      if (sr_ReadPort(serial_port, &input, 1) != 0) {
        printf("%c", input);
        fflush(stdout);
      }
    }
    else if (print_hex) {
      if (sr_ReadPort(serial_port, &input, 1) != 0) {
        printf("%2.2X", input);
        fflush(stdout);
      }
    }
    else {
      getPacket(&packet);
      if (packet.type == PKT_LOG_CAN_DATA) {
        size_t length = pkt_getTotalLength(&packet);
        /*
        pkt_log_can_data_t *module_data = pkt_interp(pkt_log_can_data_t, packet);
        printf("%x", module_data->state);
        */

        for (size_t i = length - 5; i < length; ++i) {
          printf("%x ", packet.data[i]);
        }

        printf("\n");
      }
      pkt_clear(&packet);
    }
  }
}


void writeThread() {
  char user_input = 0;
  char buff[100] =  "testingtestingab";
  char buff2[100] = "abcdefgasfecegle";
  char buff3[100] = "\0\3\0s";
  uint8_t force_syn = 0xff;
  char buf_generic[10] = "\0\0\0\0";

  pkt_generic_t packet;
  pkt_init(&packet);

  /*
  pkt_setHeader(&state_message, PKT_SET_STATE, sizeof(pkt_set_state_t));
  pkt_set_state_t *set_state_payload = pkt_interp(pkt_set_state_t,
      state_message);
  set_state_payload->state = state;

  mvu_sendData(model, pkt_encodeBuffer(packet), pkt_getTotalLength(packet));


  pkt_setHeaderTypeOnly(&start_message, PKT_DATA_START);
  */

  user_input = getch();

  switch (user_input) {
    case '!':
      print_char = !print_char;
      printf("char output: %d", print_char);
      break;

    case 3:
      print_hex = !print_hex;
      printf("hex output: %d", print_hex);
      break;

    case 2:
      raw_write = !raw_write;
      printf("raw write: %d", raw_write);
      break;

    case '@':
      sr_WritePort(serial_port, buff, 17);
      break;

    case 'f':
      sequentialWrite(serial_port, &force_syn, 1);
      break;

    case 'l':
      sendStateMsg(GED_LOCKED);
      break;

    case 'u':
      sendStateMsg(GED_UNLOCKED);
      break;

    case '1':
      sendStateMsg(GED_LEFT_DBL_STANCE);
      break;

    case '2':
      sendStateMsg(GED_RIGHT_STEP);
      break;

    case '3':
      sendStateMsg(GED_RIGHT_DBL_STANCE);
      break;

    case '4':
      sendStateMsg(GED_LEFT_STEP);
      break;

    case 's':
      //sequentialWrite(serial_port, "\0\0\0s", 4);
      pkt_setHeaderTypeOnly(&packet, PKT_DATA_START);

      sequentialWrite(serial_port, pkt_encodeBuffer(&packet),
          pkt_getTotalLength(&packet));

      break;

    default:
      if (raw_write) {
        sequentialWrite(serial_port, &user_input, 1);
      }
      else {
        buf_generic[3] = user_input;
        sequentialWrite(serial_port, buf_generic, 4);
      }
  }
}

//signal to handle
int main(int argc, char ** argv) {

  char buff1[100] = {0};
  char buff2[100] = {0};
  uint8_t input = 0;
  uint8_t option = 0;


  pthread_t read_thread;

  serial_port = sr_OpenPort(DEFAULT_SERIAL);
  if (serial_port == NULL) {
    printf("Input serial port address:");
    scanf("%s", buff1);
    serial_port = sr_OpenPort(buff1);
  }
  else {
    printf("Default serial opened\n");
  }

  if (serial_port == NULL)
    return 1;

  printf("Serial opened\n");
  if (sr_InitPort(serial_port, 115200) == false)
    return 1;

  printf("Serial Initialized\n");
  pthread_create(&read_thread, NULL, readThread, NULL);

  printf("Thread spawned\n");


  printf("Entering writeThread\n");
  for (;;)
    writeThread();

  pthread_join(read_thread, NULL);

  return 0;
}
