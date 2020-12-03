# `SimpleChatProgramm`

## Used Library:
 ### This programm was built using [CPPSOCK](https://github.com/PrugClem/cppsock).

***A simple chat programm written by Clemens Pruggmayer, Mottl Mario for our programming class.
The code might not be up to programming standards.***

## Basic usage:
```
make server
make client

./server
./chatwithme <-k key> <-m my_msg_type> <-r rcvr_msg_type> <-u user_name>

```
`key`, `my_msg_type`, `rcvr_msg_type` must be the same for both users to be able to chat with eachother.

## Example
```
chatwithme -k 9875 -m 54  -r 68 -u KieselsteinKurt
chatwithme -k 9875 -m 68  -r 54 -u BergspitzenFranz

Kieselsteinkurt:

Other >> Hello there!
KieselsteinKurt >> General Kenobi.
KieselsteinKurt >> bye

BergspitzenFranz:

BergspitzenFranz >> Hello there!
Other >> Gerneral Kenobi.
BergspitzenFranz >> bye
```

- [x] Basic functionality
- [ ] Multiclient handling
- [ ] Proper documentation
