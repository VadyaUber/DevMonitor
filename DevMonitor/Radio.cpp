#include "Radio.h"
#include <sys/time.h>
SerialPort* SR;

Radio::Radio()
{

}
#pragma push
#pragma O3
void Radio::init(string* MAC_address, string* Packet)
{
	MAC = MAC_address;
	send = Packet;
	SR = new SerialPort("/dev/ttyUSB0", 115200);
}


void Radio::WaitRecive()
{		
		char rxbuff[115] = { 0 };
		size_t received = 0;
		while (received < 36)
		{
			ssize_t r = read(SR->serial_port, rxbuff + received, 60 - received);
			if (r < 0)
			{
				perror("failed to read from port");
				//return -1;
			}
			if (r == 0)
			{
				// Timeout
				break;
			}
			received += r;
		}
		if (received)
		{
			/*long int now;
			struct  timeval  ts;
			gettimeofday(&ts, NULL);
			now = ts.tv_sec * 1000 + ts.tv_usec / 1000;;
			printf("now: %d\n", now);*/
			data = rxbuff;
			//string macrecv = data.substr(data.find(":") + 1, data.find(":")+10);

			string mc = *MAC;
			//string mc = "1281F162A6F5"; //debug
			if (data.find(mc) != string::npos)
			//{
			//	printf("mac if findedddcddfd");
			//}
			//printf("macrecv %s \n", macrecv.c_str());
			//if (macrecv == *MAC) //if mac address ok
			{
				printf("recv 12 ok: %s", rxbuff);
				datainput = true;

				/*string dst = data.substr(data.find(mc)-1, data.rfind(";") + 3);
				printf("\n\r");
				printf(" %s",dst);
				printf("\n\r");*/
			}
		}
		else
		{
			printf("recv bad 12: %s", rxbuff);
			//sleep(1);
		}
		printf("\n");
		//usleep(400000);
		//22
		//gettimeofday(&ts, NULL);
		//now = ts.tv_sec * 1000 + ts.tv_usec / 1000;;
		//printf("now: %d\n", now);
		//usleep(400000);
}

void Radio::SendPacket()
{
	//string writ2 = ":224206184EB6;;0400BF00FF;;;-68;;;02;20220428164406;92";
	int retrn;
	string packet = *send;
	printf("%s \n", packet.c_str());
	retrn = write(SR->serial_port, packet.c_str(), packet.length());
	tcflush(SR->serial_port, TCIOFLUSH);
	printf("retrn2 = %d\n", retrn);
}
#pragma pop
