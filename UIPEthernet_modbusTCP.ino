#define MACADDRESS 0x00,0x01,0x02,0x03,0x04,0x05
#define MYIPADDR 192,168,1,6
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,1,1
#define MYGW 192,168,1,1
#define LISTENPORT 502
#define UARTBAUD 115200

#include <UIPEthernet.h>
#include "utility/logging.h"


EthernetServer server = EthernetServer(LISTENPORT);


byte modbusTcpRequest[200];
byte modbusTcpResponse[100];
uint32_t checkFunction;
uint16_t checkAddress;

void setup() {
  Serial.begin(115200);
  uint8_t mac[6] = {MACADDRESS};
  uint8_t myIP[4] = {MYIPADDR};
  uint8_t myMASK[4] = {MYIPMASK};
  uint8_t myDNS[4] = {MYDNS};
  uint8_t myGW[4] = {MYGW};
  Ethernet.begin(mac, myIP, myDNS, myGW, myMASK);

  server.begin();
  Serial.print("Server start...");
}

void loop() {
  if (EthernetClient client = server.available()) {
    uint8_t i = 0;
    while ((client.available()) > 0)
    {
      byte c = client.read();
      modbusTcpRequest[i] = c;
      Serial.print(c, HEX); Serial.print(" "); 
      i++;
      if (i == 12)
      {
        Serial.println(" ");
        switch (modbusTcpRequest[7])   // check function such as 0x04, 0x01
        {
          case 0x04:
            checkAddress = modbusTcpRequest[8] << 8 | modbusTcpRequest[9];
            if (checkAddress == 0x7530)     // address  30000
            {
              uint8_t lengths = modbusTcpRequest[11];
              modbusTcpResponse[0] = modbusTcpRequest[0];   // Transaction identifier byte HIGH
              modbusTcpResponse[1] = modbusTcpRequest[1];   // Transaction identifier byte LOW

              modbusTcpResponse[2] = modbusTcpRequest[2];   // Protocol identifier 0x00
              modbusTcpResponse[3] = modbusTcpRequest[3];   // Protocol identifier 0x00

              modbusTcpResponse[4] = 0x00;                  // Message length byte HIGH
              modbusTcpResponse[5] = (lengths * 2 ) + 3 ;   // Message length byte LOW

              modbusTcpResponse[6] = modbusTcpRequest[6];   // Device address

              modbusTcpResponse[7] = modbusTcpRequest[7];   // Function code

              modbusTcpResponse[8] = lengths * 2;           // Number of bytes

              modbusTcpResponse[9] = 0x30;    // Regiter value HI
              modbusTcpResponse[10] = 0x11;   // Regiter value LO

              for (i = 0; i <= 10; i++) {
                client.write(modbusTcpResponse[i]);
              }
            }
            break;
          default:
            break;
        }
        memset(modbusTcpRequest, 0, sizeof(modbusTcpRequest));
      }
    }
  }
}
