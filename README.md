# 4 Trains Application
group:
 - André Silva Telles
 - Felipe Lorenzzon
 - 
 
# Description 
Build an application that allows for the management of 4 trains in the model
railway at Centrale Lille considering that the application managing each of the
trains could be in separate machines, all of which are to communicate with the
plc.

The project's "problem statement" is defined in the file
"problem_definition.pdf" in the docs folder

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
Encapsulates communication from the Train Manager with the PLC code

### Resource Manager Proxy 
Encapsulates communication from the Train Manager with the Resource Manager

### Interpreter

Protocol to interpret the contents of the path file and execute the route
described in them.

To ensure no route deadlocks another, the interpreter always attempts to take 
resources from the resource manager in the same order.

## Resource Manager
A multithread solution that allows resources to be locked and released in parallel.

### Resource Database
A simple module tracking which resources have been locked and unlocked.
Allowing, at the moment, at most 1 resource to be registered at a time.

## Resource Database Proxy
A wrapper around the Resource Database to ensure at most one thread modifies it
at a time. 
