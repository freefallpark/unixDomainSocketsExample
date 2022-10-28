The Goal of this Project is to demonstrate that I can pass information from one process (executable/program) to another.
Our team is trying to pass pose data from one program which generates it, to another that uses it. we need to pass
six, 3Byte signals across in addition to one byte for error handling.

Previously I created a simple shared memory project that demonstrates exchanging data (as described above) using mmaps()
or Shared Memory Mapping. This works and seems like it will be very fast; however, in conversation with my friend Peter
he recommended that I look into Unix Domain Sockets (UDS) instead of using Shared Memory Mapping. His reasoning was:


        1.  Sharing Memory between processes is generally 'dangerous' and software developers typically try and avoid
            doing this when possible. From my understanding it is considered 'dangerous' because you have to handle
            memory synchronization and its actually not to hard to find yourself in a situation where you can
            accidentally cause undefined behavior (if you go over the amount of data in your page, etc)

        2.  UDS is more similar to inter-computer communication and it will be easier to switch to that in the future
            (It is likely that our process that generates this pose would run on a separate machine in the future.)git



I found this website which walks through creating a UDS between to processes I will try to follow it in this project:
            https://medium.com/swlh/getting-started-with-unix-domain-sockets-4472c0db4eb1

        NOTE: I ended up following IBM's page instead. they don't use the funcitonality from man7...
            https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=considerations-unix-domain-sockets

Once I get it going in a simple manner I will try to send data from the 'client' (pose generator ) TO the 'server'
(pose interpreter) at a rate of 1ms.

While following what Matt Lim did, I'm noticing he has some libraries/headers from
                                    https://man7.org/tlpi/code/index.html
For now, I'm not going to download this. It seems like this will only affect my ability to follow some of his error
handling logic.
