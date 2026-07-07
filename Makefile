# Macroboard build / flash helper.
# Encodes the board target and the custom USB identity so builds are
# reproducible without remembering flags:  make upload
#
#   make compile   - build with the custom identity baked in
#   make upload    - build, then flash the board
#   make monitor   - open the serial monitor
#   make identity  - print how the device will enumerate

FQBN   ?= arduino:avr:leonardo
PORT   ?= /dev/ttyACM0
SKETCH ?= Code

# --- Custom USB device identity ---------------------------------------------
# The pad enumerates with this name / VID:PID instead of "Arduino Leonardo"
# (2341:8036). VID 0x1209 is pid.codes' open vendor ID; 0x4D42 ("MB") is an
# unregistered test PID that's fine for personal use. Register a real PID at
# https://pid.codes if you ever distribute the board.
#
# NOTE: only the running sketch takes this identity. The bootloader keeps
# Arduino's VID, so uploads (and the udev rule matching 2341) still work.
USB_MFG  ?= KopyTKG
USB_PROD ?= Macroboard
USB_VID  ?= 0x1209
USB_PID  ?= 0x4D42

# The product/manufacturer values must reach gcc as quoted string literals,
# hence the escaped inner quotes.
IDENTITY := \
  --build-property "build.usb_manufacturer=\"$(USB_MFG)\"" \
  --build-property "build.usb_product=\"$(USB_PROD)\"" \
  --build-property build.vid=$(USB_VID) \
  --build-property build.pid=$(USB_PID)

.PHONY: compile upload monitor identity clean

# --clean forces a full rebuild: the USB identity lives in USBCore.cpp (part of
# the core), and arduino-cli's core cache is not keyed on the USB flags, so
# without --clean a cached core with Arduino's default identity gets reused.
compile:
	arduino-cli compile --clean --fqbn $(FQBN) $(IDENTITY) $(SKETCH)

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

monitor:
	arduino-cli monitor -p $(PORT)

identity:
	@echo "$(USB_PROD) by $(USB_MFG)  ($(USB_VID):$(USB_PID))"

clean:
	rm -rf $(SKETCH)/build
