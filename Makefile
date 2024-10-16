# Utilisation de Bazelisk
BAZEL = bazelisk

init:
	$(BAZEL) run @hedron_compile_commands//:refresh_all

# Compilation client et serveur
build:
	$(BAZEL) build //src/client:client //src/server:server -- $(ARGS)

# Exécution
client1:
	$(BAZEL) run //src/client:client -- 127.0.0.1 1977 client1 password1 $(ARGS)

client2:
	$(BAZEL) run //src/client:client -- 127.0.0.1 1977 client2 password2 $(ARGS)

client3:
	$(BAZEL) run //src/client:client -- 127.0.0.1 1977 client3 password3 $(ARGS)

client4:
	$(BAZEL) run //src/client:client -- 127.0.0.1 1977 client4 password4 $(ARGS)

server:
	$(BAZEL) run //src/server:server -- 1977 /tmp/awale.db $(ARGS)

awale:
	$(BAZEL) run //src/awale:awale -- $(ARGS)

# Nettoyage
clean:
	$(BAZEL) clean