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