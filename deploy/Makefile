all: prepare send

prepare:
	tar hcjf deploy .

send:
	scp deploy $(USER)@$(REMOTE):$(REMOTE_PATH)

clean:
	-rm deploy 2> /dev/null
