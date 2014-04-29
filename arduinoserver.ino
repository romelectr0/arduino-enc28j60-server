// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
 
#include <EtherCard.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
int ledPin2 = 9;
char page[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
  "</title><style type='text/css'>body {background-color: black;}#button{margin-top: 300px;margin-left: 700px;color: white;background-color: blue;border-radius: 10px;width: 100px;}</style></head>"
  "<body>"
      "<a href='http://192.168.1.200/?led=on'><input id='button' type='submit' value='active'/></a>"
    "</em></p>"
  "</body>"
"</html>"
;

void setup(){
  Serial.begin(9600);
  Serial.println("\n[backSoon]");
  pinMode(ledPin2,OUTPUT);
  digitalWrite(ledPin2,LOW);
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
}

void loop(){
   word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  // wait for an incoming TCP packet, but ignore its contents
  if (ether.packetLoop(ether.packetReceive())) {
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }
  if(strstr((char *)Ethernet::buffer + pos, "GET /?led=on") != 0) {
      Serial.println("Received ON command");
      digitalWrite(ledPin2, HIGH);
      delay(1000);
      digitalWrite(ledPin2,LOW);
      
  }
}
