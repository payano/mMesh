![C/C++ CI](https://github.com/payano/mMesh/workflows/C/C++%20CI/badge.svg)

More information regarding the mesh network: https://www.exclude.se/2020/03/02/mmesh-mesh-network/

Help is needed, if you are interested come and join me for this interesting journey. The mesh network is the foundation for Internet Of Things modules and will enable products using this network. It's meant to be hardware agnostic, Interfaces is used so every new hardware can do their own implementation. Today STM32f103 and NRF24l01 is the hardware I'm using. 

When mesh network platform is up and running, then the next step is to create a server that communicates with the nodes over the network. The nodes will most often be sensors that report to the master. The server can be home automation, alarm, surveillance, medical. The role of the server is collect data and store in some kind of a database.
