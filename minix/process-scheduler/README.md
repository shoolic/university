Implement a scheduling algorithm with the following assumptions:
- three categories of user-level processes in the system: minPRI, midPRI and highPRI
- set_PRI system call for setting the process category
- processes implemented in a static time regime (having allocated time quanta)
- highPRI processes able to perform for 3 consecutive time quanta
- midPRI processes able to perform for 2 consecutive time quanta
- minPRI processes after the elapse of the quantum immediately moved to the end of the process queue