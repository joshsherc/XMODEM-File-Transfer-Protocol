# XMODEM-File-Transfer-Protocol
A simulation of an XMODEM File transfer protocol. Utilizes multi-threaded programming to run multiple sender and receiver programs, being able to send more chunks of data at a time., 

## Basic Information

This program utilizes functions provided by William Scratchley, our SFU professor, as well as code provided by ourselves. The program utilizs multiple threads to send and recieve 128 byte chunks via senders and recievers.

The medium can be defined as a simulation of a wire. The medium will induce random errors, such as corrupting a chunk. Via the checksum algorithm, the sender and reciever will communicate with each other, ensuring that the sent data matches the recieved data.


If too much corrupted data is sent (10 corrupt chunks in a row), the file transfer will terminate. If within that time, a chunk transfer is correct, the sender will continue transferring the file.

## Code Credentials

The program was developed incrementally in parts throughout the course of the semester. In the current version posted to github, the myIO files in the Ensc351x-myio were developed solely by ourselves.

In previous parts of the project, the sender and reciever code were developed by ourselves. However, in order to ensure stability across all students' projects, a template was given after those parts were submitted. This ensured every student was using the same code, and would not arise in cascading errors throughout the semester.

In the "Previous Files" section, our personally developed variations of the Sender and Reciever files are submitted, in case anybody wishes to view our iteration of those sender and reciever files.


