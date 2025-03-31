# 4 Trains Application
group:
 - André Silva Telles
 - Felipe Lorenzzon
 
# Description 
An application that allows for the management of 4 trains in the model
railway at Centrale Lille considering that the application managing each of the
trains could be in separate machines, all of which are to communicate with the
plc.

The project's "problem statement" is defined in the file
"problem_definition.pdf" in the docs folder. Additionally, files with a diagram
of the railway model are provided in the same folder.

An analysis of how the resources were divided can be found in the `docs` folder as
well as a petri network explaining the process requesting resources.

# Modules
As described in the sysML diagram in the `docs` folder, the solution is divided
amongst two main programs the Train Manager, the Resource Manager and the code
for the PLC (which has been provided).

These are designed to run on different machines connected via tcp connections in
a network.

## Train Manager
Commands 2 trains to follow two routes through a physical model. The routes can
be given through a path file provided in the folder `paths`, whose syntax is
described later on in this file.

### PLC Proxy
Encapsulates communication from the Train Manager with the PLC. The process for
sending messages is simply to lock a mutex, send a message over the file
descriptor, and unlocking the mutex. Reading, however, is more complicated.

To read a message a process attempts to read from a file descriptor assigned to
it and stays blocked on it until something arrives. In parrallel, the proxy has
it's own reader thread that actually reads messages from the socket connected to
the PLC and re-routes them to the appropriate filedescriptor based on its
contents.

An exception to this rule are Acknoledge messages. The PLC sends an acknowledge
for every message it receives. As these require no special action and, when they
are unsuccessfull, there is very little the code can do, this application simply
ignores them.

### Resource Manager Proxy 
Encapsulates communication from the Train Manager with the Resource Manager.

Currently it can support the requests for one or more resources at the same
time. The responses arive in a struct indicating whether the request has been
granted or not.

### Interpreter

Protocol to interpret lines of the path file and execute the route described in
them. Is designed to be called at the behest of a train.

The interpreter allows for the following three commands:
* trainId X: sets the Id of the train that made the call to interpret the command 
* plc: commands that address the plc
  * rail X: turns the specified rail on 
  * switch X: turns the specified switch to the configuration of the requesting
  train  
  * invert X: inverts the direction of the train on top of the specified
    rail (given it's a possition that allows for it)
* resource: commands that address the resource manager
  * lock X,Y,Z,...: stays on this instruction until the resource manager grants
  it access to all the resources listed 
  * release X,Y,Z,...: tells the resource manager to release all the resources
    listed 
    
The route files are available in the folder `routes`, one is provided for each
of the requested train routes.

## Resource Manager
A multithread solution that allows resources to be locked and released in parallel.

The requests are made over a tcp connection. These requests can attempt to lock
or release one or more resource at a time. Short of a system error or another id
attempting to unlock a resource it does not own, the realease requests are
allways grant.
For the lock requests, the resource manager attempts to lock all the requested
resources one by one and if at any point it's unable to lock a resource, it
unlocks those resources it did lock and tells the caller the request was refused.

### Resource Database
A simple module tracking which resources have been locked and unlocked.
Allowing, at the moment, at most 1 resource to be registered at a time.

### Resource Database Proxy
A wrapper around the Resource Database to ensure at most one thread modifies it
at a time. 


# Running the solution

To run the solution the computer must be in the same network as the plc. Given
that's the case, the first module to be run must be the resource manager as the
other two must connect to it. To do this, use the command `make
run/resource_manager`. Afterwards, to run the two train managers simply run
`make run/train_manager1` and `make run/train_manager2`.

If so desired, all these commands can be run with a verbose tag `-v` to print
out what each of the modules is doing more precisely. There's, however, no make
command to this. 

The make command also maps the following xway stations to the given trains:

* train 1: 40
* train 2: 41
* train 3: 42
* train 4: 43

Thus the routing table of the plc's code must be adjusted accordingly.
