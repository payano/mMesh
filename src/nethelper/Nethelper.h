/*
MIT License

Copyright (c) 2020 Johan Svensson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once


class NetHelper{
public:
       static void printf_address(const struct net_address *address) {
               printf("broadcast: %d, "
                               "master: %d, "
                               "generated: %d, "
                               "net: 0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x "
                               "host addr: 0x%02x\n",
                               address->broadcast,
                               address->master,
                               address->gen_addr,
                               address->nbs[0].net,
                               address->nbs[1].net,
                               address->nbs[2].net,
                               address->nbs[3].net,
                               address->nbs[4].net,
                               address->nbs[5].net,
                               address->nbs[6].net,
                               address->nbs[7].net,
                               address->nbs[8].net,
                               address->host_addr);
       }

       static const char *getMsgno(MSGNO msgno){
               switch(msgno){
               case MSGNO::INVALID: return "INVALID";
               case MSGNO::BROADCAST_ASSOCIATE_REQ: return "BROADCAST_ASSOCIATE_REQ";
               case MSGNO::BROADCAST_ASSOCIATE_RSP: return "BROADCAST_ASSOCIATE_RSP";
               case MSGNO::BROADCAST_NEIGHBOUR_REQ: return "BROADCAST_NEIGHBOUR_REQ";
               case MSGNO::BROADCAST_NEIGHBOUR_RSP: return "BROADCAST_NEIGHBOUR_RSP";
               case MSGNO::NETWORK_ASSIGNMENT_REQ: return "NETWORK_ASSIGNMENT_REQ";
               case MSGNO::NETWORK_ASSIGNMENT_RSP: return "NETWORK_ASSIGNMENT_RSP";
               case MSGNO::REGISTER_TO_MASTER_REQ: return "REGISTER_TO_MASTER_REQ";
               case MSGNO::REGISTER_TO_MASTER_RSP: return "REGISTER_TO_MASTER_RSP";
               case MSGNO::PING_PARENT_REQ: return "PING_PARENT_REQ";
               case MSGNO::PING_PARENT_RSP: return "PING_PARENT_RSP";
               case MSGNO::DISCONNECT_CHILD_REQ: return "DISCONNECT_CHILD_REQ";
               case MSGNO::DISCONNECT_CHILD_RSP: return "DISCONNECT_CHILD_RSP";
               case MSGNO::MESSAGE_REQ: return "MESSAGE_REQ";
               case MSGNO::MESSAGE_RSP: return "MESSAGE_RSP";
               case PING_NEIGHBOUR_REQ: return "PING_NEIGHBOUR_REQ";
               case PING_NEIGHBOUR_RSP: return "PING_NEIGHBOUR_RSP";
               }
               return "MSGNO DOESN'T EXIST";
       }

       NetHelper(){};

};

