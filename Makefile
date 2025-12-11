# Konpiladorea
CC = gcc
# Konpilazio eta Estekatze banderak hariak kudeatzeko (funtsezkoak zure proiektuan)
CFLAGS = -pthread

# Exekutagarriaren izena
TARGET = main

# Objektu fitxategiak. Make-ek aurkitzen ditu .c fitxategi guztiak eta .o bihurtzen ditu
OBJS = *.c

# Helburu nagusia: Exekutagarria sortu
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Garbiketa
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
