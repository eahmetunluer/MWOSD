
uint8_t update_crc8(uint8_t crc, uint8_t crc_seed){
  uint8_t crc_u, i;
  crc_u = crc;
  crc_u ^= crc_seed;
  for ( i=0; i<8; i++) crc_u = ( crc_u & 0x80 ) ? 0x7 ^ ( crc_u << 1 ) : ( crc_u << 1 );
  return (crc_u);
}

uint8_t get_crc8(uint8_t *Buf, uint8_t BufLen){
  uint8_t crc = 0, i;
  for( i=0; i<BufLen; i++) crc = update_crc8(Buf[i], crc);
  return (crc);
}

void serialESCreceive(uint8_t c){
  static uint8_t SerialBuf[10];
  MwHeading++;
  ESC_loopTime = micros();
  if (Settings[S_CUSTOM] == 0) Settings[S_CUSTOM] = 1;
  if(receivedBytes > 10) receivedBytes = 0;
  if(receivedBytes < 10){ // collect bytes
    SerialBuf[receivedBytes] = c;
    receivedBytes++;
  }  
  if(receivedBytes == 10){ // transmission complete      
    uint8_t crc8 = get_crc8(SerialBuf, 9); // get the 8 bit CRC
  if(crc8 != SerialBuf[9]) return; // transmission failure       
    ESC_telemetrie[0] = SerialBuf[0]; // temperature
    ESC_telemetrie[1] = ((SerialBuf[1]<<8)|SerialBuf[2])/10; // voltage
    ESC_telemetrie[2] = ((SerialBuf[3]<<8)|SerialBuf[4])/10; // Current
    ESC_telemetrie[3] = (SerialBuf[5]<<8)|SerialBuf[6]; // used mA/h
    ESC_telemetrie[4] = ((SerialBuf[7]<<8)|SerialBuf[8]) / Settings[S_CUSTOM]; // eRpM *100  for 14 pole
    temperature = ESC_telemetrie[0];
    voltage     = ESC_telemetrie[1];
    amperage    = ESC_telemetrie[2]; 
    amperagesum = ESC_telemetrie[3]*360;   
    rpm         = ESC_telemetrie[4];
    receivedBytes = 0;
  }
}
