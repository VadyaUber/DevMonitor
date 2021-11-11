#include "Mercury.h"

Merc::Merc(string devUSB, uint32_t baud_rate)
{
    openserialport(devUSB, baud_rate);
}

int Merc::openserialport(string devUSB, uint32_t baud_rate)
{
    //открытие порта
    serial_port = open("/dev/ttyS2", O_RDWR);
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }
    //очистка "буфера"
    int result = tcflush(serial_port, TCIOFLUSH);
    if (result)
    {
        perror("tcflush failed");  // just a warning, not a fatal error
    }
    //настройка порта
    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    //Set in/out baud rate to be 9600
    //cfsetispeed(&tty, B9600);
    //cfsetospeed(&tty, B9600);

    switch (baud_rate)
    {
    case 4800:   cfsetospeed(&tty, B4800);   break;
    case 9600:   cfsetospeed(&tty, B9600);   break;
    case 19200:  cfsetospeed(&tty, B19200);  break;
    case 38400:  cfsetospeed(&tty, B38400);  break;
    case 115200: cfsetospeed(&tty, B115200); break;
    default:
        printf("херню ввёл а не частоту: %u ,поставлю 9600 по дефолту.\n", baud_rate);
        cfsetospeed(&tty, B9600);
        break;
    }
    cfsetispeed(&tty, cfgetospeed(&tty));


    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }
    usleep(10000);
    return serial_port;
}

int Merc::writeserial(char* buff, size_t size)
{
    write(serial_port, buff, size);
}

int Merc::readserial(uint8_t buf_read, size_t size)
{
    return read(serial_port, &buf_read, size);
}

int Merc::mercInit()
{
    string zapstr;
    int num_bytes = 0;
    char read_buf[256] = { 0 };
    uint8_t data_raed[256] = { 0 };
    //digitalWrite(4, HIGH);
    for (int i = 0; i < 11; i++) {
        //zapstr.append((char*)&zapros[i]);
        //writeserial(zapstr.c_str(), 1);
        writeserial((char*)&zapros[i], 1);
        //write(serial_port, zapstr.c_str(), 1);
        zapstr = "";
    }
    //delayMicroseconds(50000);
    //digitalWrite(4, LOW);
    //delayMicroseconds(80000);
    try
    {
    uint8_t ln = 10;
    while (ln--)
    {
            num_bytes = read(serial_port, &data_raed, sizeof(data_raed));
            //num_bytes = readserial(*read_buf, sizeof(read_buf));
        
        
        //num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
    }
    }
    catch (const std::exception&)
    {
        printf("errorreadserial \n");
    }
    return 0;

}
uint32_t Merc::getEnergy()
{
    uint16_t crc_tmp;
    int num_bytes = 0;
    uint8_t data_raed[30] = { 0 };

    usleep(20000);

    //data_format[1] = 0x05;
    //data_format[2] = 0x0;
    //data_format[3] = 0x0;
    //data_format[3] = array_phase[0];
    crc_tmp = CRC16_2(datel_format, 4);
    datel_format[4] = crc_tmp;
    datel_format[5] = crc_tmp >> 8;

    //digitalWrite(4, HIGH);
    usleep(1000);
    for (int i = 0; i < 6; i++) {
        writeserial((char*)&datel_format[i], 1);
    }
        
    //usleep(8000);
    tcdrain(serial_port);
    //digitalWrite(4, LOW);
    //usleep(400000);
    uint8_t ln = 10;
    while (ln--) {
        int bytes_available;
        num_bytes = ioctl(serial_port, FIONREAD, &bytes_available);
        if (num_bytes < 0) {
            perror("FIONREAD ioctl failed\n");
            break;
        }
        usleep(50 * 1000L);
        if (bytes_available >= 5)
            break;
    }
    num_bytes = read(serial_port, &data_raed, sizeof(data_raed));


    if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
    }
    if (num_bytes == 0)
    {
        printf("err0\n");
    }
    if (num_bytes > 0)
    {
        uint32_t recks;
        if (CRC16_2(data_raed, 19) == 0) {
            //printf("dad = %d %d %d %d \n", data_raed[1], data_raed[2], data_raed[3], data_raed[4]);
            recks = (((data_raed[1] << 16) |  (data_raed[4] << 8) | (data_raed[3])));
            printf("Energy %d\n", recks);
            enegj = recks;
            return recks;
        }
    }
    usleep(20000);
}

int cnt_zapros = 0;

void Merc::getWats()
{
    uint16_t crc_tmp;
    int num_bytes = 0;
    uint8_t data_red[30] = { 0 };
    do
    {
        data_format[3] = array_phase[cnt_zapros];
        crc_tmp = CRC16_2(data_format, 4);
        data_format[4] = crc_tmp;
        data_format[5] = crc_tmp >> 8;
        for (int i = 0; i < 6; i++) {
            //appha.append((char*)&data_format[i]);
            //writeserial(appha.c_str(), 1);
            writeserial((char*)&data_format[i], 1);
            //write(serial_port, appha.c_str(), 1);
        }
        usleep(40000);

        //num_bytes = readserial(*data_red, 12);
        //num_bytes = readserial(*read_buf, sizeof(read_buf));
        num_bytes = read(serial_port, &data_red, sizeof(data_red));

        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));
        }
        if (num_bytes == 0)
        {
            printf("err0\n");
        }
        if (num_bytes > 0)
        {
            if (CRC16_2(data_red, 6) == 0) {
                power[cnt_zapros] = ((double)((data_red[1] & 0xf) << 16 | (data_red[2]) | (data_red[3] << 8))) / 100;
                //printf("dad = %d %d %d \n", data_red[1], data_red[2], data_red[3]);
                cnt_zapros++;
            }
            if (cnt_zapros >= 9) {
                //usleep(20000);
                //
                ////data_format[1] = 0x05;
                ////data_format[2] = 0x0;
                ////data_format[3] = 0x0;
                ////data_format[3] = array_phase[0];
                //crc_tmp = CRC16_2(datel_format, 4);
                //datel_format[4] = crc_tmp;
                //datel_format[5] = crc_tmp >> 8;
                //for (int i = 0; i < 6; i++) {
                //    writeserial((char*)&datel_format[i], 1);
                //}
                //usleep(20000);
                //num_bytes = read(serial_port, &data_raed, 19);
                //if (num_bytes > 0)
                //{
                //    uint32_t recks;
                //    //recks = (((data_raed[5] << 8) | (data_raed[4])));
                //    printf("dadss = %d %d %d \n", data_raed[1], data_raed[2], data_raed[3]);
                //    if (CRC16_2(data_raed, 19) == 0) {
                //        printf("dad = %d %d %d %d \n", data_raed[1], data_raed[2], data_raed[3], data_raed[4]);
                //        recks = (((data_raed[4] << 8) | (data_raed[3])));
                //        printf("Energ %d\n", recks);
                //    }
                //}
                printf("Merc: ");
                printf("Wat %f %f %f \n", power[0], power[1], power[2]);
                printf("va %f %f %f\n", power[3], power[4], power[5]);
                printf("var %f %f %f\n", power[6], power[7], power[8]);
                //printf("Energ %f\n", recks);
                printf("\n");

            }
        }
        usleep(20000);
    } while (cnt_zapros != 9);
    cnt_zapros = 0;
}

void Merc::getLivePower()
{
    uint16_t crc_tmp;
    int num_bytes = 0;
    uint8_t data_red[30] = { 0 };
    
        data_format[3] = 0x08;
        crc_tmp = CRC16_2(data_format, 4);
        data_format[4] = crc_tmp;
        data_format[5] = crc_tmp >> 8;

        //digitalWrite(4, HIGH);
        usleep(1000);
        for (int i = 0; i < 6; i++) {
            //appha.append((char*)&data_format[i]);
            //writeserial(appha.c_str(), 1);
            writeserial((char*)&data_format[i], 1);
            //write(serial_port, appha.c_str(), 1);
        }
        usleep(8000);
        //tcdrain(serial_port);
        //digitalWrite(4, LOW);
        //usleep(400000);
        uint8_t ln = 10;
        while (ln--) {
            int bytes_available;
            num_bytes = ioctl(serial_port, FIONREAD, &bytes_available);
            if (num_bytes < 0) {
                perror("FIONREAD ioctl failed\n");
                break;
            }
            usleep(50 * 1000L);
            if (bytes_available >= 5)
                break;
        }
        num_bytes = read(serial_port, &data_red, sizeof(data_red));

        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));
        }
        if (num_bytes == 0)
        {
            printf("err0\n");
        }
        if (num_bytes > 0)
        {
            uint32_t recks;
            if (CRC16_2(data_red, 19) == 0) {
                sumpower = ((data_red[1] & 0xf) << 16 | (data_red[2]) | (data_red[3] << 8));
                sumpower = sumpower * 10;
                printf("Sumpower %d\n", sumpower);
            }
        }
        usleep(20000);
}

void Merc::get_data_merc()
{

    int init = mercInit();
    if (init != 0)
    {
        printf("failed Init Mercury \n");
    }
    else
    {
        while (senegj == 0)
        {
            senegj = getEnergy();
        }
        while (true) {

            getEnergy();
            //getWats();
            usleep(100000);
            getLivePower();
            power_quality_analyze();
            usleep(100000);
            //usleep(1000);
            //getVoltage(1);
            //usleep(1000);
            //getVoltage(2);
            //usleep(1000);
            //getVoltage(3);
            
            usleep(1000000); //sleep 1 sec

        }
    }
}

uint16_t Merc::CRC16_2(unsigned char* buf, int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++)
    {
        crc ^= (uint16_t)buf[pos];    // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {    // Loop over each bit
            if ((crc & 0x0001) != 0) {      // If the LSB is set
                crc >>= 1;                    // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                            // Else LSB is not set
                crc >>= 1;                    // Just shift right
        }
    }

    return crc;
}

double Merc::getFreq()
{
    uint16_t crc_tmp;
    int num_bytes = 0;
    double freq;
    uint8_t data_red[30] = { 0 };

    data_format[3] = 0x40;
    crc_tmp = CRC16_2(data_format, 4);
    data_format[4] = crc_tmp;
    data_format[5] = crc_tmp >> 8;

    //digitalWrite(4, HIGH);
    usleep(1000);
    for (int i = 0; i < 6; i++) {
        //appha.append((char*)&data_format[i]);
        //writeserial(appha.c_str(), 1);
        writeserial((char*)&data_format[i], 1);
        //write(serial_port, appha.c_str(), 1);
    }
    tcdrain(serial_port);
    //digitalWrite(4, LOW);
    usleep(400000);

    //num_bytes = readserial(*data_red, 12);
    //num_bytes = readserial(*read_buf, sizeof(read_buf));
    num_bytes = read(serial_port, &data_red, sizeof(data_red));

    if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
    }
    if (num_bytes == 0)
    {
        printf("err0\n");
    }
    if (num_bytes > 0)
    {
        uint32_t recks;
        if (CRC16_2(data_red, 19) == 0) {
            freq = ((data_red[1] & 0xf) << 16 | (data_red[2]) | (data_red[3] << 8));
            printf("freq %f \n", freq/100);
            return freq / 100;
        }
    }
    usleep(20000);
}

uint32_t Merc::getVoltage(uint8_t phase)
{
    uint16_t crc_tmp;
    int num_bytes = 0;
    uint32_t voltage;
    uint8_t data_red[30] = { 0 };

    data_format[3] = 0x10 + phase;
    crc_tmp = CRC16_2(data_format, 4);
    data_format[4] = crc_tmp;
    data_format[5] = crc_tmp >> 8;

    //digitalWrite(4, HIGH);
    usleep(1000);
    for (int i = 0; i < 6; i++) {
        //appha.append((char*)&data_format[i]);
        //writeserial(appha.c_str(), 1);
        writeserial((char*)&data_format[i], 1);
        //write(serial_port, appha.c_str(), 1);
    }
    //tcdrain(serial_port);
    usleep(8000);
    //digitalWrite(4, LOW);
    //usleep(400000);

    //num_bytes = readserial(*data_red, 12);
    //num_bytes = readserial(*read_buf, sizeof(read_buf));
    uint8_t ln = 10;
    while (ln--) {
        int bytes_available;
        num_bytes = ioctl(serial_port, FIONREAD, &bytes_available);
        if (num_bytes < 0) {
            perror("FIONREAD ioctl failed\n");
            break;
        }
        usleep(50 * 1000L);
        if (bytes_available >= 5)
            break;
    }
    num_bytes = read(serial_port, &data_red, sizeof(data_red));

    if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
    }
    if (num_bytes == 0)
    {
        printf("err0 phase %d\n",phase);
    }
    if (num_bytes > 0)
    {
        uint32_t recks;
        if (CRC16_2(data_red, 19) == 0) {
            voltage = ((data_red[1] & 0xf) << 16 | (data_red[2]) | (data_red[3] << 8));
            printf("voltage %d phase %d", voltage, phase);
            return voltage/100;
        }
    }
    usleep(20000);
}

void Merc::power_quality_analyze()
{
    uint16_t u_min = 500;
    uint16_t u_max = 0;
    double freq = getFreq();
    power_quality = 0;
    usleep(100000);
    uint32_t VRMSA = getVoltage(1);
    u_min = (u_min > VRMSA) ? VRMSA : u_min;
    u_max = (u_max < VRMSA) ? VRMSA : u_max;
    usleep(100000);
    uint32_t VRMSB = getVoltage(2);
    u_min = (u_min > VRMSB) ? VRMSB : u_min;
    u_max = (u_max < VRMSB) ? VRMSB : u_max;
    usleep(100000);
    uint32_t VRMSC = getVoltage(3);
    u_min = (u_min > VRMSC) ? VRMSC : u_min;
    u_max = (u_max < VRMSC) ? VRMSC : u_max;
    usleep(100000);
    printf("\n");

    if (u_max == 0)
        return;
    double asymmetry = 100 * (double)(1 - (double)(u_min / u_max));
    if (asymmetry > VOLTAGE_ASYMMETRY_MAX)
        power_quality |= 1;
    else
        power_quality |= 0;
    if (u_min > (NOMINAL_SUPPLY_VOLTAGE + VOLTAGE_DEVIATION_MAX))
        power_quality |= 1 << 1;
    else
        power_quality |= 0 << 1;
    if (u_max < (NOMINAL_SUPPLY_VOLTAGE - VOLTAGE_DEVIATION_MAX))
        power_quality |= 1 << 2;
    else
        power_quality |= 0 << 2;
    if ((freq > (NOMINAL_POWER_FREQUENCY + FREQUENCY_DEVIATION_MAX)) || (freq < (NOMINAL_POWER_FREQUENCY - FREQUENCY_DEVIATION_MAX)))
        power_quality |= 1 << 3;
    else
        power_quality |= 0 << 3;
}