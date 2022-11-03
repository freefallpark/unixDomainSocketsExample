The Goal of this Project is to demonstrate that I can pass information from one process (executable/program) to another.
I need to pass 24 bytes of data from one program (or process) to another.

Previously I created a simple shared memory project that demonstrates exchanging data (as described above) using mmaps()
or Shared Memory Mapping. This works and seems like it will be very fast; however, in conversation with my friend Peter
he recommended that I look into Unix Domain Sockets (UDS) instead of using Shared Memory Mapping. His reasoning was:

        1.  Sharing Memory between processes is generally 'dangerous' and software developers typically try and avoid
            doing this when possible. From my understanding it is considered 'dangerous' because you have to handle
            memory synchronization and its actually not to hard to find yourself in a situation where you can
            accidentally cause undefined behavior (if you go over the amount of data in your page, etc)

        2.  UDS is more similar to inter-computer communication and it will be easier to switch to that in the future

I was surprised how few examples in this use case I could find. I had to do a lot of digging. I ended up finding the man
pages the most useful. Here's the one that got me there: https://man7.org/linux/man-pages/man7/unix.7.html

I did my best to add in comments so I could come back to this at a later date and still understand whats going on...

The general process happening is:
Server creates a file descriptor (FD) and an sockaddr(I'm equating this to an address is some remote virtual fairly land)
we then bind the socket to the sockaddr. Once bound, we can start listening for/accepting connections with clients.
On the client side. We create an FD and bind it to the same sockaddr Path we set up the server with. This creates the link.
The client can then request a connection from the server. When the server accepts the connection it sends the file descriptor
location to the output of the connect() function the client called. This is the FD the server and client will pass data on.
The first FD was just so the server can listen for clients.



Here are some Key words That I used to get to this program running:

Inter Process Communication
Unix Domain Sockets (UDS)
Remote Process Communication
poll()
read()
recv()
write()
send()
sendto()

