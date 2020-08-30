# Swarm Robotic Platform
### Josh Cohen

## Intro
With the goal of making decentralized, equitable, and sustainable systems, swarm robotic systems seem like a promising framework. Inspired by the success of the open source hardware community over the past 10 years and works like the [swarmbot: Jasmine](http://www.swarmrobot.org/), I hope to attempt to design my own low cost platform that can help propel swarm robotics into the real world. 


![](gifs/circ.gif)
![](gifs/5_node.gif)


## TI CC1310
This is the [micro-conrtoller](http://www.ti.com/product/CC1310) we are using due to its small profile, low power consumption, and integrated RF capabilities. This board is currently interfacing with a [Pololu Zumo](https://www.pololu.com/product/2508) robot through a custom connector board.    

## Current Work
* Working "Firefly Synchronicity" inspired biosynchronicity algorithm that multiplexes time domain such that all robots can communicate within a given time period (\~1 second)
* 2 Working PCB Implementations:
	* Interface to Zumo Robot and CC13310 Launchpad XL
	* From scrath PCB for the CC1310 with working integrated PCB RF Antena that also serves as chassis for a diff drive robot
* Working line following robot that communcites based on the "Bayesian Particle" control laws researched by Ana Pervan and the Murphy lab
	* bit wise state machine to handle state transitions between every encountered intersection


## Directory Structure
* cc1310_code
	code running on the CC1310, controls zumo, implements RF synchronicity algorithm. See directory for more information
* listener
	* code that runs on "Hub" zumo, intercepts all TX'd messages from the zumo cc1310s and interperts the packets to be transmitted on via serial to the host computer, must also be used to communicate to zumos that start|stop them running or change their policies.  
* doc
	documentation relating to the Zumo, CC1310, and other components in this design
* eagle_files
	all PCB related documents
* scripts
	* a script to interpret and graph outputs of the ADC reads from the IR color sensors 
	* small c files in which we can test the bitwise state machine
* interface
	* hub.py is run to open the gui that lets you control the robots and collect data
* stls 
	* holds stl files for which the CAD can be found at [here](https://cad.onshape.com/documents?column=modifiedAt&nodeId=bd4b57b4a59c5991dd655b93&resourceType=folder&sortOrder=desc)