CC := gcc
CFLAGS := -g -Wall
TARGET := pipe fifo socketpair uds udsd tcp udp shm udpcs

# Background color
GREEN  				:= $(shell tput -Txterm setaf 2)
YELLOW 				:= $(shell tput -Txterm setaf 3)
BLUE 				:= $(shell tput -Txterm setaf 4)
MAGENTA             := $(shell tput -Txterm setaf 5)
WHITE  				:= $(shell tput -Txterm setaf 7)
RESET  				:= $(shell tput -Txterm sgr0)
TARGET_MAX_CHAR_NUM := 20

## Show help
help:
	@echo ''
	@echo 'Usage:'
	@echo '  ${YELLOW}make${RESET} ${GREEN}<target>${RESET} ${MAGENTA}[variable=value]${RESET}'
	@echo ''
	@echo 'Targets:'
	@awk '/^[a-zA-Z\-\_0-9]+:/ { \
		helpMessage = match(lastLine, /^## (.*)/); \
		if (helpMessage) { \
			helpCommand = substr($$1, 0, index($$1, ":")-1); \
			helpMessage = substr(lastLine, RSTART + 3, RLENGTH); \
			printf "  ${YELLOW}%-$(TARGET_MAX_CHAR_NUM)s${RESET} ${GREEN}%s${RESET}\n", helpCommand, helpMessage; \
		} \
	} \
	{ lastLine = $$0 }' $(MAKEFILE_LIST)	

.PHONY: all
## Compile all 
all: $(TARGET)

.PHONY: pipe
## Compile pipe
pipe: pipe.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: fifo
## Compile fifo
fifo: fifo.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: socketpair
## Compile socketpair
socketpair: socketpair.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: uds
## Compile unix domain socket
uds: uds.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: udsd
## Compile unix domain socket
udsd: udsd.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: tcp
## Compile tcp
tcp: tcp.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: udp
## Compile udp
udp: udp.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: udpcs
## Compile udp
udpcs: udpcs.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: shm
## Compile shared memmory
shm: shm.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: test
## Run benchmark tests
test:
	./run_tests.sh

.PHONY: clean
## Clean build artifacts
clean:
	rm $(TARGET)
