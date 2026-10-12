#!/bin/sh
# Polls the OTG UDC's enumeration state once a second and appends every transition to a log on
# /usr/data (real flash, ubifs, mounted sync) so the record survives the device losing power the
# instant it's unplugged from the car -- /tmp would be wiped before anyone could read it.
LOG=/usr/data/livi/usb_events.log
STATE_FILE=/sys/class/udc/13500000.otg_new/state

mkdir -p /usr/data/livi

prev=""
echo "$(cat /proc/uptime) START pid=$$" >> "$LOG"

while true; do
	cur=$(cat "$STATE_FILE" 2>/dev/null)
	if [ "$cur" != "$prev" ]; then
		echo "$(cat /proc/uptime) STATE ${prev:-<none>} -> ${cur:-<none>}" >> "$LOG"
		prev="$cur"
	fi
	sleep 1
done
