# PCP-FreeRTOS
Modification of the FreeRTOS operating system to support the PCP synchronization protocol for shared resources. 
It supports working with resources through lock and unlock operations, which are defined
independent of the standard FreeRTOS mutex mechanism, and which work according to the PCP algorithm.

Mutexes can be declared programmatically. There is a fixed upper
limit for the maximum number of mutexes. When starting the system the user via the console or
serial port specifies for each task which resources it will use.

Tests have been formed to show that the PCP implementation is valid. These
tests cover typical problem situations of the PCP algorithm.

## Priority Ceiling Protocol

In real-time computing, the priority ceiling protocol is a synchronization protocol for shared resources to avoid unbounded priority inversion and mutual deadlock due to wrong nesting of critical sections. In this protocol each resource is assigned a priority ceiling, which is a priority equal to the highest priority of any task which may lock the resource. The protocol works by temporarily raising the priorities of tasks in certain situations, thus it requires a scheduler that supports dynamic priority scheduling.


The basic idea of this method is to extend the *Priority Inheritance Protocol - PIP* by using a rule that allows locking on a free semaphore.

To avoid multiple blocking, this rule does not allow a task to enter a critical section if there are locked semaphores that could block it.  
This means that once a task enters its first critical section, it can never be blocked by lower priority tasks until it completes. 

In order to accomplish this, each semaphore is assigned an upper priority limit equal to the highest priority of the task that can lock it. 

Task τi can enter the critical section only if its priority is higher than all semaphore priority upper bounds locked by tasks other than τi.
