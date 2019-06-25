SRC_DIR = src
BUILD_DIR = build
CLIENT_DIR = client
LOG_DIR = logs

REQUEST_PKG = compute
SERVER_PKG = engine
CLIENT_PKG = client
SERVER_CLASS = Server
CLIENT_CLASS = Client
REQUEST_CLASS = Interface
# Java libraries
JLIB_DIR = /usr/share/java
JLIBS = $(JLIB_DIR)/mysql-connector-java-8.0.16.jar


.PHONY: build

build: $(BUILD_DIR)/$(SERVER_PKG)/$(SERVER_CLASS).class \
			 $(BUILD_DIR)/$(CLIENT_PKG)/$(CLIENT_CLASS).class

clean:
	rm -r $(BUILD_DIR)/* $(CLIENT_DIR)/* $(LOG_DIR)/*

run_server: $(BUILD_DIR)/$(SERVER_PKG)/$(SERVER_CLASS).class
	@mkdir -p $(BUILD_DIR) $(CLIENT_DIR) $(LOG_DIR)
	@java -cp $(BUILD_DIR):$(JLIBS) $(SERVER_PKG)/$(SERVER_CLASS)

run_client: $(BUILD_DIR)/$(CLIENT_PKG)/$(CLIENT_CLASS).class
# ifndef HOST
# 	$(error HOST is undefined)
# endif
	@mkdir -p $(BUILD_DIR) $(CLIENT_DIR) $(LOG_DIR)
	@java -cp $(BUILD_DIR) $(CLIENT_PKG)/$(CLIENT_CLASS)

$(BUILD_DIR)/$(REQUEST_PKG)/$(REQUEST_CLASS).class: \
	$(SRC_DIR)/$(REQUEST_PKG)/*.java
	javac -d $(BUILD_DIR) -sourcepath $(SRC_DIR) $^

$(BUILD_DIR)/$(SERVER_PKG)/$(SERVER_CLASS).class: \
	$(BUILD_DIR)/$(REQUEST_PKG)/$(REQUEST_CLASS).class \
	$(SRC_DIR)/$(SERVER_PKG)/*.java
	javac -cp $(JLIBS):$(BUILD_DIR) -d $(BUILD_DIR) -sourcepath $(SRC_DIR) $(filter-out $<,$^)

$(BUILD_DIR)/$(CLIENT_PKG)/$(CLIENT_CLASS).class: \
	$(BUILD_DIR)/$(REQUEST_PKG)/$(REQUEST_CLASS).class \
	$(SRC_DIR)/$(CLIENT_PKG)/*.java
	javac -cp $(JLIBS):$(BUILD_DIR) -d $(BUILD_DIR) -sourcepath $(SRC_DIR) $(filter-out $<,$^)
