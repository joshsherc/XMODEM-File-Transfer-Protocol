//============================================================================
//
//% Student Name 1: Layomi Dele-Dare
//% Student 1 #: 301136462
//% Student 1 userid (email): ldeledar (ldeledar@sfu.ca)
//
//% Student Name 2: Josh Shercliffe
//% Student 2 #: 301294276
//% Student 2 userid (email): jshercli (jshercli@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__ TA
//
// Also, list any resources beyond the course textbooks and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P2_<userid1>_<userid2>" (eg. P1_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : ReceiverX.cpp
// Version     : September 3rd, 2017
// Description : Starting point for ENSC 351 Project Part 2
// Original portions Copyright (c) 2017 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <string.h> // for memset()
#include <fcntl.h>
#include <stdint.h>
#include <iostream>
#include "myIO.h"
#include "ReceiverX.h"
#include "VNPE.h"
#include <iostream>

enum START{
	START_STATE,
	GET_STATE,
	DO_STATE,
	CAN_STATE,
	EOT_STATE,
};

using namespace std;

ReceiverX::
ReceiverX(int d, const char *fname, bool useCrc)
:PeerX(d, fname, useCrc), goodBlk(false), goodBlk1st(false), numLastGoodBlk(0)
{
	NCGbyte = useCrc ? 'C' : NAK;
}

void ReceiverX::receiveFile()
{
	int testNum = 0;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	// should we test for error and set result to "OpenError" if necessary?
	transferringFileD = PE2(creat(fileName, mode), fileName);

	// ***** improve this member function *****

	// below is just an example template.  You can follow a
	// 	different structure if you want.

	// inform sender that the receiver is ready and that the
	//		sender can send the first block

//	sendByte(NCGbyte);

	START states = START_STATE;
	bool flag=true;
	while(flag)
	{
		switch(states)
		{
			case START_STATE :
//				sendByte(NAK);
				sendByte(NCGbyte);
				errCnt = 0;
				states = GET_STATE;
				break;

			case GET_STATE :
				PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
					if(rcvBlk[0] == SOH)
					{
						getRestBlk();
						if(!goodBlk1st)
						{
							errCnt++;
//							sendByte(NAK);
						}
						else
						{
							 errCnt = 0;
						}
						states = DO_STATE;
					}
					else if(rcvBlk[0] == CAN)
					{
						states = CAN_STATE;
					}
					else if(rcvBlk[0] == EOT)
					{
						sendByte(NAK);
						states = EOT_STATE;
					}
					break;
		       case DO_STATE:
		           if(errCnt < errB)
		           {
		        	   	   //testNum = (int)rcvBlk[1];

		        	   	if(goodBlk)
		                {
//		        	   		if (testNum%11 == 0)
//		        	   		{
//		        	   			sendByte(NAK);
//		        	   		}
//		        	   		else
		        		   	sendByte(ACK);
		                }
		                else
		                {
//		                	if(testNum%11 == 0)
//		                	{
//		                		sendByte(ACK);
//		                	}
//		                	else
		                		sendByte(NAK);
		                }
		                if(goodBlk1st)
		                {
		                	writeChunk();
		                }
		           }
		           else
		           {
		        	   result = "too many NAKS";
		           }
		           states = GET_STATE;
		           break;
		       case EOT_STATE:
		    	   PE_NOT(myRead(mediumD, rcvBlk, 1), 1);
		    	   if(rcvBlk[0] == EOT)
		    	   {
		    		   sendByte(ACK);
		    		   result = "Done";
		    	   }
		    	   return;
		    	   break;
		       case CAN_STATE:
		           result = "Cancelled";
		    	   break;
		       }
			}
			PE(close(transferringFileD));


//	while(PE_NOT(myRead(mediumD, rcvBlk, 1), 1), (rcvBlk[0] == SOH))
//	{
//		getRestBlk();
//		sendByte(ACK);
//		writeChunk();
//	};
////	 EOT was presumably just read in the condition for the while loop
//	sendByte(NAK); // NAK the first EOT
//	PE_NOT(myRead(mediumD, rcvBlk, 1), 1); // presumably read in another EOT
//	sendByte(ACK); // ACK the second EOT
//	PE(close(transferringFileD));
//	result = "Done"; // move this line above somewhere?
}

/* Only called after an SOH character has been received.
The function tries
to receive the remaining characters to form a complete
block.  The member variable goodBlk will be made false if
the received block formed has something
wrong with it, like the checksum being incorrect.  The member
variable goodBlk1st will be made true if this is the first
time that the block was received in "good" condition.
*/
void ReceiverX::getRestBlk()
{


	// ********* this function must be improved ***********
	//PE_NOT(myReadcond(mediumD, &rcvBlk[1], REST_BLK_SZ_CRC, REST_BLK_SZ_CRC, 0, 0), REST_BLK_SZ_CRC);
	if(Crcflg == false)
	{
		PE_NOT(myReadcond(mediumD, &rcvBlk[1], REST_BLK_SZ_CS, REST_BLK_SZ_CS, 0, 0), REST_BLK_SZ_CS);
		uint8_t checkSumRecieved = 0;;
		uint8_t checkSumTest = 0;
		checkSumRecieved = rcvBlk[131];
			for( int i=3; i<131; i++ )
				checkSumTest += rcvBlk[i];
			//Validating the checkSum and block num
			goodBlk = (checkSumTest == checkSumRecieved) && (rcvBlk[1] == 255 - rcvBlk[2]);
			if (goodBlk)
			{
				goodBlk1st = (rcvBlk[1] == (uint8_t) (numLastGoodBlk + 1));
				if (goodBlk1st)
				{
//					if(rcvBlk[1]%10 == 0)
//					{
//						sendByte(NAK);
//					}
					numLastGoodBlk = rcvBlk[1];
				}

			}
			else
			{
				goodBlk1st = false;
				//sendByte(NAK);
			}
//		goodBlk1st = goodBlk = true;
		}
	else
	{
		PE_NOT(myReadcond(mediumD, &rcvBlk[1], REST_BLK_SZ_CRC, REST_BLK_SZ_CRC, 0, 0), REST_BLK_SZ_CRC);
		//int testNum = (int)rcvBlk[1];
		//cout << " Blk Number is " << testNum << endl;

		uint16_t crcValue = 0;
        crc16ns(&crcValue, &rcvBlk[3]);
        uint8_t HighNum, LowNum, testHighNum, testLowNum = 0;
        testHighNum = rcvBlk[132];
        testLowNum = rcvBlk[131];
        //take the first 8 bits and cast it and assign it to LowNum
        LowNum = (uint8_t)crcValue;


        //Shift it 8 bits and then cast it and assign it to the HighNum
        HighNum = (uint8_t)(crcValue>>8);
        goodBlk = ((HighNum == testHighNum && LowNum == testLowNum)&& (rcvBlk[1] == 255 - rcvBlk[2]));
			if (goodBlk)
			{
				goodBlk1st = (rcvBlk[1] == (uint8_t) (numLastGoodBlk + 1));
				if (goodBlk1st)
				{
					numLastGoodBlk = rcvBlk[1];
				}

			}
			else
			{
				 goodBlk1st = false;
				//sendByte(NAK);
			}
        //goodBlk1st = goodBlk = true;
	}

}

//Write chunk (data) in a received block to disk
void ReceiverX::writeChunk()
{
	PE_NOT(write(transferringFileD, &rcvBlk[DATA_POS], CHUNK_SZ), CHUNK_SZ);
}

//Send 8 CAN characters in a row to the XMODEM sender, to inform it of
//	the cancelling of a file transfer
void ReceiverX::can8()
{
	// no need to space CAN chars coming from receiver
	const int canLen=8; // move to defines.h
    char buffer[canLen];
    memset( buffer, CAN, canLen);
    PE_NOT(myWrite(mediumD, buffer, canLen), canLen);
}
