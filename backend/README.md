# oopetris backend 

OOPetris backend


ATM ONLY INTENDED TO RUN ON LINUX! (even if there's nothing that only would run on linux, but it's designed to run on a server, so linux is the best and only choice)

backend using [oatpp](https://github.com/oatpp/oatpp)

frontend `WIP` (which one do we choose?)


## Build and Run

### Using Meson

```bash
meson setup build
meson compile -C build
./build/oopetris-backend
```


Note: this only listens to https and wss (secure websocket) so a proxy like e.g. nginx is required, to direct http and ws traffic to the secure sites!

### In Docker

`WIP`
