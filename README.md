# CS5600-Practicum-II


# How to Run File Server

 - Go to server-code project if you want to run the file-server
 - Do ```make clean``` , then ```make server```. You should see a server binary created.
 - Do ```./server``` to run the server , which starts accepting socket connections on port 8080.
 - You need ```server-file-system`` directory in server-code (server file system root)


# How to Run File Client

 - Go to client-code project if you want to run the file-server-client
 - Do ```make clean``` , then ```make rfs```. You should see a rfs binary created.
 - You need ```client-file-system`` directory in client-code (client file system root)
 - Do ```./rfs CMD <filepath> ...``` to run the client command , which starts sending requst over socket connection on port 8080.

  ## Different commands supported on server
  
  - ```./rfs WRITE <local-file-path> <remote-file-path> ``` , writes the content of local file to remote server at the given path.
  -  ```./rfs WRITE <local-file-path> ``` , writes the content of local file to remote server at the same local path.
  - ```./rfs GET <remote-file-path> <local-file-path> ``` , writes the content of remote file to local file system at the given path.
  -  ```./rfs GET <local-file-path> ``` , writes the content of remote file to local file system at the same local path.
  -  ```./rfs GET <local-file-path> <version_number>``` , writes the content of remote file to local file system at the same local path using the integer version number.
  - ```./rfs RM <remote-file-path> ``` , deletes the content of remote file and the file at the given path on server.
  -  ```./rfs LS <remote-file-path> ``` , fetches the versioned information of remote file on the server.


# Design on server
  
  - ```server.c``` : Comprises of the server code to handle concurrent clients using multithreading, and processes each request to a specific handlers.
  - ```handlers``` : There are multiple handlers for each command , Like get , ls , rm and write.
  - ```utilities``` : Some utility functions are also present to do simple tasks like creating directories and files if not present to avoid code redundancy.


# Design on client

   - ```client.c``` : Entry point to the client , takes cmd line args as specifies above and processes them using library functions from client-lib.
   - ```client-lib.c``` : Consists of command parsers for each type of commands and talks to the server and executes the specific operation.

# Handling Concurrency in multithreading 
   - Shared read locks applied on read operations on files.
   - Exclusive write locks applied on write operations on files.








