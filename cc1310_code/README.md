# Swarm Robotic Platform
### Josh Cohen

## Intro
With the goal of making decentralized, equitable, and sustainable systems, swarm robotic systems seem like a promising framework. Inspired by the success of the open source hardware community over the past 10 years and works like the [swarmbot: Jasmine](http://www.swarmrobot.org/), I hope to attempt to design my own low cost platform that can help propel swarm robotics into the real world. 

## TI CC1310
This is the [micro-conrtoller](http://www.ti.com/product/CC1310) we are using due to its small profile, low power consumption, and integrated RF capabilities. This board is currently interfacing with a [Pololu Zumo](https://www.pololu.com/product/2508) robot through a custom connector board.    

## Current Work
* Integrate biosynchronicity algorithm with Zumo movement to get them to "dance"
* Iterate on board
	* custom PCB for CC1310
	* LED array for representing bitwise memory
	* power at 3.3V  
* reorganize this github

## Accomplished Work
* Implemented driver library level functionality of control of the Zumo robot
* Designed v1.0 of interface PCB
* Implemented prototype of a custom biosynchronicity algorithm using the CC1310's RF Driver