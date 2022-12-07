# ESS_MAC

This project simulates ESS-MAC with embedded learning components. The simulation kernel performs event scheduling in terms of packet generation, transmissions and receptions. To implement the proposed ESS-MAC protocol, the Reinforcement Learning and Multi-Armed Bandits update equations are embedded on top of the MAC layer functions.  

There are two main scripts: ESS_MAC_Slot_Allocator.c and ESS_MAC_Scheduler.py

ESS_MAC_Slot_Allocator.c is for MAB-based slot allocation

To run a simulation proceed using the following steps:

1. Input Number of Network nodes in No_of_Nodes
2. Define the network topology using the matrix topology[][], where topology[i][j]=1, if nodes i and j are connected. Else, topology[i][j]=0. To note, that the node index starts from 0.
3. Input the frame size in number of slots using the variable minislots. Note that actions and minislots variables should have the same value
4. Input the learning hyperparameters: Learning rates (&alpha; and $\beta$), Epsilon greedy policy parameters ($\eps_max$)
5. Input the number of learning epochs in variable epochs
6. Observe the outputs in terms of allocated slot for each node


ESS_MAC_Scheduler.py is for RL/MAB-based sleep scheduler

To run a simulation proceed using the following steps:

1. Input Number of Network nodes in Num_nodes and frame size in frame_size
2. Define the source nodes in variable 'sources', intermediate nodes in variable 'int_nodes' and destination nodes in variable 'destinations'.
3. Input the allocated slot for each node in sequence of node number. This will come from ESS_MAC_Slot_Allocator.c
4. Define all the flows in the network in the list 'flow'
5. Define the data rate of each flow in the list 'rate'
6. Input the learning hyperparameters: Learning rate (&alpha;), Epsilon greedy policy parameters (&$\epsilon_{max}, \epsilon_{decay}$;), discount factor (&gamma;), learning rate decay
7. Input the number of learning epochs in variable epochs
8. Observe the outputs in terms of packet missed rate, efficiency, end-to-end delay
