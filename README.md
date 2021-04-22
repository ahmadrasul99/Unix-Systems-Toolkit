Ahmad Rasul
Project 1
02/26/2021

All the commands implemented in my toolkit are implemented as internal commands. I implemented the
commands as internal due to the fact that their execution would be fast in the sense that the shell
doesn’t have to search the given path for them. Also, no process needs to be spawned for its execution.

Custom commands implemented:
  myexit,
  mycd,
  mypwd,
  mytree,
  mytime.
  
All existing external UNIX commands are implemented successfully.

Able to support multiple pipes (up to 10 in a command line). However, custom commands cannot be used with pipes.

Termination upon receiving EOF (CTRL+D) not implemented.

I/O redirection is not implemented.
