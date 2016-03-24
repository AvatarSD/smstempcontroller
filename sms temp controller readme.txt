Commands:
search
status
setnode
shownode
deletenode
disablenode
enablenode
help


Will be:
enablenode
setnodealarm
setnodequite
setapn
getapn
setbattalarm
showbattalarm
setalarmperiod
getalarmperiod
setunit


Commands
Parameter
What it will do
Possibility
Example of repete
search
 
Default search with 10 attempts of search. Return list of founded sensors.
search


<num_of_attempts>
Search with settings of search attempts(if not all sensors found)
search  5
search 30

status

Read all temp at sensors and return list of sensors with temperature
status

setnode
<num>,<min>,<max>




<num>,<min>,<max>,<phone>



shownode
<num>
Show all nodes with sensor, witch saved at <num> position at sensor list(witch returnet by “search” or ”status” command)



<phone>




all




my



deletenode




disablenode




enablenode




help








































