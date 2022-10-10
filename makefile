all:
	$(MAKE) -C ./PollClientServer
	$(MAKE) -C ./ProcessClientServer
	$(MAKE) -C ./SelectClientServer
	$(MAKE) -C ./SequentialClientServer
	$(MAKE) -C ./ThreadedClientServer

clean:
	$(MAKE) -C ./PollClientServer clean
	$(MAKE) -C ./ProcessClientServer clean
	$(MAKE) -C ./SelectClientServer clean
	$(MAKE) -C ./SequentialClientServer clean
	$(MAKE) -C ./ThreadedClientServer clean