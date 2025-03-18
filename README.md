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

## Train Manager

### PLC Proxy

### Resource Manager Proxy 

### Interpreter

To ensure no route deadlocks another, the interpreter always attempts to take 
resources from the resource manager in the same order.

## Resource Manager
### Resource Database

### Resource Database Proxy
