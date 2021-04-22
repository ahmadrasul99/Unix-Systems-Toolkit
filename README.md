Ahmad Rasul
Project 1
02/26/2021

All the commands implemented in my toolkit are implemented as internal commands. I implemented the
commands as internal due to the fact that their execution would be fast in the sense that the shell
doesn’t have to search the given path for them. Also, no process needs to be spawned for its execution.

Custom commands:
  myexit      implemented successfully
  mycd        implemented successfully
  mypwd       implemented successfully
  mytree      implemented successfully
  mytime      partially implemented; displays system CPU time and elapsed time but unable to show user CPU time
  mymtimes    not implemented
  mytimeout   not implemented

Termination upon receiving EOF (CTRL+D) not implemented.

All existing external UNIX commands are implemented successfully.

Able to support multiple pipes (up to 10 in a command line). However, custom commands cannot be used with pipes.
  successful usage      ls -al | sort | wc
  unsuccessful usage    mypwd | wc

I/O redirection is not implemented.
Ahmads-MacBook-Pro:Project 1 ahmadrasul$ 
