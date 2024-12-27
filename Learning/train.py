# importing libraries

import numpy as np
from environment import Environment

#define the parameters 
gamma = 0.9 #discount factor and determines how much future reward is considered
alpha = 0.75 #learning rate determines how much new info overrides old info
nEpochs = 1500 #number of iterations training process will execute

#Environment and Q-Table initialization

env = Environment()
rewards = env.rewardBoard
QTable = rewards.copy()

#Preparing the Q-learning process
#What are all the valid states from the current states
possibleStates = list() #instantiate all the possible states player can move from
for i in range(rewards.shape[0]):
        if sum(abs(rewards[i])) != 0:
            possibleStates.append(i)

#preparing Q-learning process that maximizes Q values

def maximum(qvalues):
    inx = 0
    maxQValue = -np.inf #initialization process
    for i in range(len(qvalues)):
        if qvalues[i] > maxQValue and qvalues[i] !=0:
            maxQValue = qvalues[i]
            """We got new qvalue and that qvalue is with valid action"""
            inx = i
    return inx, maxQValue


#start Q-Learning process (training process)

for epoch in range(nEpochs):
    startingPos = np.random.choice(possibleStates)
    
    #get all the playable actions
    possibleActions = list() #next state 
    for i in range(rewards.shape[1]):
        if rewards[startingPos][i] !=0:
            possibleActions.append(i)
            
    #play one action randomly: action: nextPos
    action = np.random.choice(possibleActions)
    
    reward = rewards[startingPos][action]
    
    #maximum Q-Value
    _, maxQValue = maximum(QTable[action])
    
    #model using temporal difference learning: update QTable
    
    TD = reward + gamma * maxQValue - QTable[startingPos][action]
    #Bellmans equation
    QTable[startingPos][action] = QTable[startingPos][action] + alpha*TD
    
#Display the result
currentPos = env.startingPos
while True:
    action, _ = maximum(QTable[currentPos])
    env.movePlayer(action)
    currentPos = action #action is state
    
    
    
    
    
    
    
    
    
    
    
